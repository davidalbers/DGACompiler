#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 72
#define RESERVED 0
#define ID 30
#define NUM 31
#define RELOP 32
#define ADDOP 33
#define MULOP 34
#define ASSIGNOP 35
#define OPENPAREN 36
#define CLOSEPAREN 37
#define COMMA 38
#define SEMICOLON 39
#define COLON 40
#define PERIOD 41
#define LEXERR 99
#define INTTOOLONG 2
#define UNRECOGNIZEDNUM 3
#define DECIMALTOOLONG 4
#define REALTOOLONG 5
#define LEADINGZEROES 6
#define LEADINGZEROESEXP 7
#define TRAILINGZERO 8
#define LESSTHAN 9
#define LESSTHANEQUAL 10
#define NOTEQUAL 11
#define GREATERTHAN 12
#define GREATERTHANEQUAL 13
#define EQUAL 14
#define ADD 15
#define SUBTRACT 16
#define OR 17
#define MULTIPLY 18
#define DIVIDE 19
#define DIV 20
#define MOD 21
#define AND 22
#define UNRECOGNIZEDSYMBOL 23
#define PROGRAM 24
#define VAR 25
#define INTEGER 26
#define FUNCTION 27
#define BEGIN 28
#define END 29
#define THEN 30
#define IF 31
#define ELSE 32
#define IDTOOLONG 33



union Attribute {
	char *attrString;
	int attrInt;
};

struct token {
	int tokenName;
	union Attribute *attribute;
};

struct FSA {
	int f;
	int b;
	int state;	
	struct token *currToken;
} fsa;

struct node {
	char *id;
	struct node *next;
} *symbolTableRoot;

struct resWordNode{
	char *id;
	int tokenName;
	int attr;
	struct resWordNode *next;
	int empty;
} *resWordsRoot;

struct tokenNode {
	struct token *thisToken;
	struct tokenNode *next;
} *tokenNodeRoot;

extern int errno;

FILE *listingFile;
FILE *tokenFile;
int tokenizingLine = 0;

void sourceToListing(char *);
char** readFileLineByLine(FILE *, int);
int fileLineCount(FILE *);
int fileLength(FILE *);
char **readFileLineByLine(FILE *, int);
void idMachine(char *);
void whitespaceMachine(char *);
int getNextToken(char *);
void relopMachine(char *);
void realMachine(char *);
void catchAllMachine(char *);
void newLineMachine(char *);
void addToSymbolTable(char *);
void loadReservedWords(char *);
void addToReservedWords(char *, int, int);
int checkInResWordTable(char *);
struct resWordNode *findInResWordTable(char *);
void addopMachine(char *);
void mulopMachine(char *);
void assignopMachine(char *);
void intMachine(char *);
char *tokenNameToString(int);
char *attributeToString(int);
void printToken(char *);
void addToken();

int main(int argc, char *argv[]) 
{
	
	if(argc == 2) 
	{
		loadReservedWords("reservedWords.txt");
		sourceToListing(argv[1]);
	}
	else
	{
		puts("Invalid number of arguments. Valid syntax is 'proj1 /path/to/source'");
		return -1;
	}
	puts("Program finished");
	return 0;
}

