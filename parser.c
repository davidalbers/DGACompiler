#include <stdio.h>
#include <stdlib.h>
#include "lex.h"

int main(int argc, char *argv[]) 
{
	if(argc == 2) 
	{
		loadFiles(argv[1]);
		
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
	tok = getToken();
	//match end of file
}

void sign() {
	switch(tok) 
	{
		case 
	}
}
