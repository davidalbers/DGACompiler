#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 72;
int main(int argc, char *argv[]) 
{
	if(argc == 3) 
	{
		FILE *src = fopen(argv[1], "r"); 
		//lines can only be 72 chars long
		char line[MAX_LINE_LENGTH] = {0};
		if(src != NULL)
		{
			FILE *dest = fopen(argv[2], "w");
			if(dest != NULL) 
			{
				//determine total file size
				fseek (src, 0, SEEK_END);
				int srcLength = ftell (src);
				rewind (src);

				int linePos = 0;
				int filePos = 0;
				int lineNumber = 1;
				//iterate line by line 
				while(filePos < srcLength) 
				{
					//iterate character by character
					//go to end of line or end of file
					while(linePos < MAX_LINE_LENGTH && filePos < srcLength) 
					{
						char charAtPos;
						fscanf(src, "%c", &charAtPos);
						if(charAtPos == '\n') {
							linePos++;
							filePos++;
							break;
						}	
						
						line[linePos] = charAtPos;
						linePos++;
						filePos++;
					}
					//write to dest file
					fprintf(dest, "%d:\t%s\n", lineNumber, line);
					linePos = 0;
					memset(line, 0, MAX_LINE_LENGTH); //zero out array
					lineNumber++;
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
		puts("Invalid number of arguments. Valid syntax is 'proj0 /path/to/source /path/to/destination'");
		exit(1);
	}
	puts("end of program");
}