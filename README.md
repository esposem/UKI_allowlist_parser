# UKI_allowlist_parser

Given a kernel command line string and an allowlist string, checks if each space-separated* token in command line is accepted by the allowlist.
* If a single token is not accepted by allowlist, discard the whole command line.
* If (unlikely) there are more tokens than the ones allowed in memory by the parser, discard the command line


## Test next_param()

Make sure that the function is correctly splitting strings by space*.
Therefore, a fuzzer creates strings of random length (max `MAX_STR_LEN`) and throws
random ascii characters at it. As soon a space* is met, we consider the parameter/token
to be finished.
Perform `NEXT_PARAM_ITERATIONS` with random strings and make sure they are correctly split.

## Test check_cmdline()

Now that we are sure that tokens are correctly separated and spaces* are not considered,
make sure the parser does its job when checking the three different types of parameter:
1) parameter
2) parameter=
3) parameter=value

Case 1) and 3) are simply hardcoded strings. If the command line
argument differs from length or content from them, it is not a match.
Case 2) covers parameters where we don't know what the value could be.
Since we don't want globbing or regex that could be potentially
exploited to add unwanted arguments, 2) allows any character that is not
defined as space*. For this case, the matching function compares up to
'=', but does not check what comes after.
Note that `allowlist` is considered trusted, despite being parsed just like
the untrusted command line.



*= with space it is meant ' ', '\n', '\r' and '\t'
