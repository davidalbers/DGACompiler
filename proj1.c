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

struct node {
	char *id;
	struct node *next;
} *resWordsRoot, *symbolTableRoot;

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
void realMachine(char *);
void catchAllMachine(char *);
void newLineMachine(char *);
void addToSymbolTable(char *);
void loadReservedWords(char *);

int main(int argc, char *argv[]) 
{
	
	if(argc == 3) 
	{
		loadReservedWords("reservedWords.txt");
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
			
			while(i < numberLines) 
			{
				getNextToken(lines[i]);
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
			lines[lineNumber][linePos] = charAtPos;
			printf("got new line '%c'\n", charAtPos);
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

void loadReservedWords(char *pathToReservedWords) {
	FILE *reservedWords = fopen(pathToReservedWords, "r"); 
	if(reservedWords != NULL){
		char c = 0;
		int index = 0;
		char *resWord = malloc(sizeof(char) * 32);
		while(fscanf(reservedWords,"%c",&c) != EOF ) {
			if(c == ',' || c == '\n' && index > 0) {
				addToSymbolTable(resWord);
				resWord = malloc(sizeof(char) * 32);
				index = 0;
			}
			else if (c != ' '){
				resWord[index] = c;
				index++;
			}
		}
	}
	else {
		perror("Could not open reserved words file");
	}	
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
			case 7:
				realMachine(line);
				break;
			case 15:
				newLineMachine(line);
				if(fsa.state == 0)
					return;
				break;
			default:
				catchAllMachine(line);
				break;
		}
	}
}

void whitespaceMachine(char *line) {
	while(fsa.f < MAX_LINE_LENGTH) { 
		char ws = line[fsa.f];
		fsa.f++;
		if(ws == ' ' || ws == '\t') {
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
	char *id = malloc(sizeof(char) * 10);
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
					fsa.state = 0;
					printf("got an id %s\n", id);
					addToSymbolTable(id);
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
					fsa.state = 0;
					return;
				}
				else {
					fsa.f--;
					fsa.state = 7;
					return;
				}
				break;
			case 5:
				if(relop == '=') 
					puts("relop found <=");
				else if(relop == '>') 
					puts("relop found <>");
				else {
					puts("relop found <");
					fsa.f--;
				}
				fsa.state = 0;
				return;
			case 6:
				if(relop == '=') 
					puts("relop found >=");
				else {
					puts("relop found >");
					fsa.f--;
				}
				fsa.state = 0;
				return;	
		}	
	}
}


void realMachine(char *line) {
	char *d = malloc(sizeof(char) * 10);
	char *yy = malloc(sizeof(char) * 5);
	char *zz = malloc(sizeof(char) * 2);
	int dCount = 0;
	int yyCount = 0;
	int zzCount = 0;
	int lengthError = 0;
	//assumed to be positive unless said otherwise
	int negative = 0; 
	while(fsa.f < MAX_LINE_LENGTH) {
		char c = line[fsa.f];
		fsa.f++;
		// printf("%dreal machine read %c\n",fsa.state, c);
		switch(fsa.state) 
		{
			case 7:
				if(isdigit(c)) {
					d[dCount] = c;
					dCount++;
					fsa.state = 8;
				}
				else {
					printf("real machine got error1 %c\n",c);
					//todo: this sends it to catchall, find a more intuitive way to do this
					fsa.state = 15; 
					fsa.f--;
					return;
				}
				break;
			case 8:
				if(isdigit(c)) {
					if(dCount >= 10) {
						
						lengthError = 1;
					}
					d[dCount] = c;
					dCount++;
				}
				else if(c == '.') {
					fsa.state = 9;
				}
				else if(c == 'E') {
					fsa.state = 11;
				}
				else {
					if(lengthError) {
						puts("real machine got length error");
					}
					else {
						printf("real machine got int %s\n", d);
						
					}
					fsa.f--;
					fsa.state = 0;
					return;
				}
				break;
			case 9:
				if(isdigit(c)) {
					yy[yyCount] = c;
					yyCount++;
					fsa.state = 10;
				}
				else {
					fsa.f--;
					printf("real machine got error2 %c\n", c);
					fsa.state = 100;
					return;
				}
				break;
			case 10:
				if(isdigit(c)) {
					if(yyCount >= 5) {
						lengthError = 1;
					}
					else {
						yy[yyCount] = c;
						yyCount++;
					}
				}
				else if(c == 'E') {
					fsa.state = 11;
				}
				else {
					if(lengthError) {
						puts("real machine got length error");
					}
					else {
						printf("real machine got decimal %s.%s\n", d,yy);
						
					}
					fsa.f--;
					fsa.state = 0;
					return;
				}
				break;
			case 11:
				if(isdigit(c)) {
					fsa.state = 12;
					zz[zzCount] = c;
					zzCount++;
				}
				else if(c == '+' || c == '-') {
					if(c == '-')
						negative = 1;
					fsa.state = 14;

				}
				else {
					printf("real machine got error3 %c\n", c);
					fsa.state = 100;
					fsa.f--;
					return;
				}
				break;
			case 12:
				if(isdigit(c)) {
					fsa.state = 13;
					zz[zzCount] = c;
					zzCount++;
				}
				else {
					if(lengthError) {
						puts("real machine got length error");
					}
					else {
						printf("real machine got real %s.%se%s\n",d,yy,zz);
					}
					fsa.f--;
					fsa.state = 0;
					return;
				}
				break;
			case 13:
				if(isdigit(c)) {
					lengthError = 1;
				}
				else {
					if(lengthError) {
						puts("real machine got length error");
					}
					else {
						printf("real machine got real %s.%se%s\n",d,yy,zz);
					}
					fsa.f--;
					fsa.state = 0;
					return;
				}
				break;
			case 14:
				if(isdigit(c)) {
					zz[zzCount] = c;
					zzCount++;
					fsa.state = 12;
				}
				else {
					printf("real machine got error5 %c\n", c);
					fsa.state = 100;
					fsa.f--;
					return;
				}
				break;
		}
	}
}

