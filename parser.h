#ifndef _H_UKI_ALLOWLIST_PARSER
#define _H_UKI_ALLOWLIST_PARSER

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define char16_t char

struct str_token {
        const char16_t *param;
        size_t len; /* \0 not included */
};

bool my_isspace(char16_t c);
bool next_param(const char16_t **start, struct str_token *found);
bool check_cmdline(const char16_t *cmdline, const char16_t *allowlist);

#endif