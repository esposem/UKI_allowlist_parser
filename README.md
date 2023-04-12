# UKI_allowlist_parser

Given a kernel command line string and an allowlist string, checks if each space-separated token in command line is accepted by the allowlist.
* If a single token is not accepted by allowlist, discard the whole command line.
* If (unlikely) there are more tokens than the ones allowed in memory by the parser, discard the command line