void newLineMachine(char* line) {
	char c = line[fsa.f];
	fsa.f++;
	if(c == '\n') {
		puts("found new line");
		fsa.state = 0;
		fsa.f = 0;	
	}
	else {
		fsa.f--;
		fsa.state = 100;
	}
}

void catchAllMachine(char* line) {
	char c = line[fsa.f];
	fsa.f++;
	fsa.state = 0;
	printf("catchall read %c%d\n", c, fsa.f);
}

void addToReservedWords(char *id) {
	if(resWordsRoot == NULL) {
		resWordsRoot = (struct node *) malloc(sizeof(struct node));
		printf("id '%s' added to resWordsRoot of symbol table\n",id);
		resWordsRoot->next = 0;
		resWordsRoot->id = id;
	}
	else {
		struct node *currNode = resWordsRoot;
		if(!strcmp(id,resWordsRoot->id)) {
			printf("id '%s' already exists in resWordsRoot of symbol table as '%s'\n",id,resWordsRoot->id);
			return;
		}
		while(currNode->next != 0) {
			currNode = currNode->next;
			if(!strcmp(id,currNode->id)) {
				printf("id '%s' already exists in symbol table\n",id);
				return; //found id in table, don't add
			}
		}
		//not found, add a new node to list
		struct node *newNode = (struct node *) malloc(sizeof(struct node));
		newNode->id = id;
		newNode->next = 0;
		printf("adding %s to symbol table\n",id);
		currNode->next = newNode;
	}
}

void addToSymbolTable(char* id) {
	if(symbolTableRoot == NULL) {
		symbolTableRoot = (struct node *) malloc(sizeof(struct node));
		printf("id '%s' added to symbolTableRoot of symbol table\n",id);
		symbolTableRoot->next = 0;
		symbolTableRoot->id = id;
	}
	else {
		struct node *currNode = symbolTableRoot;
		if(!strcmp(id,symbolTableRoot->id)) {
			printf("id '%s' already exists in symbolTableRoot of symbol table as '%s'\n",id,symbolTableRoot->id);
			return;
		}
		while(currNode->next != 0) {
			currNode = currNode->next;
			if(!strcmp(id,currNode->id)) {
				printf("id '%s' already exists in symbol table\n",id);
				return; //found id in table, don't add
			}
		}
		//not found, add a new node to list
		struct node *newNode = (struct node *) malloc(sizeof(struct node));
		newNode->id = id;
		newNode->next = 0;
		printf("adding %s to symbol table\n",id);
		currNode->next = newNode;
	}
}
	
