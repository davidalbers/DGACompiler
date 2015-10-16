#include <stdio.h>
#include <stdlib.h>
#include "lex.h"

struct token *tok;
void parse();
void sign();
void match();

int main(int argc, char *argv[]) 
{
	if(argc == 2) 
	{
		loadFiles(argv[1]);
		parse();
	}
	else
	{
		puts("Invalid number of arguments. Valid syntax is 'proj1 /path/to/source'");
		return -1;
	}
	puts("Program finished");
	return 0;
}

void parse() {
	tok = getNextToken();
	sign();
	match();
	//match end of file
}

void sign() {
	switch(tok->attribute->attrInt) {
		case ADD:
			match(ADD);
			puts("add");
			break;
		case SUBTRACT:
			match(SUBTRACT);
			break;
		default:
			//error
			break;
	}
}

void match() {
	//?
}
