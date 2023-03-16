#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
///   function   ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ssize_t parse_line(char *ln, ssize_t sz) {

    ssize_t i;
    int curly = 0;
    int round = 0;
    bool single_q = false;
    bool double_q = false;

    for (i = 0; i < sz; i++) {
	    switch (ln[i]) {
		case '#':
		    if (i == 0)
			return 0;
		    else {
			// the magic happens here:
			if (!single_q && !double_q)
			    if ((curly <= 0) && (round <= 0))
				if ((ln[i-1] != '$') && (ln[i-1] != '\\'))
				    return i;
		    }
		    break;
		case '\'':
		    single_q = !single_q;
		    break;
		case '\"':
		    double_q = !double_q;
		    break;
		case '{':
		    curly++;
		    break;
		case '}':
		    curly--;
		    break;
		case '(':
		    round++;
		    break;
		case ')':
		    round--;
		    break;
		default:
		    break;
	    }
    }
    return sz;
}

////////////////////////////////////////////////////////////////////////////////
///   main   ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {

    FILE *f;
    char *line = NULL;
    size_t len = 0;
    ssize_t size, cut;

    ////////////////////////////////////////////////////////////////////////////
    ///   open file   //////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    if (!isatty(STDIN_FILENO) || argc < 2 || (argv[1] == "-"))
        f = stdin;
    else
        if (!(f = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }

    ////////////////////////////////////////////////////////////////////////////
    //   first line is a special case: check for shebang '#!'   ////////////////
    ////////////////////////////////////////////////////////////////////////////

    if ((size = getline(&line, &len, f)) != -1) {


	if (line[0] == '#') {
	    if ((size > 3) && (line[1] == '!'))
		printf("%s", line);
	}
	else {
	    cut = parse_line(line, size);

	    // if no cut was made, print line exacly
	    if (cut == size)
		printf("%s", line);
	    else {
		// strip any trailing spaces
		while ((cut > 0) && isspace(line[cut-1]))
		    cut--;
		// do not print empty lines
		if (cut > 0)
		    printf("%.*s\n", cut, line);
	    }
	}

	////////////////////////////////////////////////////////////////////////
	//   process the remaining lines in a loop   ///////////////////////////
	////////////////////////////////////////////////////////////////////////

	while ((size = getline(&line, &len, f)) != -1) {

	    cut = parse_line(line, size);

	    // if no cut was made, print line exacly
	    if (cut == size)
		printf("%s", line);
	    else {
		// strip any trailing spaces
		while ((cut > 0) && isspace(line[cut-1]))
		    cut--;
		// do not print empty lines
		if (cut > 0)
		    printf("%.*s\n", cut, line);
	    }
	}
    }

    ////////////////////////////////////////////////////////////////////////////
    ///   finish   /////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    fclose(f);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}

