package output

import (
	"encoding/csv"
	"encoding/json"
	"fmt"
	"io"
	"text/tabwriter"

	"gitlab.com/slon/shad-go/gitfame/internal/stats"
)

func PrintResults(authorStats []stats.AuthorStats, format string, w io.Writer) error {
	switch format {
	case "tabular":
		return printTabular(authorStats, w)
	case "csv":
		return printCSV(authorStats, w)
	case "json":
		return printJSON(authorStats, w)
	case "json-lines":
		return printJSONLines(authorStats, w)
	default:
		return fmt.Errorf("unknown output format: %s", format)
	}
}

func printTabular(authorStats []stats.AuthorStats, w io.Writer) error {
	tw := tabwriter.NewWriter(w, 0, 8, 1, ' ', 0)

	fmt.Fprintf(tw, "Name\tLines\tCommits\tFiles\n")
	for _, as := range authorStats {
		fmt.Fprintf(tw, "%s\t%d\t%d\t%d\n",
			as.Name,
			as.Lines,
			as.Commits,
			as.Files,
		)
	}
	return tw.Flush()
}

func printCSV(authorStats []stats.AuthorStats, w io.Writer) error {
	writer := csv.NewWriter(w)
	if err := writer.Write([]string{"Name", "Lines", "Commits", "Files"}); err != nil {
		return err
	}
	for _, as := range authorStats {
		record := []string{
			as.Name,
			fmt.Sprintf("%d", as.Lines),
			fmt.Sprintf("%d", as.Commits),
			fmt.Sprintf("%d", as.Files),
		}
		if err := writer.Write(record); err != nil {
			return err
		}
	}
	writer.Flush()
	return writer.Error()
}

func printJSON(authorStats []stats.AuthorStats, w io.Writer) error {
	data, err := json.Marshal(authorStats)
	if err != nil {
		return err
	}
	_, err = w.Write(data)
	return err
}

func printJSONLines(authorStats []stats.AuthorStats, w io.Writer) error {
	encoder := json.NewEncoder(w)
	for _, as := range authorStats {
		if err := encoder.Encode(as); err != nil {
			return err
		}
	}
	return nil
}
