#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>


int main(int argc, char **argv) {

    FILE *f;
    ssize_t size;
    char *line = NULL;
    size_t len = 0;
    int i, cut;
    bool inside_single, inside_double, inside_curly, inside_round;

    if (!isatty(STDIN_FILENO) || argc < 2 || (argv[1] == "-")) {
        // using standard input
        f = stdin;
    } else {
        // try and open the first command line argument
        if (!(f = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }

    while ((size = getline(&line, &len, f)) != -1) {

	cut = size;

	inside_curly = false;
	inside_round = false;
	inside_single = false;
	inside_double = false;

	// process line backwards
	for (i = size-1; i >= 0; i--) {
	    switch (line[i]) {
		case '}':
		    inside_curly = true;
		    break;
		case '{':
		    inside_curly = false;
		    break;
		case ')':
		    inside_round = true;
		    break;
		case '(':
		    inside_round = false;
		    break;
		case '\'':
		    inside_single = !inside_single;
		    break;
		case '\"':
		    inside_double = !inside_double;
		    break;
		case '#':
		    if (!inside_single && !inside_double && !inside_curly && !inside_round)
			if (i>0) {
			    // do not treat \# and $# as comments
			    if ((line[i-1] != '\\') && (line[i-1] != '$'))
				cut = i;
			}
			else
			    cut=0;
		    break;
		default:
		    break;
	    }
	}

	// if no cut was made, print line exacly
	if (cut == size)
	    printf("%s", line);
	else {
	    // strip any trailing spaces
	    while ((cut > 0) && isspace(line[cut-1])) {
		cut--;
	}
	    // do not print modified line, if all that remains are whitespace characters
	   if (cut > 0)
		printf("%.*s\n", cut, line);
	}
    }

    fclose(f);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}

