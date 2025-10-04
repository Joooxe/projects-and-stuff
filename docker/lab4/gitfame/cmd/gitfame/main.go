//go:build !solution

package main

import (
	"fmt"
	"os"
	"strings"

	"github.com/spf13/cobra"

	"gitlab.com/slon/shad-go/gitfame/internal/blame"
	"gitlab.com/slon/shad-go/gitfame/internal/filters"
	"gitlab.com/slon/shad-go/gitfame/internal/languages"
	"gitlab.com/slon/shad-go/gitfame/internal/output"
	"gitlab.com/slon/shad-go/gitfame/internal/repo"
	"gitlab.com/slon/shad-go/gitfame/internal/stats"
)

var (
	repository   string
	revision     string
	orderBy      string
	useCommitter bool
	format       string

	extensionsArg string
	languagesArg  string
	excludeArg    string
	restrictArg   string

	showProgress bool
)

var rootCmd = &cobra.Command{
	Use:   "gitfame",
	Short: "gitfame - a tool for counting lines, commits, and files by author in a Git repo",
	Long: `gitfame is a command-line tool for analyzing Git repositories and producing 
author-based statistics: number of lines, commits, and files touched by each author (or committer)`,
	RunE: func(cmd *cobra.Command, args []string) error {
		if err := validateFlags(); err != nil {
			return err
		}
		if err := run(); err != nil {
			return err
		}
		return nil
	},
}

func init() {
	rootCmd.Flags().StringVar(&repository, "repository", ".", "path to Git repository")
	rootCmd.Flags().StringVar(&revision, "revision", "HEAD", "git revision")
	rootCmd.Flags().StringVar(&orderBy, "order-by", "lines", "sort key: lines, commits, files")
	rootCmd.Flags().BoolVar(&useCommitter, "use-committer", false, "use committer instead of author")
	rootCmd.Flags().StringVar(&format, "format", "tabular", "output format: tabular, csv, json, json-lines")

	rootCmd.Flags().StringVar(&extensionsArg, "extensions", "", "comma-separated list of file extensions (e.g. '.go,.md')")
	rootCmd.Flags().StringVar(&languagesArg, "languages", "", "comma-separated list of languages (e.g. 'go,markdown')")
	rootCmd.Flags().StringVar(&excludeArg, "exclude", "", "comma-separated list of glob patterns to exclude (e.g. 'foo/*,bar/*')")
	rootCmd.Flags().StringVar(&restrictArg, "restrict-to", "", "comma-separated list of glob patterns to restrict to (e.g. '*.go,*.md')")

	rootCmd.Flags().BoolVar(&showProgress, "show-progress", false, "show progress in stderr (optional)")
}

func main() {
	if err := rootCmd.Execute(); err != nil {
		fmt.Fprintf(os.Stderr, "%v\n", err)
		os.Exit(1)
	}
}

func validateFlags() error {
	switch orderBy {
	case "lines", "commits", "files":
	default:
		return fmt.Errorf("invalid --order-by value '%s'", orderBy)
	}

	switch format {
	case "tabular", "csv", "json", "json-lines":
		// Norm, pass
	default:
		return fmt.Errorf("invalid --format value '%s'", format)
	}
	return nil
}

func run() error {
	if err := repo.CheckRepo(repository); err != nil {
		return fmt.Errorf("%w not a repo", err)
	}

	if !repo.RevisionExists(repository, revision) {
		return fmt.Errorf("revision '%s' does not exist", revision)
	}

	allFiles, err := repo.ListFiles(repository, revision)
	if err != nil {
		return err
	}
	if showProgress {
		fmt.Fprintf(os.Stderr, "Found %d files in repo\n", len(allFiles))
	}

	langDefs, err := languages.LoadEmbeddedLanguages()
	if err != nil {
		return fmt.Errorf("failed to load language map: %w", err)
	}
	langMap := languages.BuildLanguageMap(langDefs)

	exts := splitComma(extensionsArg)
	langs := splitComma(languagesArg)
	excl := splitComma(excludeArg)
	restr := splitComma(restrictArg)

	fcfg := filters.Config{Extensions: exts, Languages: langs, Exclude: excl, RestrictTo: restr}
	filteredFiles := filters.Apply(allFiles, fcfg, langMap, os.Stderr)
	if showProgress {
		fmt.Fprintf(os.Stderr, "After filtering: %d files\n", len(filteredFiles))
	}

	var allChunks []blame.BlameChunk
	for i, path := range filteredFiles {
		if path == "" {
			continue
		}
		if showProgress {
			fmt.Fprintf(os.Stderr, "Blaming war crimes on file %d/%d: %s\n", i+1, len(filteredFiles), path)
		}

		chunks, err := blame.BlameFile(repository, revision, path, useCommitter)
		if err != nil {
			fmt.Fprintf(os.Stderr, "warning: blame failed for '%s': %v\n", path, err)
			continue
		}
		allChunks = append(allChunks, chunks...)
	}

	authorStats := stats.Calculate(allChunks)
	stats.Sort(authorStats, orderBy)
	if err := output.PrintResults(authorStats, format, os.Stdout); err != nil {
		return err
	}
	return nil
}

func splitComma(s string) []string {
	if s == "" {
		return nil
	}
	parts := strings.Split(s, ",")
	var result []string
	for _, p := range parts {
		p = strings.TrimSpace(p)
		if p != "" {
			result = append(result, p)
		}
	}
	return result
}
