package languages

import (
	_ "embed"
	"encoding/json"
	"fmt"
)

//go:embed language_extensions.json
var embeddedLanguageData []byte

func LoadEmbeddedLanguages() ([]LanguageDefinition, error) {
	var langs []LanguageDefinition
	if err := json.Unmarshal(embeddedLanguageData, &langs); err != nil {
		return nil, fmt.Errorf("failed to parse embedded data: %w", err)
	}
	return langs, nil
}
