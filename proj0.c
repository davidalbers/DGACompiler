#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 72

extern int errno;

void sourceToListing(char *, char *);
char** readFileLineByLine(FILE *, int);
int fileLineCount(FILE *);
int fileLength(FILE *);

int main(int argc, char *argv[]) 
{
	if(argc == 3) 
	{
		sourceToListing(argv[1],argv[2]);
	}
	else
	{
		puts("Invalid number of arguments. Valid syntax is 'proj0 /path/to/source /path/to/listing'");
		return -1;
	}
	puts("Program finished");
	return 0;
}

void sourceToListing(char *sourcePath, char *listingPath) 
{
	FILE *src = fopen(sourcePath, "r"); 
	if(src != NULL)
	{
		FILE *listing = fopen(listingPath, "w");
		if(listing != NULL) 
		{
			int numberLines = fileLineCount(src);
			char **lines = readFileLineByLine(src, numberLines);	
			int i = 0;
			//output lines of file to in form of:
			//1:	int i = 0
			while(i < numberLines) 
			{
				if(i == numberLines - 1)
					fprintf(listing, "%d:\t%sEOF", (i+1), lines[i]);
				else 
					fprintf(listing, "%d:\t%s\n", (i+1), lines[i]);
				i++;
			}
		}
		else 
		{
			perror("Could not open listing file!");
		}
	}
	else
	{
		perror("Could not open source file!");
	}
}

char **readFileLineByLine(FILE *source, int lineCount) 
{
	char **lines = malloc(sizeof(char) * lineCount * MAX_LINE_LENGTH);
	lines[0] = malloc(sizeof(char) * MAX_LINE_LENGTH);
	char charAtPos;
	int lineNumber = 0;
	int linePos = 0;
	//iterate over each char and place it in the appropriate line
	//stop at EOF
	while(fscanf(source, "%c", &charAtPos) != EOF) 
	{
		if(charAtPos == '\n') 
		{
			//found new line
			lineNumber++;
			linePos = 0;
			lines[lineNumber] = malloc(sizeof(char) * MAX_LINE_LENGTH);		
		}
		else 
		{
			lines[lineNumber][linePos] = charAtPos;
			linePos++;
		}
	}
	return lines;
}

int fileLineCount(FILE *src) 
{
	char charAtPos;
	int lineCount = 0;
	while(fscanf(src, "%c", &charAtPos) != EOF) 
	{
		if(charAtPos == '\n')
			lineCount++;
	}
	rewind(src);
	return lineCount;
}
	
