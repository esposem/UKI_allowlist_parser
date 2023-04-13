#include <time.h>
#include "parser.h"

#define MAX_STR_LEN 1000
#define ASCII_LAST 127
#define NEXT_PARAM_ITERATIONS 100000

#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))

/** create_full_string
 * Allocates and populates a string @budget chars long, starting with separator (at least
 * a space) if @start_sep == true and with a parameter otherwise.
 * Both parameter and separator lengths are random. The chars chosen for separator
 * and parameters are random.
 *
 * The function alternates parameter with separators, and returns the amount of
 * parameters created. @sol is then changed to point to the allocated string,
 * that must be freed by the caller. @solution instead keeps track of the beginning
 * and length of each parameter, that will be then used as truth source to compare
 * with the output of next_param().
*/
static int create_full_string(int budget, bool start_sep, const char **sol, struct str_token *solution) {
	char *str = malloc(budget+1);
	int chosen = 0, ret = -1;
	bool is_first_char = true;
	int i = 0;

	str[budget] = '\0';

	/* Throw at least a space as separator */
	if (start_sep && budget > 0) {
		str[0] = ' ';
		i = 1;
	}

	for (; i < budget; i++) {
		str[i] = (char) ((rand() % (ASCII_LAST)) + 1); // ignore 0, include 127
		if (my_isspace(str[i])) {
			is_first_char = true;
		} else {
			if (is_first_char) {
				ret++;
				is_first_char = false;
				solution[ret].param = str+i;
				solution[ret].len = 1;
			} else {
				solution[ret].len++;
			}
		}
	}

	*sol = str;
	return ret+1;
}

static void compare_tokens(const char *str, struct str_token *solution, int len_sol) {
	struct str_token tmp;
	int i = 0;
	while (i < len_sol) {
		assert(next_param(&str, &tmp));
		// printf("solution %d points to %u (%p) len %zu\n", i, *solution->param, solution->param, solution->len);
		// printf("proposal %d points to %u (%p) len %zu\n", i, *tmp.param, tmp.param, tmp.len);
		assert(!memcmp(solution, &tmp, sizeof(struct str_token)));
		solution++;
		i++;
	}

	assert(*str == '\0' || my_isspace(*str));
	assert(!next_param(&str, &tmp));
}

enum string_beginning {
	RANDOM,
	SEPARATOR,
	PARAMETER
};

static void _test_next_param(int len, int start_type, struct str_token *solution) {
	int len_sol;
	bool start_sep;
	const char *str;

	switch (start_type) {
	case RANDOM:
		start_sep = rand() %2;
		break;
	case SEPARATOR:
		start_sep = true;
		break;
	case PARAMETER:
		start_sep = false;
		break;
	default:
		assert(false);
		break;
	}

	len_sol = create_full_string(len, start_sep, &str, solution);
	compare_tokens(str, solution, len_sol);
	free((char *) str);
}

static void test_next_param(void) {
	struct str_token *solution = malloc(sizeof(struct str_token) * (MAX_STR_LEN/2 + 1));
	int i = 0;

	_test_next_param(0, RANDOM, solution);
	_test_next_param(1, SEPARATOR, solution);
	_test_next_param(1, PARAMETER, solution);
	_test_next_param(2, SEPARATOR, solution);
	_test_next_param(2, PARAMETER, solution);
	for (i = 0; i < NEXT_PARAM_ITERATIONS; i++) {
		_test_next_param(rand() % MAX_STR_LEN+1, RANDOM, solution);
	}
	free(solution);
	printf("Test next_param() PASSED\n");
}

/* any combination cmdline/allowlist using these is valid */
static const char *fix_correct[] = {
	"asd", " asd", "asd ", " asd ",
};

static void test_fix_correct(void) {
	int i,j;

	for (i=0; i < ARRAY_SIZE(fix_correct); i++) {
		for (j=0; j < ARRAY_SIZE(fix_correct); j++) {
			assert(check_cmdline(fix_correct[i], fix_correct[j]));
		}
	}
}

/* a combination cmdline/allowlist using these is valid only if the index is the same */
static const char *fix_incorrect[] = {
	"aaaa", " aa", "aaa ", " a ",
};

static void test_fixed_args(void)
{
	int i,j;

	for (i=0; i < ARRAY_SIZE(fix_incorrect); i++) {
		for (j=0; j < ARRAY_SIZE(fix_incorrect); j++) {
			if (i != j)
				assert(!check_cmdline(fix_incorrect[i], fix_incorrect[j]));
			else
				assert(check_cmdline(fix_incorrect[i], fix_incorrect[j]));
		}
	}
}

static bool simple_allowlist_checker(const char *allowlist_param, const char *cmdline_param) {
	size_t alen = strlen(allowlist_param);
	size_t clen = strlen(cmdline_param);
	if (allowlist_param[alen-1] != '=' && clen != alen)
		return false;
	if (clen < alen)
		return false;
	return memcmp(allowlist_param, cmdline_param, alen) == 0;
}

static void _test_check_cmdline(const char *allowlist_param, const char *cmdline_param) {
	bool sol = simple_allowlist_checker(allowlist_param, cmdline_param);
	bool res = check_cmdline(cmdline_param, allowlist_param);

	assert(sol == res);
}

static const char *allowlist_args[] = {
	"arg", "xxxarg", "argxxx", "asdasd",
	"arg=", "xxxarg=", "argxxx=", "asdasd=",
	"arg=123", "arg=1234", "arg=4", "arg=1",
};

static void test_allowlist_args(void)
{
	int i,j;

	for (i=0; i < ARRAY_SIZE(allowlist_args); i++) {
		for (j=0; j < ARRAY_SIZE(allowlist_args); j++) {
			_test_check_cmdline(allowlist_args[i], allowlist_args[j]);
		}
	}
}

static const char *command_line_args[] = {
	"arg", "arg=", " arg==", "arg=sthing", "arg==sthing",
	"arg=arg=arg", "arg=arg=arg=", "arg=arg=arg==", "arg=arg==arg", "arg=arg==arg=",
	"arg=sthing=sthing", "arg=sthing=sthing=", "arg=sthing=sthing==", "arg=sthing==sthing", "arg=sthing==sthing=",
	"arg=arg=sthing", "arg=arg=sthing=", "arg=arg=sthing==", "arg=arg==sthing", "arg=arg==sthing=",
	"arg=sthing=arg", "arg=sthing=arg=", "arg=sthing=arg==", "arg=sthing==arg", "arg=sthing==arg=",
	"=", "==", "=123", "1", "=arg", "=arg=", "==arg", "==arg=="
};

static void test_cmdline_args(void)
{
	int i,j;

	for (i=0; i < ARRAY_SIZE(command_line_args); i++) {
		for (j=0; j < ARRAY_SIZE(allowlist_args); j++) {
			_test_check_cmdline(command_line_args[i], allowlist_args[j]);
		}
	}
}

static void test_check_cmdline(void) {

	test_fix_correct();
	test_fixed_args();
	test_allowlist_args();
	test_cmdline_args();
	printf("Test check_cmdline() PASSED\n");
}

int main(int argc, char const *argv[]) {
	srand(time(NULL));
	test_next_param();
	test_check_cmdline();
	return 0;
}
