package filters

import (
	"fmt"
	"io"
	"path/filepath"
	"strings"

	"gitlab.com/slon/shad-go/gitfame/internal/languages"
)

type Config struct {
	Extensions []string
	Languages  []string
	Exclude    []string
	RestrictTo []string
}

func Apply(files []string, cfg Config, langMap map[string][]string, warnWriter io.Writer) []string {
	result := files
	if len(cfg.Extensions) > 0 {
		result = filterByExtensions(result, cfg.Extensions)
	}

	if len(cfg.Languages) > 0 {
		result = filterByLanguages(result, cfg.Languages, langMap, warnWriter)
	}

	if len(cfg.Exclude) > 0 {
		result = excludeByGlobs(result, cfg.Exclude)
	}

	if len(cfg.RestrictTo) > 0 {
		result = restrictByGlobs(result, cfg.RestrictTo)
	}
	return result
}

func filterByExtensions(files []string, exts []string) []string {
	// Silly little optimization
	extSet := make(map[string]struct{}, len(exts))
	for _, e := range exts {
		extSet[strings.ToLower(e)] = struct{}{}
	}

	var result []string
	for _, f := range files {
		ext := filepath.Ext(f)
		ext = strings.ToLower(ext)
		if _, ok := extSet[ext]; ok {
			result = append(result, f)
		}
	}
	return result
}

func filterByLanguages(files []string, userLangs []string, langMap map[string][]string, warnWriter io.Writer) []string {
	var result []string
	warnSet := make(map[string]struct{})
	for _, f := range files {
		ext := filepath.Ext(f)
		ext = strings.ToLower(ext)

		ok, warns := languages.IsExtensionInLanguageSet(ext, userLangs, langMap)
		for _, w := range warns {
			warnSet[w] = struct{}{}
		}
		if ok {
			result = append(result, f)
		}
	}
	for w := range warnSet {
		if warnWriter != nil {
			fmt.Fprintln(warnWriter, w)
		}
	}
	return result
}

func excludeByGlobs(files []string, excludeList []string) []string {
	var result []string
	for _, f := range files {
		exclude := false
		for _, pattern := range excludeList {
			match, _ := filepath.Match(pattern, f)
			if match {
				exclude = true
				break
			}
		}
		if !exclude {
			result = append(result, f)
		}
	}
	return result
}

func restrictByGlobs(files []string, restrictList []string) []string {
	var result []string
	for _, f := range files {
		if anyMatch(f, restrictList) {
			result = append(result, f)
		}
	}
	return result
}

func anyMatch(path string, patterns []string) bool {
	for _, pat := range patterns {
		match, _ := filepath.Match(pat, path)
		if match {
			return true
		}
	}
	return false
}
