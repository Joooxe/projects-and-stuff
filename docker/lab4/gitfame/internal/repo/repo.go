package repo

import (
	"errors"
	"os/exec"
	"strings"
)

var ErrNotARepo = errors.New("invalid git repository")

func CheckRepo(repoPath string) error {
	cmd := exec.Command("git", "rev-parse", "HEAD")
	cmd.Dir = repoPath
	if err := cmd.Run(); err != nil {
		return ErrNotARepo
	}
	return nil
}

func ListFiles(repoPath, revision string) ([]string, error) {
	cmd := exec.Command("git", "ls-tree", "-r", "--name-only", revision)
	cmd.Dir = repoPath

	output, err := cmd.Output()
	if err != nil {
		return nil, err
	}

	lines := strings.Split(string(output), "\n")

	var files []string
	for _, line := range lines {
		f := strings.TrimSpace(line)
		files = append(files, f)
	}

	return files, nil
}

func RevisionExists(repoPath, revision string) bool {
	cmd := exec.Command("git", "rev-parse", "--verify", revision+"^{commit}")
	cmd.Dir = repoPath

	err := cmd.Run()
	return err == nil
}
