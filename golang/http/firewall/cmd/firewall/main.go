//go:build !solution

package main

import (
	"bytes"
	"flag"
	"io"
	"net/http"
	"os"
	"regexp"
	"strings"

	"gopkg.in/yaml.v2"
)

func writeForbidden(w http.ResponseWriter) {
	w.WriteHeader(http.StatusForbidden)
	_, _ = w.Write([]byte("Forbidden"))
}

type yamlRule struct {
	Endpoint               string   `yaml:"endpoint"`
	ForbiddenUserAgents    []string `yaml:"forbidden_user_agents"`
	ForbiddenHeaders       []string `yaml:"forbidden_headers"`
	RequiredHeaders        []string `yaml:"required_headers"`
	MaxRequestLengthBytes  *int     `yaml:"max_request_length_bytes"`
	MaxResponseLengthBytes *int     `yaml:"max_response_length_bytes"`
	ForbiddenResponseCodes []int    `yaml:"forbidden_response_codes"`
	ForbiddenRequestRE     []string `yaml:"forbidden_request_re"`
	ForbiddenResponseRE    []string `yaml:"forbidden_response_re"`
}
type yamlConfig struct {
	Rules []yamlRule `yaml:"rules"`
}

type headerRule struct {
	Name string
	Re   *regexp.Regexp
}

type rule struct {
	Endpoint        string
	ForbidUA        []*regexp.Regexp
	ForbidHeaders   []headerRule
	RequireHeaders  []string
	MaxReqLen       *int
	MaxRespLen      *int
	ForbidRespCodes map[int]struct{}
	ForbidReqRE     []*regexp.Regexp
	ForbidRespRE    []*regexp.Regexp
}

func parseRules(path string) ([]rule, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	var rc yamlConfig
	if err := yaml.Unmarshal(data, &rc); err != nil {
		return nil, err
	}
	var out []rule
	for _, rr := range rc.Rules {
		r := rule{
			Endpoint:        rr.Endpoint,
			RequireHeaders:  rr.RequiredHeaders,
			MaxReqLen:       rr.MaxRequestLengthBytes,
			MaxRespLen:      rr.MaxResponseLengthBytes,
			ForbidRespCodes: make(map[int]struct{}),
		}
		for _, code := range rr.ForbiddenResponseCodes {
			r.ForbidRespCodes[code] = struct{}{}
		}
		for _, pat := range rr.ForbiddenUserAgents {
			re := regexp.MustCompile(pat)
			r.ForbidUA = append(r.ForbidUA, re)
		}
		for _, hd := range rr.ForbiddenHeaders {
			parts := strings.SplitN(hd, ":", 2)
			name := strings.TrimSpace(parts[0])
			re := regexp.MustCompile(strings.TrimSpace(parts[1]))
			r.ForbidHeaders = append(r.ForbidHeaders, headerRule{name, re})
		}
		for _, pat := range rr.ForbiddenRequestRE {
			r.ForbidReqRE = append(r.ForbidReqRE, regexp.MustCompile(pat))
		}
		for _, pat := range rr.ForbiddenResponseRE {
			r.ForbidRespRE = append(r.ForbidRespRE, regexp.MustCompile(pat))
		}
		out = append(out, r)
	}
	return out, nil
}

func main() {
	serviceAddr := flag.String("service-addr", "", "bruh")
	addr := flag.String("addr", "", "bruh")
	conf := flag.String("conf", "", "bruh")
	flag.Parse()

	rules, err := parseRules(*conf)
	if err != nil {
		panic(err)
	}

	client := http.DefaultClient

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		var sub []rule
		for _, ru := range rules {
			if ru.Endpoint == r.URL.Path {
				sub = append(sub, ru)
			}
		}

		ua := r.Header.Get("User-Agent")

		bodyReq, _ := io.ReadAll(r.Body)
		for _, ru := range sub {
			// 1)
			for _, re := range ru.ForbidUA {
				if ua != "" && re.MatchString(ua) {
					writeForbidden(w)
					return
				}
			}
			// 2)
			for _, hdr := range ru.ForbidHeaders {
				for _, v := range r.Header.Values(hdr.Name) {
					if hdr.Re.MatchString(v) {
						writeForbidden(w)
						return
					}
				}
			}
			// 3)
			for _, name := range ru.RequireHeaders {
				if r.Header.Get(name) == "" {
					writeForbidden(w)
					return
				}
			}
			// 4)
			if ru.MaxReqLen != nil && len(bodyReq) > *ru.MaxReqLen {
				writeForbidden(w)
				return
			}
			// 5)
			s := string(bodyReq)
			for _, re := range ru.ForbidReqRE {
				if re.MatchString(s) {
					writeForbidden(w)
					return
				}
			}
		}

		upURL := strings.TrimRight(*serviceAddr, "/") + r.URL.Path
		if q := r.URL.RawQuery; q != "" {
			upURL += "?" + q
		}

		reqUp, _ := http.NewRequest(r.Method, upURL, bytes.NewReader(bodyReq))
		reqUp.Header = r.Header.Clone()

		respUp, err := client.Do(reqUp)
		if err != nil {
			writeForbidden(w)
			return
		}

		bodyResp, _ := io.ReadAll(respUp.Body)
		for _, ru := range sub {
			// 6)
			if _, bad := ru.ForbidRespCodes[respUp.StatusCode]; bad {
				writeForbidden(w)
				return
			}
			// 7)
			if ru.MaxRespLen != nil && len(bodyResp) > *ru.MaxRespLen {
				writeForbidden(w)
				return
			}
			// 8)
			s := string(bodyResp)
			for _, re := range ru.ForbidRespRE {
				if re.MatchString(s) {
					writeForbidden(w)
					return
				}
			}
		}

		for k, vv := range respUp.Header {
			for _, v := range vv {
				w.Header().Add(k, v)
			}
		}
		w.WriteHeader(respUp.StatusCode)
		_, _ = w.Write(bodyResp)
	})

	if err := http.ListenAndServe(*addr, nil); err != nil {
		panic(err)
	}
}
