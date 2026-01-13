args	"-F poly -c g.c -H g.h -uCOEFFICIENT --func-name ggo --show-required --default-optional --no-help --no-version -G"

package "poly"
version "1.0.0"

description	"Print the solutions to polynomial with COEFFICIENTs\nIf no COEFFICIENTs, read from standard input.\n\nThe first COEFFICIENT is assumed to be of the highest order term."

section "Options"
option	"loose-exit-status" l "Exit with 0 even if invalid input encountered"
option 	"reverse" r "Assume coefficients given in increasing term order"
option	"silent" s "Don't print error messages"
option	"verbose" V "Also output input polynomial"
section	"Getting help"
option	"help" h "Print this help message and exit"
option	"version" v "Print version information and exit"
text	"\nTry 'man poly' for more information."

versiontext	"Copyright (C) 2026 Jack Renton Uteg.\nLicense GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\n\nWritten by Jack R. Uteg."
