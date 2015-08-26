#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) 
{
	if(argc == 3) 
	{
		FILE *src = fopen(argv[1], "r"); 
		char line[73] = {0};
		if(src != NULL)
		{
			FILE *dest = fopen(argv[2], "w");
			if(dest != NULL) 
			{
				fseek (src, 0, SEEK_END);
				int srcLength = ftell (src);
				rewind (src);

				int linePos = 0;
				int filePos = 0;
				while(filePos < srcLength) 
				{
					while(linePos < 72 && filePos < srcLength) 
					{
						char charAtPos;
						fscanf(src, "%c", &charAtPos);
						linePos++;
						filePos++;
						if(charAtPos == '\n') {
							break;
						}	
						line[linePos] = charAtPos;
					}
					line[72] = '\0';
					printf("1:\t%s", line);
					linePos = 0;
					memset(line, 0, 72); //zero out array
				}
			}
			else 
			{
				puts("Could not open destination file!");
			}
		}
		else
		{
			puts("Could not open source file!");
			exit(1);
		}
	}
	else
	{
		puts("Invalid number of arguments. Valid syntax is 'proj0 /path/to/source /path/to/destination");
		exit(1);
	}
	puts("end of program");
}