/*
 * Utility functions.
 *
 * @author: Jose Carlos Martinez Garcia-Vaso <carlosgvaso@gmail.com>
 */

#include <main.h>


const char CMD_TOKEN_DELIM = ' ';	// From requirements


/*
 * Parse a command into strings.
 *
 * Read the man page for strtok.
 *
 * TODO: Implement.
 */
char** parseString(char* str) {
	int len = 0;
	char** tokens = (char**) malloc(MAX_TOKEN_NUM * sizeof(char*));


	// Remove final newline char and replace with NULL char
	len = strlen(str);
	if (str[len-1] == '\n') {
		str[len-1] = '\0';
	}

	// Break down the command into tokens using strtok
	tokens[0] = strtok(str, &CMD_TOKEN_DELIM);
	//tokens[0] = token;
	int count = 1;	// Start at one because we already run strtok once
	while ((tokens[count] = strtok(NULL, &CMD_TOKEN_DELIM))) {
		//tokens[count] = token;
		count++;
	}

	return tokens;
}
