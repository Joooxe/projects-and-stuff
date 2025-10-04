#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

enum {
    MAX_ARGS_COUNT = 256,
    MAX_CHAIN_LINKS_COUNT = 256
};

typedef struct {
    char* command;
    uint64_t argc;
    char* argv[MAX_ARGS_COUNT];
} chain_link_t;

typedef struct {
    uint64_t chain_links_count;
    chain_link_t chain_links[MAX_CHAIN_LINKS_COUNT];
} chain_t;

void create_chain(char* command, chain_t* chain) {
    char* saveptr;
    char* token = strtok_r(command, "|", &saveptr);
    while (token && chain->chain_links_count < MAX_CHAIN_LINKS_COUNT) {
        char* arg = strtok(token, " ");
        chain->chain_links[chain->chain_links_count].command = arg;
        while (arg && chain->chain_links[chain->chain_links_count].argc < MAX_ARGS_COUNT - 1) {
            chain->chain_links[chain->chain_links_count].argv[chain->chain_links[chain->chain_links_count].argc] = arg;
            ++chain->chain_links[chain->chain_links_count].argc;
            arg = strtok(NULL, " ");
        }
        ++chain->chain_links_count;

        token = strtok_r(NULL, "|", &saveptr);
    }
}

void run_chain(chain_t* chain) {
    if (chain->chain_links_count == 0) {
        return;
    }

    int pipes[MAX_CHAIN_LINKS_COUNT - 1][2];
    for (uint64_t i = 0; i < chain->chain_links_count - 1; ++i) {
        if (pipe(pipes[i]) < 0) {
            exit(1);
        }
    }

    for (uint64_t i = 0; i < chain->chain_links_count; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            exit(1);
        }

        if (pid == 0) {
            if (i < chain->chain_links_count - 1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) < 0) {
                    exit(1);
                }
            }

            if (i > 0) {
                if (dup2(pipes[i-1][0], STDIN_FILENO) < 0) {
                    exit(1);
                }
            }

            for (uint64_t j = 0; j < chain->chain_links_count - 1; ++j) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(chain->chain_links[i].command, chain->chain_links[i].argv);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 1;
    }
    chain_t chain;
    create_chain(argv[1], &chain);
    run_chain(&chain);
}
