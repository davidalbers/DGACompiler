#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 72

struct FSA {
	int f;
	int b;
	int state;	
} fsa;

extern int errno;


void sourceToListing(char *, char *);
char** readFileLineByLine(FILE *, int);
int fileLineCount(FILE *);
int fileLength(FILE *);
char **readFileLineByLine(FILE *, int);
void idMachine(char *);
void whitespaceMachine(char *);
void getNextToken(char *);
void relopMachine(char *);

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
			getNextToken(lines[0]);
			while(i < numberLines) 
			{
				i++;
			}
			i = 0;
			//output lines of file to in form of:
			//1:	int i = 0
			while(i < numberLines) 
			{
				if(i == numberLines - 1)
					printf("%d:\t%sEOF", (i+1), lines[i]);
				else 
					printf("%d:\t%s\n", (i+1), lines[i]);
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
	

void getNextToken(char *line) {
	fsa.state = 0;
	while(fsa.f < MAX_LINE_LENGTH) {
		switch(fsa.state) 
		{
			case 0:
				whitespaceMachine(line);
				break;
			case 2:
				idMachine(line);
				break;
			case 4:
				relopMachine(line);
				break;
		}
	}
}

void whitespaceMachine(char *line) {
	while(fsa.f < MAX_LINE_LENGTH) { 
		char ws = line[fsa.f];
		fsa.f++;
		if(ws == ' ' || ws == '\t' || ws == '\n') {
			fsa.state = 1;
		}
		else {
			//block
			fsa.state = 2;
			fsa.f--;
			return;
		}
		
	}
}

void idMachine(char *line) {
	char id[10] = {0};
	int idLength = 0;
	while(fsa.f < MAX_LINE_LENGTH && idLength < 10) {
		char c = line[fsa.f];
		fsa.f++;
		switch(fsa.state) 
		{
			case 2:
				if(isalpha(c)) {
					id[idLength] = c;
					idLength++;
					fsa.state = 3;
				}
				else {
					fsa.f--;
					fsa.state = 4;
					printf("id machine block %s\n", id);
					return;
				}
				break;
			case 3:
				if(isalpha(c) || isdigit(c)) {
					id[idLength] = c;
					idLength++;
				}
				else {
					fsa.f--;
					//got an id
					fsa.state = 0;
					printf("got an id %s\n", id);
					return;
				}
				break;
		}	
	}
}

void relopMachine(char* line) {
	while(fsa.f < MAX_LINE_LENGTH) {
		char relop = line[fsa.f];
		fsa.f++;
		switch(fsa.state) 
		{
			case 4:
				if(relop == '<') 
					fsa.state = 5;
				else if(relop == '>') 
					fsa.state = 6;
				else if(relop == '=') {
					puts("relop found equals");
					fsa.f--;
					fsa.state = 0;
					return;
				}
				else {
					puts("relop: not a relop");
					fsa.f--;
					return;
				}
				break;
			case 5:
				if(relop == '=') 
					puts("relop found <=");
				else if(relop == '>') 
					puts("relop found <>");
				else 
					puts("relop found <");
				fsa.f--;
				fsa.state = 0;
				return;
			case 6:
				if(relop == '=') 
					puts("relop found >=");
				else 
					puts("relop found >");
				fsa.f--;
				fsa.state = 0;
				return;	
		}	
	}
}
