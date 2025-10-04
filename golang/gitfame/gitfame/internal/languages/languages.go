package languages

import (
	"encoding/json"
	"fmt"
	"os"
	"strings"
)

type LanguageDefinition struct {
	Name       string
	Type       string
	Extensions []string
}

func LoadLanguageExtensions(jsonFilePath string) ([]LanguageDefinition, error) {
	data, err := os.ReadFile(jsonFilePath)
	if err != nil {
		return nil, fmt.Errorf("failed to read language file %s: %w", jsonFilePath, err)
	}

	var langs []LanguageDefinition
	if err := json.Unmarshal(data, &langs); err != nil {
		return nil, fmt.Errorf("failed to parse language file %s: %w", jsonFilePath, err)
	}

	return langs, nil
}

func BuildLanguageMap(langs []LanguageDefinition) map[string][]string {
	result := make(map[string][]string)
	for _, ld := range langs {
		result[strings.ToLower(ld.Name)] = ld.Extensions
	}
	return result
}

func IsExtensionInLanguageSet(extension string, userLangs []string, langMap map[string][]string) (bool, []string) {
	extension = strings.ToLower(extension)
	var warnings []string

	for _, ul := range userLangs {
		langLower := strings.ToLower(ul)
		exts, ok := langMap[langLower]
		if !ok {
			warning := fmt.Sprintf("warning: language '%s' does not exist", ul)
			warnings = append(warnings, warning)
			continue
		}

		for _, ext := range exts {
			if ext == extension {
				return true, warnings
			}
		}
	}

	return false, warnings
}