void sourceToListing(char *sourcePath) 
{
	FILE *src = fopen(sourcePath, "r"); 
	if(src != NULL)
	{
		listingFile = fopen("listing.txt", "w");
		tokenFile = fopen("token.txt", "w");
		if(listingFile != NULL && tokenFile != NULL) 
		{
			int numberLines = fileLineCount(src);
			char **lines = readFileLineByLine(src, numberLines);
			fprintf(tokenFile,"LINE\t%-12s\t%-25s\tLEXEME\n", "TOKEN-TYPE", "ATTRIBUTE");
			while(tokenizingLine < numberLines) 
			{
				printf("%d:\t%s",(tokenizingLine+1), lines[tokenizingLine]);
				fprintf(listingFile, "%d:\t%s",(tokenizingLine + 1), lines[tokenizingLine] );
				while(getNextToken(lines[tokenizingLine]) == 0) {

				}
				tokenizingLine++;
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
		int tokenType = -1;
		int attr = -1;
		while(fscanf(reservedWords,"%s %d %d", resWord, &tokenType, &attr) != EOF) {
			addToReservedWords(resWord, tokenType, attr);
		}
	}
	else {
		perror("Could not open reserved words file");
	}	
}
	
//Return 1 at end of line
//Return 0 if token or error
int getNextToken(char *line) {
	fsa.state = 0;
	
	while(fsa.f < MAX_LINE_LENGTH) {
		switch(fsa.state) 
		{
			case 0:
				// puts("whitespaceMachine");
				fsa.f = fsa.b; 
				whitespaceMachine(line);
				break;
			case 2:
				// puts("idMachine");
				fsa.f = fsa.b; 
				idMachine(line);
				break;
			case 4:
				//puts("relopMachine");
				fsa.f = fsa.b; 
				relopMachine(line);
				break;
			case 7:
				// puts("realMachine");
				fsa.f = fsa.b; 
				realMachine(line);
				break;
			case 15:
				// puts("newLineMachine");
				fsa.f = fsa.b; 
				newLineMachine(line);
				if(fsa.state == 0)
					return 1;
				break;
			case 16:
				// puts("addop machine");
				fsa.f = fsa.b; 
				addopMachine(line);
				break;
			case 17:
				//puts("mulopMachine");
				fsa.f = fsa.b; 
				mulopMachine(line);
				break;
			case 18:
				//puts("assignop machine");
				fsa.f = fsa.b; 
				assignopMachine(line);
				break;
			case 20:
				fsa.f = fsa.b; 
				//puts("int machine");
				intMachine(line);
				break;
			case 101:
				goto Cleanup;
				Cleanup: ; //this is an empty statement so I can declare a variable
				
				char *lexeme = malloc(sizeof(char) * (fsa.f - fsa.b));
			
				strncpy(lexeme, line+fsa.b, (fsa.f-fsa.b));
				
				fsa.b = fsa.f;
				printToken(lexeme);
				return 0;
			default:
				fsa.f = fsa.b; 
				catchAllMachine(line);
				break;
		}
	}
	return 0;
}

void whitespaceMachine(char *line) {
	while(fsa.f < MAX_LINE_LENGTH) { 
		char ws = line[fsa.f];
		fsa.f++;	
		if(ws == ' ' || ws == '\t') {
			fsa.state = 1;
			fsa.b = fsa.f;
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
	int idTooLong = 0;
	while(fsa.f < MAX_LINE_LENGTH) {
		char c = line[fsa.f];
		// printf("idMachine got '%c'\n",c);
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
					idLength++;
					if(idLength > 10)
						idTooLong = 1;
					else 
						id[idLength - 1] = c;
				}
				else {
					fsa.f--;
					fsa.state = 101;
					if(idTooLong) {
						struct token *idToken = (struct token *) malloc(sizeof(struct token ));
						union Attribute *attr = (union Attribute *) malloc(sizeof(union Attribute));
						idToken->tokenName = LEXERR;
						attr->attrInt = IDTOOLONG;
						idToken->attribute = attr;
						fsa.currToken = idToken;
						return;
					}
					struct resWordNode *resWord = findInResWordTable(id);
					if(resWord->empty != 1) {
						// printf("id is a reserved word '%s,%d,%d'\n", id, resWord->tokenName, resWord->attr);
						struct token *idToken = (struct token *) malloc(sizeof(struct token ));
						union Attribute *attr = (union Attribute *) malloc(sizeof(union Attribute));
						attr->attrInt = resWord->attr;
						idToken->tokenName = resWord->tokenName;
						idToken->attribute = attr;
						fsa.currToken = idToken;
						return;
					}
					addToSymbolTable(id);
					struct token *idToken = malloc(sizeof(struct token));
					union Attribute *attr = malloc(sizeof(union Attribute));
			
					attr->attrString = id;
					idToken->tokenName = ID;
					idToken->attribute = attr;
					fsa.currToken = idToken;
					return;
				}
				break;
		}	
	}
}

void relopMachine(char* line) {
	
	while(fsa.f < MAX_LINE_LENGTH) {
		struct token *relopToken = malloc(sizeof(struct token));
		union Attribute *attr = malloc(sizeof(union Attribute));
		char relop = line[fsa.f];
		 // printf("relopMachine got '%c'\n",relop);
		fsa.f++;
		switch(fsa.state) 
		{
			case 4:
				if(relop == '<') 
					fsa.state = 5;
				else if(relop == '>') 
					fsa.state = 6;
				else if(relop == '=') {
					relopToken->tokenName = RELOP;
					attr->attrInt = EQUAL;
					relopToken->attribute = attr;
					fsa.currToken = relopToken;
					fsa.state = 101;
					return;
				}
				else {
					fsa.f--;
					fsa.state = 7;
					return;
				}
				break;
			case 5:
				//any character here results in a relop
				relopToken->tokenName = RELOP;
				if(relop == '=') {
					attr->attrInt = LESSTHANEQUAL;
				}
				else if(relop == '>') {
					attr->attrInt = NOTEQUAL;
				}
				else {
					fsa.f--; //'<' + another, non relop, character, so just <
					attr->attrInt = LESSTHAN;
				}
				relopToken->attribute = attr;
				fsa.currToken = relopToken;
				fsa.state = 101;
				return;
			case 6:
				relopToken->tokenName = RELOP;
				if(relop == '=') 
					attr->attrInt = GREATERTHANEQUAL;
				if(relop != '=') {
					attr->attrInt = GREATERTHAN;
					fsa.f--;//'>'+ another, non relop, character, so just >
				}
				relopToken->attribute = attr;
				fsa.currToken = relopToken;
				fsa.state = 101;
				return;	
		}	
	}
}


void realMachine(char *line) {
	char *d = malloc(sizeof(char) * 5);
	char *yy = malloc(sizeof(char) * 5);
	char *zz = malloc(sizeof(char) * 2);
	char *num = malloc(sizeof(char) * 17);
	int dCount = 0;
	int yyCount = 0;
	int zzCount = 0;
	int intLengthError = 0;
	int decimalLengthError = 0;
	int realLengthError = 0;
	//assumed to be positive unless said otherwise
	int negative = 0; 
	int leadingZero = 0;
	int leadingZeroError = 0;
	int zzLeadingZero = 0;
	int zzLeadingZeroError = 0;
	int yyTrailingZero = 0;
	while(fsa.f < MAX_LINE_LENGTH) {
		char c = line[fsa.f];
		// printf("realMachine got '%c'\n",c);
		fsa.f++;
		// printf("%dreal machine read %c\n",fsa.state, c);
		//todo add leading zero check
		switch(fsa.state) 
		{
			case 7:
				if(isdigit(c)) {
					d[dCount] = c;
					dCount++;
					fsa.state = 8;
					if(c == '0')
						leadingZero = 1;
				}
				else {
					fsa.state = 15; 
					fsa.f--;
					return;
				}
				break;
			case 8:
				if(isdigit(c)) {
					dCount++;
					if(dCount > 5) {
						intLengthError = 1;
					}
					else {
						if(leadingZero) //leading zero + anything else is an error
							leadingZeroError = 1;
						d[dCount - 1] = c;
					}
				}
				else if(c == '.') {
					fsa.state = 9;
				}
				else if(c == 'E') {
					fsa.state = 11;
				}
				else {
					fsa.state = 20; //go to intmachine
					fsa.f = fsa.b;
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
					struct token *errToken = (struct token *)malloc(sizeof(struct token));
					errToken->tokenName = LEXERR;
					union Attribute *attr = (union Attribute *)malloc(sizeof(union Attribute));
					attr->attrInt = UNRECOGNIZEDNUM;
					errToken->attribute = attr;
					fsa.currToken = errToken;
					fsa.state = 101;
					return;
				}
				break;
			case 10:
				if(isdigit(c)) {
					yyCount++;
					if(c == '0')
						yyTrailingZero = 1;
					else
						yyTrailingZero = 0;
					if(yyCount > 5) {
						decimalLengthError = 1;
					}
					else {
						yy[yyCount - 1] = c;
					}
				}
				else if(c == 'E') {
					fsa.state = 11;
				}
				else {
					struct token *numToken = (struct token *)malloc(sizeof(struct token));
					union Attribute *attr = (union Attribute *)malloc(sizeof(union Attribute));
					if(intLengthError) {
						
						numToken->tokenName = LEXERR;
						attr->attrInt = INTTOOLONG;
	
					}
					else if(decimalLengthError) {
						numToken->tokenName = LEXERR;
						attr->attrInt = DECIMALTOOLONG;
					}
					else if (leadingZeroError) {
						numToken->tokenName = LEXERR;
						attr->attrInt = LEADINGZEROES;
					}
					else if(yyTrailingZero) {
						numToken->tokenName = LEXERR;
						attr->attrInt = TRAILINGZERO;
					}
					else {
						numToken->tokenName = NUM;
						strcpy(num,d);
						strcat(num,".");
						strcat(num,yy);
						attr->attrString = num;
				
					}
					numToken->attribute = attr;
					fsa.currToken = numToken;
					fsa.state = 101;
					fsa.f--;
					return;
				}
				break;
			case 11:
				if(isdigit(c)) {
					fsa.state = 12;
					zz[zzCount] = c;
					if(c == '0')
						zzLeadingZero = 1;
					zzCount++;
				}
				else if(c == '+' || c == '-') {
					if(c == '-')
						negative = 1;
					fsa.state = 14;

				}
				else {
					struct token *errToken = (struct token *)malloc(sizeof(struct token));
					errToken->tokenName = LEXERR;
					union Attribute *attr = (union Attribute *)malloc(sizeof(union Attribute));
					attr->attrInt = UNRECOGNIZEDNUM;
					errToken->attribute = attr;
					fsa.currToken = errToken;
					fsa.state = 101;
					fsa.f--;
					return;
				}
				break;
			case 12:
				if(isdigit(c)) {
					if(zzLeadingZero) {
						zzLeadingZeroError = 1;
					}
					fsa.state = 13;
					zz[zzCount] = c;
					zzCount++;
				}
				else {
					struct token *numToken = (struct token *)malloc(sizeof(struct token));
					union Attribute *attr = (union Attribute *)malloc(sizeof(union Attribute));
					if(intLengthError) {
						numToken->tokenName = LEXERR;
						attr->attrInt = INTTOOLONG;
					}
					else if(decimalLengthError) {
						numToken->tokenName = LEXERR;
						attr->attrInt = DECIMALTOOLONG;
					}
					else if (leadingZeroError) {
						numToken->tokenName = LEXERR;
						attr->attrInt = LEADINGZEROES;
					}
					else if(yyTrailingZero) {
						numToken->tokenName = LEXERR;
						attr->attrInt = TRAILINGZERO;
					}
					else {
						numToken->tokenName = NUM;
						strcpy(num,d);
						strcat(num,".");
						strcat(num,yy);
						strcat(num,"E");
						strcat(num,zz);
						attr->attrString = num;
					}
					numToken->attribute = attr;
					fsa.currToken = numToken;
					fsa.state = 101;
					fsa.f--;
					return;
				}
				break;
			case 13:
				if(isdigit(c)) {
					realLengthError = 1;
				}
				else {
					struct token *numToken = (struct token *)malloc(sizeof(struct token));
					union Attribute *attr = (union Attribute *)malloc(sizeof(union Attribute));
					if(intLengthError) {
						numToken->tokenName = LEXERR;
						attr->attrInt = INTTOOLONG;
					}
					else if(decimalLengthError) {
						numToken->tokenName = LEXERR;
						attr->attrInt = DECIMALTOOLONG;
					}
					else if (leadingZeroError) {
						numToken->tokenName = LEXERR;
						attr->attrInt = LEADINGZEROES;
					}
					else if(realLengthError) {
						numToken->tokenName = LEXERR;
						attr->attrInt = REALTOOLONG;
					}
					else if(zzLeadingZeroError) {
						numToken->tokenName = LEXERR;
						attr->attrInt = LEADINGZEROESEXP;
					}
					else if(yyTrailingZero) {
						numToken->tokenName = LEXERR;
						attr->attrInt = TRAILINGZERO;
					}
					else {
						numToken->tokenName = NUM;
						strcpy(num,d);
						strcat(num,".");
						strcat(num,yy);
						strcat(num,"E");
						strcat(num,zz);
						attr->attrString = num;
					}
					numToken->attribute = attr;
					fsa.currToken = numToken;
					fsa.state = 101;
					fsa.f--;
					return;
				}
				break;
			case 14:
				if(isdigit(c)) {
					zz[zzCount] = c;
					zzCount++;
					if(c == '0')
						zzLeadingZero = 1;
					fsa.state = 12;
				}
				else {
					struct token *errToken = (struct token *)malloc(sizeof(struct token));
					errToken->tokenName = LEXERR;
					union Attribute *attr = (union Attribute *)malloc(sizeof(union Attribute));
					attr->attrInt = UNRECOGNIZEDNUM;
					errToken->attribute = attr;
					fsa.currToken = errToken;
					fsa.state = 101;
					fsa.f--;
					return;
				}
				break;
		}
	}
}

void newLineMachine(char* line) {
	char c = line[fsa.f];
	// printf("newLine got '%c'\n",c);
	fsa.f++;
	if(c == '\n') {
		fsa.state = 0;
		fsa.f = 0;	
		fsa.b = 0;
	}
	else {
		fsa.f--;
		fsa.state = 16;
	}
}

void addopMachine(char* line) {
	char c = line[fsa.f];
	fsa.f++;
	if(c == '+') {
		struct token *addopToken = (struct token *)malloc(sizeof(struct token));
		addopToken->tokenName = ADDOP;
		union Attribute *attr = (union Attribute *)malloc(sizeof(union Attribute));
		attr->attrInt = ADD;
		addopToken->attribute = attr;
		fsa.currToken = addopToken;
		fsa.state = 101;
		return;
	}
	else if(c == '-') {
		struct token *addopToken = (struct token *)malloc(sizeof(struct token));
		addopToken->tokenName = ADDOP;
		union Attribute *attr = (union Attribute *)malloc(sizeof(union Attribute));
		attr->attrInt = SUBTRACT;
		addopToken->attribute = attr;
		fsa.currToken = addopToken;
		fsa.state = 101;
		return;
	}
	//'or' is also an addop but it is a reserved word also
	//so it would have already been found
	fsa.f--;
	fsa.state = 17;
}

void assignopMachine(char* line) {
	int count = 0;

	while(fsa.f < MAX_LINE_LENGTH && count < 2) {
		count++;
		char c = line[fsa.f];
		// printf("assignop got '%c'\n",c);
		fsa.f++;
		if(count == 1 && c != ':') {
			//first char has to be ':' if not, exit
			fsa.f--;
			fsa.state = 19;
			return; 
		}
		if(count == 2 && c != '=') {
			//second char has to be '=' if not, exit
			fsa.f--;
			fsa.state = 20;
			return;
		}
		else if(count == 2 && c == '=') {
			//if second char is '=', return token
			fsa.state = 101;
			struct token *assignopToken = (struct token *) malloc(sizeof(struct token));
			assignopToken->tokenName = ASSIGNOP;
			fsa.currToken = assignopToken;
			return;
		}
	}
}

void mulopMachine(char *line) {
	char c = line[fsa.f];
	// printf("mulop got '%c'\n",c);
	fsa.f++;
	if(c == '*') {
		struct token *mulopToken = (struct token *)malloc(sizeof(struct token));
		mulopToken->tokenName = MULOP;
		union Attribute *attr = (union Attribute *)malloc(sizeof(union Attribute));
		attr->attrInt = MULTIPLY;
		mulopToken->attribute = attr;
		fsa.currToken = mulopToken;
		fsa.state = 101;
		return;
	}
	else if(c == '/') {
		struct token *mulopToken = (struct token *)malloc(sizeof(struct token));
		mulopToken->tokenName = MULOP;
		union Attribute *attr = (union Attribute *)malloc(sizeof(union Attribute));
		attr->attrInt = DIVIDE;
		mulopToken->attribute = attr;
		fsa.currToken = mulopToken;
		fsa.state = 101;
		return;
	}
	//'div', 'mod', and 'and' are also mulop but are reserved words also
	//so they would have already been found
	fsa.f--;
	fsa.state = 18;
}

void intMachine(char *line) {
	int leadingZero = 0;
	int leadingZeroError = 0;
	int length = 0;
	int maxLength = 10;
	char *num = malloc(sizeof(char) * maxLength);
	while(fsa.f < MAX_LINE_LENGTH) {
		char d = line[fsa.f];
		fsa.f++;
		if(fsa.state == 20) {
			if(isdigit(d)) {
				num[length] = d;
				length++;
				fsa.state = 21;
				if(d == '0') 
					leadingZero = 1;
			}
			else {
				fsa.state = 22;
				fsa.f--;
				return;
			}
		}
		else if(fsa.state == 21) {
			if(isdigit(d)) {
				length++;
				if(length <= maxLength) {
					num[length - 1] = d;
					//leading zero followed by anything else is an error
					if(leadingZero) 
						leadingZeroError = 1;
				}
			}
			else {
				fsa.state = 101;
				fsa.f--;
				struct token *intToken = (struct token *)malloc(sizeof(struct token));
				union Attribute *attr = (union Attribute *)malloc(sizeof(union Attribute));
				if(length > maxLength) {
					intToken->tokenName = LEXERR;
					attr->attrInt = INTTOOLONG;
				}
				else if(leadingZeroError) {
					intToken->tokenName = LEXERR;
					attr->attrInt = LEADINGZEROES;
				}
				else {
					intToken->tokenName = NUM;
					attr->attrString = num;
				}
				intToken->attribute = attr;
				fsa.currToken = intToken;
				return;
			}
		}
	}
} 

void parenMachine(char *line) {
	char c = line[fsa.f];
	fsa.f++;

}

void catchAllMachine(char* line) {
	char c = line[fsa.f];
	fsa.f++;
	//check for all single character tokens
	if(c == '(') {
		fsa.state = 101;
		struct token *parenToken = (struct token *) malloc(sizeof(struct token));
		parenToken->tokenName = OPENPAREN;
		fsa.currToken = parenToken;
		return;
	}
	else if(c == ')') {
		fsa.state = 101;
		struct token *parenToken = (struct token *) malloc(sizeof(struct token));
		parenToken->tokenName = CLOSEPAREN;
		fsa.currToken = parenToken;
		return;
	}
	else if(c == ',') {
		fsa.state = 101;
		struct token *commaToken = (struct token *) malloc(sizeof(struct token));
		commaToken->tokenName = COMMA;
		fsa.currToken = commaToken;
		return;
	}
	else if(c == ';') {
		fsa.state = 101;
		struct token *semicolonToken = (struct token *) malloc(sizeof(struct token));
		semicolonToken->tokenName = SEMICOLON;
		fsa.currToken = semicolonToken;
		return;
	}
	else if(c == ':') {
		fsa.state = 101;
		struct token *colonToken =  (struct token *) malloc(sizeof(struct token));
		colonToken->tokenName = COLON;
		fsa.currToken = colonToken;
		return;
	}
	else if (c == '.') {
		fsa.state = 101;
		struct token *periodToken =(struct token *) malloc(sizeof(struct token));
		periodToken->tokenName = PERIOD;
		fsa.currToken = periodToken;
		return;
	}
	//not a single character token? don't know what to do
	//ignore this char and go back to initial state
	fsa.state = 101;
	struct token *catchallToken = (struct token *)malloc(sizeof(struct token));
	union Attribute *catchallAttr = (union Attribute *)malloc(sizeof(union Attribute));
	catchallToken->tokenName = LEXERR;
	catchallAttr->attrInt = UNRECOGNIZEDSYMBOL;
	catchallToken->attribute = catchallAttr;
	fsa.currToken = catchallToken;
}



void addToReservedWords(char *id, int tokenName, int attr) {
	if(resWordsRoot == NULL) {
		resWordsRoot = (struct resWordNode *) malloc(sizeof(struct resWordNode));
		// printf("id '%s' added to root of symbol table; %d,%d\n",id,tokenName,attr);
		resWordsRoot->next = 0;
		char *idCopy = malloc(sizeof(char) * 32);
		strcpy(idCopy, id);
		resWordsRoot->id = idCopy;
		resWordsRoot->tokenName = tokenName;
		resWordsRoot->attr = attr;
	}
	else {
		struct resWordNode *currNode = resWordsRoot;
		if(!strcmp(id,resWordsRoot->id)) {
			// printf("id '%s' already exists in root of symbol table as '%s'\n",id,resWordsRoot->id);
			return;
		}
		while(currNode->next != 0) {
			currNode = currNode->next;
			if(!strcmp(id,currNode->id)) {
				// printf("id '%s' already exists in symbol table\n",id);
				return; //found id in table, don't add
			}
		}
		//not found, add a new node to list
		struct resWordNode *newNode = (struct resWordNode *) malloc(sizeof(struct resWordNode));
		char *idCopy = malloc(sizeof(char) * 32);
		strcpy(idCopy, id);
		newNode->id = idCopy;
		newNode->tokenName = tokenName;
		newNode->attr = attr;
		newNode->next = 0;
		// printf("adding %s to symbol table; %d,%d\n",id,tokenName,attr);
		currNode->next = newNode;
	}
}

int checkInResWordTable(char *id) {
	struct resWordNode *currNode = resWordsRoot;
	if(!strcmp(id,resWordsRoot->id)) {
		// printf("id '%s' already exists in reserved word table as '%s'\n",id,resWordsRoot->id);
		return 1;
	}
	while(currNode->next != 0) {
		currNode = currNode->next;
		if(!strcmp(id,currNode->id)) {
			// printf("id '%s' already exists in reserved word table\n",id);
			return 1; //found id in table, don't add
		}
	}
	return 0; //not found
}

struct resWordNode *findInResWordTable(char *id) {
	struct resWordNode *currNode = resWordsRoot;
	if(!strcmp(id,resWordsRoot->id)) {
		return resWordsRoot;
	}
	while(currNode->next != 0) {
		currNode = currNode->next;
		if(!strcmp(id,currNode->id)) {

			return currNode; //found id in table, don't add
		}
	}
	//nothing found
	struct resWordNode *emptyNode = (struct resWordNode *) malloc(sizeof(struct resWordNode));
	emptyNode->empty = 1;
	return emptyNode; //not found
}

void addToSymbolTable(char* id) {

	if(symbolTableRoot == NULL) {
		symbolTableRoot = (struct node *) malloc(sizeof(struct node));
		// printf("id '%s' added to symbolTableRoot of symbol table\n",id);
		symbolTableRoot->next = 0;
		symbolTableRoot->id = id;
	}
	else {
		struct node *currNode = symbolTableRoot;
		if(!strcmp(id,symbolTableRoot->id)) {
			// printf("id '%s' already exists in symbolTableRoot of symbol table as '%s'\n",id,symbolTableRoot->id);
			return;
		}
		while(currNode->next != 0) {
			currNode = currNode->next;
			if(!strcmp(id,currNode->id)) {
				// printf("id '%s' already exists in symbol table\n",id);
				return; //found id in table, don't add
			}
		}
		//not found, add a new node to list
		struct node *newNode = (struct node *) malloc(sizeof(struct node));
		newNode->id = id;
		newNode->next = 0;
		// printf("adding %s to symbol table\n",id);
		currNode->next = newNode;
	}
}

char *tokenNameToString(int tokenName) {
	switch(tokenName) 
	{
		case RESERVED:
			return "Reserved";
		case ID:
			return "Id";
		case NUM:
			return "num";
		case RELOP:
			return "relop";
		case ADDOP:
			return "addop";
		case MULOP:
			return "mulop";
		case ASSIGNOP:
			return "assignop";
		case LEXERR:
			return "lexerr";
		case OPENPAREN:
			return "open paren";
		case CLOSEPAREN:
			return "close paren";
		case COLON:
			return "colon";
		case SEMICOLON:
			return "semicolon";
		case PERIOD:
			return "period";
		case COMMA:
			return "comma";
		default:
			return "unknown";
	}
}

char *attributeToString(int attribute) {
	switch(attribute) 
	{
		case INTTOOLONG:
			return "Extra Long Int";
		case UNRECOGNIZEDNUM:
			return "Unrec number";
		case DECIMALTOOLONG:
			return "Extra Long Dec";
		case REALTOOLONG:
			return "Extra Long Re";
		case LEADINGZEROES:
			return "Leading Zero";
		case LEADINGZEROESEXP:
			return "Leading Zero in Exp";
		case TRAILINGZERO:
			return "Trailing zero";
		case LESSTHAN:
			return "<";
		case LESSTHANEQUAL:
			return "<=";
		case NOTEQUAL:
			return "<>";
		case GREATERTHAN:
			return ">";
		case GREATERTHANEQUAL:
			return ">=";
		case EQUAL:
			return "=";
		case ADD:
			return "+";
		case SUBTRACT:
			return "-";
		case OR:
			return "OR";
		case MULTIPLY:
			return "*";
		case DIVIDE:
			return "/";
		case DIV:
			return "DIV";
		case MOD:
			return "MOD";
		case AND:
			return "AND";
		case PROGRAM:
			return "program";
		case VAR:
			return "var";
		case INTEGER:
			return "integer";
		case FUNCTION:
			return "function";
		case BEGIN:
			return "begin";
		case END:
			return "end";
		case THEN:
			return "then";
		case IF:
			return "if";
		case ELSE: 
			return "else";
		case UNRECOGNIZEDSYMBOL:
			return "Unrecog Symbol";
		case IDTOOLONG:
			return "Extra long id";
		default:
			return "unknown";
	}

}

void printToken(char *lexeme) {
		
	if(fsa.currToken->tokenName == LEXERR) {
		//printf( "LEXERR:\t%s:\t'%s'\n", attributeToString(fsa.currToken->attribute->attrInt), lexeme);
		fprintf(listingFile, "LEXERR:\t%s:\t'%s'\n", attributeToString(fsa.currToken->attribute->attrInt), lexeme);		
	}
	switch(fsa.currToken->tokenName) {
		case RESERVED: //int attrs
		case RELOP:	
		case ADDOP:
		case MULOP:
		case LEXERR:
			fprintf(tokenFile,"%d:\t\t%d %-10s\t%-10d\t%-15s\t%s\n",(tokenizingLine+1), fsa.currToken->tokenName, tokenNameToString(fsa.currToken->tokenName), fsa.currToken->attribute->attrInt, attributeToString(fsa.currToken->attribute->attrInt),lexeme);
			break;
		case OPENPAREN: //no attrs
		case CLOSEPAREN:
		case COLON:
		case SEMICOLON:
		case PERIOD:
		case COMMA:
		case ASSIGNOP:
			fprintf(tokenFile,"%d:\t\t%d %-10s\t%-25s\t%s\n",(tokenizingLine+1), fsa.currToken->tokenName, tokenNameToString(fsa.currToken->tokenName),"null", lexeme);
			break;
		case ID:
		case NUM:
			fprintf(tokenFile,"%d:\t\t%d %-10s\t%-10u\t%-15s\t%s\n",(tokenizingLine+1), fsa.currToken->tokenName, tokenNameToString(fsa.currToken->tokenName), (unsigned)fsa.currToken->attribute->attrString, "ptr to sym tab", lexeme);
			break;
		default:
			puts("Token not recognized");
	}
}

void addToken() {
	if(tokenNodeRoot == NULL) {
		tokenNodeRoot = (struct tokenNode *)(malloc(sizeof(struct tokenNode)));
		tokenNodeRoot->thisToken = fsa.currToken;
	}
	else {
		struct tokenNode *currNode = tokenNodeRoot;
		while(currNode->next != 0) {
			currNode = currNode->next;
		}
		struct tokenNode *newNode = (struct tokenNode *)(malloc(sizeof(struct tokenNode )));
		newNode->thisToken = fsa.currToken;
		currNode->next = newNode;
	}
}
