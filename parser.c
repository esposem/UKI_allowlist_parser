#include "parser.h"

#define MAX_CMDLINE_PARAMS 1024
#define MAX_ALLOWLIST_PARAMS 1024

static int strncmp16(const char16_t *s1, const char16_t *s2, size_t len) {
	return memcmp(s1, s2, len);
}

bool my_isspace(char16_t c) {
        return c == ' ' || c == '\r' || c == '\t' || c == '\n';
}

bool next_param(const char16_t **start, struct str_token *found) {
        const char16_t *ptr = NULL, *el = *start;

        while (*el != '\0') {
                if (my_isspace(*el)) {
                        if (ptr)
                                break;
                } else {
                        if (!ptr)
                                ptr = el;
                }
                el++;
        }

        *start = el;
        if (ptr) {
                found->param = ptr;
                found->len = el - ptr;
                return true;
        }
        return false;
}

static int match(struct str_token *cmd, struct str_token *alw, bool is_hardcoded) {
        if (is_hardcoded) {
                /* it's useless if length differ */
                if (alw->len != cmd->len)
                        return false;
        }
        if (cmd->len < alw->len)
                return false;
        return strncmp16(cmd->param, alw->param, alw->len) == 0;
}

static bool match_with_allowlist(const char16_t *allowlist_str, struct str_token *cmd) {
        struct str_token alw;

        while (next_param(&allowlist_str, &alw)) {
                if (match(cmd, &alw, *(allowlist_str-1) != '='))
                        return true;
        }

        return false;
}

bool check_cmdline(const char16_t *cmdline, const char16_t *allowlist) {
        struct str_token cmd = {};

        if (!cmdline || !allowlist || *cmdline == '\0' || *allowlist == '\0')
                return false;

        while (next_param(&cmdline, &cmd)) {
                if(!match_with_allowlist(allowlist, &cmd))
                        return false;
        }

        return true;
}