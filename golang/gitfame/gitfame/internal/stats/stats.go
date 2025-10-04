package stats

import (
	"sort"
	"strings"

	"gitlab.com/slon/shad-go/gitfame/internal/blame"
)

type AuthorStats struct {
	Name    string `json:"name"`
	Lines   int    `json:"lines"`
	Commits int    `json:"commits"`
	Files   int    `json:"files"`
}

func Calculate(chunks []blame.BlameChunk) []AuthorStats {
	// to mark unique commits/files by SHA/filename and count author lines
	type helper struct {
		lines   int
		commits map[string]struct{}
		files   map[string]struct{}
	}

	helperMap := make(map[string]*helper)

	for _, chunk := range chunks {
		name := chunk.AuthorName
		if _, ok := helperMap[name]; !ok {
			helperMap[name] = &helper{0, make(map[string]struct{}), make(map[string]struct{})}
		}
		helperMap[name].lines += chunk.LinesCount
		helperMap[name].commits[chunk.CommitSHA] = struct{}{}
		helperMap[name].files[chunk.FileName] = struct{}{}
	}

	result := make([]AuthorStats, 0, len(helperMap))
	for author, ac := range helperMap {
		as := AuthorStats{
			author,
			ac.lines,
			len(ac.commits),
			len(ac.files),
		}
		result = append(result, as)
	}
	return result
}

func Sort(authorStats []AuthorStats, orderBy string) {
	sort.Slice(authorStats, func(i, j int) bool {
		switch orderBy {
		case "commits":
			if authorStats[i].Commits != authorStats[j].Commits {
				return authorStats[i].Commits > authorStats[j].Commits
			}
			if authorStats[i].Lines != authorStats[j].Lines {
				return authorStats[i].Lines > authorStats[j].Lines
			}
			if authorStats[i].Files != authorStats[j].Files {
				return authorStats[i].Files > authorStats[j].Files
			}
		case "files":
			if authorStats[i].Files != authorStats[j].Files {
				return authorStats[i].Files > authorStats[j].Files
			}
			if authorStats[i].Lines != authorStats[j].Lines {
				return authorStats[i].Lines > authorStats[j].Lines
			}
			if authorStats[i].Commits != authorStats[j].Commits {
				return authorStats[i].Commits > authorStats[j].Commits
			}
		case "lines":
			fallthrough
		default:
			if authorStats[i].Lines != authorStats[j].Lines {
				return authorStats[i].Lines > authorStats[j].Lines
			}
			if authorStats[i].Commits != authorStats[j].Commits {
				return authorStats[i].Commits > authorStats[j].Commits
			}
			if authorStats[i].Files != authorStats[j].Files {
				return authorStats[i].Files > authorStats[j].Files
			}
		}
		return strings.ToLower(authorStats[i].Name) < strings.ToLower(authorStats[j].Name)
	})
}
