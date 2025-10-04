package blame

import (
	"bufio"
	"bytes"
	"fmt"
	"os/exec"
	"strings"
)

type BlameChunk struct {
	CommitSHA  string
	AuthorName string
	LinesCount int
	FileName   string
}

func BlameFile(repoPath, revision, filePath string, useCommitter bool) ([]BlameChunk, error) {
	cmd := exec.Command("git", "blame", "--porcelain", revision, "--", filePath)
	cmd.Dir = repoPath

	output, err := cmd.Output()
	if err != nil {
		return nil, err
	}
	if len(output) == 0 {
		return blameEmptyFile(repoPath, revision, filePath, useCommitter)
	}
	return parsePorcelainBlame(output, filePath, useCommitter)
}

func parsePorcelainBlame(porcelainData []byte, filePath string, useCommitter bool) ([]BlameChunk, error) {
	var chunks []BlameChunk
	scanner := bufio.NewScanner(bytes.NewReader(porcelainData))

	lastAuthorForCommit := make(map[string]string)

	var currentCommit, currentAuthor, currentCommitter string
	var currentLinesCount int
	inBlock := false

	flushBlock := func() {
		if !inBlock {
			return
		}
		authorName := pickAuthorOrCommitter(currentAuthor, currentCommitter, useCommitter)
		if authorName == "" {
			if last, ok := lastAuthorForCommit[currentCommit]; ok {
				authorName = last
			}
		}
		if authorName != "" {
			chunks = append(chunks, BlameChunk{
				currentCommit,
				authorName,
				currentLinesCount,
				filePath,
			})
			lastAuthorForCommit[currentCommit] = authorName
		}
		currentCommit = ""
		currentAuthor = ""
		currentCommitter = ""
		currentLinesCount = 0
		inBlock = false
	}

	for scanner.Scan() {
		line := scanner.Text()
		if len(line) == 0 {
			flushBlock()
			continue
		}

		fields := strings.Fields(line)
		// "<commit> <oldLine> <newLine> <numLines>"
		if len(fields) == 4 {
			newCommit := fields[0]
			newCount, err := parseInt(fields[3])
			if err != nil {
				newCount = 0
			}

			if inBlock && (newCommit == currentCommit) {
				currentLinesCount += newCount
			} else {
				if inBlock {
					flushBlock()
				}

				currentCommit = newCommit
				currentLinesCount = newCount
				currentAuthor = ""
				currentCommitter = ""
				inBlock = true
			}
			continue
		}

		if strings.HasPrefix(line, "author ") {
			if currentAuthor == "" {
				currentAuthor = strings.TrimPrefix(line, "author ")
			}
			continue
		}
		if strings.HasPrefix(line, "committer ") {
			if currentCommitter == "" {
				currentCommitter = strings.TrimPrefix(line, "committer ")
			}
			continue
		}
	}
	flushBlock()

	if err := scanner.Err(); err != nil {
		return chunks, fmt.Errorf("failed to parse blame output: %w", err)
	}

	return chunks, nil
}

func pickAuthorOrCommitter(author, committer string, useCommitter bool) string {
	if useCommitter {
		if committer != "" {
			return committer
		}
		return author
	}

	if author != "" {
		return author
	}
	return committer
}

func blameEmptyFile(repoPath, revision, filePath string, useCommitter bool) ([]BlameChunk, error) {
	var logFormat string
	// %H - full SHA, %an - author name, %cn - committer name
	if useCommitter {
		logFormat = "%H %cn"
	} else {
		logFormat = "%H %an"
	}

	cmd := exec.Command("git", "log", "-n", "1", "--pretty="+logFormat, revision, "--", filePath)
	cmd.Dir = repoPath

	out, err := cmd.Output()
	if err != nil {
		return nil, nil
	}

	line := strings.TrimSpace(string(out))
	idx := strings.IndexByte(line, ' ')
	commit := line[:idx]
	name := strings.TrimSpace(line[idx+1:])

	chunk := BlameChunk{commit, name, 0, filePath}
	return []BlameChunk{chunk}, nil
}

func parseInt(s string) (int, error) {
	var x int
	var err error
	_, err = fmt.Sscanf(s, "%d", &x)
	return x, err
}
