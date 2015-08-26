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
	//determine total file size of source
	int srcLength = fileLength(source);
	char **lines = malloc(sizeof(char) * lineCount * MAX_LINE_LENGTH);
	int linePos = 0;
	int filePos = 0;
	int lineNumber = 0;
	//iterate line by line 
	while(filePos < srcLength) 
	{
		char *line = malloc(sizeof(char) * MAX_LINE_LENGTH);
		//iterate character by character
		//go to end of line or end of file
		while(linePos < MAX_LINE_LENGTH && filePos < srcLength) 
		{
			char charAtPos;
			fscanf(source, "%c", &charAtPos);
			if(charAtPos == '\n') 
			{
				linePos++;
				filePos++;
				break; //found new line
			}	
			//store char in line
			line[linePos] = charAtPos;
			linePos++;
			filePos++;
		}
		lines[lineNumber] = line;
		linePos = 0;
		lineNumber++;
	}
	return lines;
}

int fileLineCount(FILE *src) 
{
	int srcLength = fileLength(src);
	int filePos = 0;
	int linePos = 0;
	int lineCount = 0;
	//iterate line by line
	while(filePos < srcLength) 
	{
		//iterate character by character
		//go to end of line or end of file
		while(linePos < MAX_LINE_LENGTH && filePos < srcLength) 
		{
			char charAtPos;
			fscanf(src, "%c", &charAtPos);
			filePos++;
			linePos++;
			if(charAtPos == '\n')
			{
				break; //found new line
			}		
		}
		linePos = 0;
		lineCount++;
	}
	return lineCount;
}

int fileLength(FILE *src) 
{
	fseek (src, 0, SEEK_END); //go to end
	int srcLength = ftell (src); //get position at end
	rewind (src);
	return srcLength;
}
