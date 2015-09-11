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
} *resWordsRoot, *symbolTableRoot;



extern int errno;


void sourceToListing(char *, char *);
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
void addToReservedWords(char *);
int checkInResWordTable(char *);
void addopMachine(char *);
void mulopMachine(char *);
void assignopMachine(char *);
void intMachine(char *);
char *tokenNameToString(int);
char *attributeToString(int);

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
				printf("%d:\t%s",(i+1), lines[i]);
				while(getNextToken(lines[i]) == 0) {

				}
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
			if( (c == ',' || c == '\n') && index > 0) {
				addToReservedWords(resWord);
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
	
//Return 1 at end of line
//Return 0 if token or error
int getNextToken(char *line) {
	fsa.state = 0;
	while(fsa.f < MAX_LINE_LENGTH) {
		switch(fsa.state) 
		{
			case 0:
				// puts("whitespaceMachine");
				whitespaceMachine(line);
				break;
			case 2:
				// puts("idMachine");
				idMachine(line);
				break;
			case 4:
				// puts("relopMachine");
				relopMachine(line);
				break;
			case 7:
				// puts("realMachine");
				realMachine(line);
				break;
			case 15:
				// puts("newLineMachine");
				newLineMachine(line);
				if(fsa.state == 0)
					return 1;
				break;
			case 16:
				// puts("addop machine");
				addopMachine(line);
				break;
			case 17:
				// puts("mulopMachine");
				mulopMachine(line);
				break;
			case 18:
				// puts("assignop machine");
				assignopMachine(line);
				break;
			case 20:
				// puts("int machine");
				intMachine(line);
				break;
			case 101:
				//seg fault if try to read attrString and there wasn't one
				//only read applicable type for token
				//theres probably a better way than >= 32
				fsa.b = fsa.f;
				if(fsa.currToken->tokenName >= 32)
					printf("got token name: '%s' attribute:  '%s'\n",tokenNameToString(fsa.currToken->tokenName),attributeToString(fsa.currToken->attribute->attrInt));
				else 
					printf("got token name: '%s' attribute:  '%s'\n",tokenNameToString(fsa.currToken->tokenName),fsa.currToken->attribute->attrString);
				return 0;
			default:
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
	while(fsa.f < MAX_LINE_LENGTH && idLength < 10) {
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
					id[idLength] = c;
					idLength++;
				}
				else {
					fsa.f--;
					fsa.state = 101;
					printf("got an id %s\n", id);
					if(checkInResWordTable(id)) {
						printf("id is a reserved word '%s'\n", id);
						struct token idToken;
						union Attribute attr;
						attr.attrString = id;
						idToken.tokenName = RESERVED;
						idToken.attribute = &attr;
						fsa.currToken = &idToken;
						return;
					}
					addToSymbolTable(id);
					struct token idToken;
					union Attribute attr;
					attr.attrString = id;
					idToken.tokenName = ID;
					idToken.attribute = &attr;
					fsa.currToken = &idToken;
					return;
				}
				break;
		}	
	}
}

void relopMachine(char* line) {
	struct token relopToken;
	union Attribute attr;
	while(fsa.f < MAX_LINE_LENGTH) {
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
					relopToken.tokenName = RELOP;
					attr.attrInt = EQUAL;
					relopToken.attribute = &attr;
					fsa.currToken = &relopToken;
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
				relopToken.tokenName = RELOP;
				if(relop == '=') {
					attr.attrInt = LESSTHANEQUAL;
				}
				else if(relop == '>') {
					attr.attrInt = NOTEQUAL;
				}
				else {
					fsa.f--; //'<' + another, non relop, character, so just <
					attr.attrInt = LESSTHAN;
				}
				relopToken.attribute = &attr;
				fsa.currToken = &relopToken;
				fsa.state = 101;
				return;
			case 6:
				relopToken.tokenName = RELOP;
				if(relop == '=') 
					attr.attrInt = GREATERTHANEQUAL;
				if(relop != '=') {
					attr.attrInt = GREATERTHAN;
					fsa.f--;//'>'+ another, non relop, character, so just >
				}
				relopToken.attribute = &attr;
				fsa.currToken = &relopToken;
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
					struct token errToken;
					errToken.tokenName = LEXERR;
					union Attribute attr;
					attr.attrInt = UNRECOGNIZEDNUM;
					errToken.attribute = &attr;
					fsa.currToken = &errToken;
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
					struct token numToken;
					union Attribute attr;
					if(intLengthError) {
						
						numToken.tokenName = LEXERR;
						attr.attrInt = INTTOOLONG;
	
					}
					else if(decimalLengthError) {
						numToken.tokenName = LEXERR;
						attr.attrInt = DECIMALTOOLONG;
					}
					else if (leadingZeroError) {
						numToken.tokenName = LEXERR;
						attr.attrInt = LEADINGZEROES;
					}
					else if(yyTrailingZero) {
						numToken.tokenName = LEXERR;
						attr.attrInt = TRAILINGZERO;
					}
					else {
						numToken.tokenName = NUM;
						strcpy(num,d);
						strcat(num,".");
						strcat(num,yy);
						attr.attrString = num;
				
					}
					numToken.attribute = &attr;
					fsa.currToken = &numToken;
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
					struct token errToken;
					errToken.tokenName = LEXERR;
					union Attribute attr;
					attr.attrInt = UNRECOGNIZEDNUM;
					errToken.attribute = &attr;
					fsa.currToken = &errToken;
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
					struct token numToken;
					union Attribute attr;
					if(intLengthError) {
						numToken.tokenName = LEXERR;
						attr.attrInt = INTTOOLONG;
					}
					else if(decimalLengthError) {
						numToken.tokenName = LEXERR;
						attr.attrInt = DECIMALTOOLONG;
					}
					else if (leadingZeroError) {
						numToken.tokenName = LEXERR;
						attr.attrInt = LEADINGZEROES;
					}
					else if(yyTrailingZero) {
						numToken.tokenName = LEXERR;
						attr.attrInt = TRAILINGZERO;
					}
					else {
						numToken.tokenName = NUM;
						strcpy(num,d);
						strcat(num,".");
						strcat(num,yy);
						strcat(num,"E");
						strcat(num,zz);
						attr.attrString = num;
					}
					numToken.attribute = &attr;
					fsa.currToken = &numToken;
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
					struct token numToken;
					union Attribute attr;
					if(intLengthError) {
						numToken.tokenName = LEXERR;
						attr.attrInt = INTTOOLONG;
					}
					else if(decimalLengthError) {
						numToken.tokenName = LEXERR;
						attr.attrInt = DECIMALTOOLONG;
					}
					else if (leadingZeroError) {
						numToken.tokenName = LEXERR;
						attr.attrInt = LEADINGZEROES;
					}
					else if(realLengthError) {
						numToken.tokenName = LEXERR;
						attr.attrInt = REALTOOLONG;
					}
					else if(zzLeadingZeroError) {
						numToken.tokenName = LEXERR;
						attr.attrInt = LEADINGZEROESEXP;
					}
					else if(yyTrailingZero) {
						numToken.tokenName = LEXERR;
						attr.attrInt = TRAILINGZERO;
					}
					else {
						numToken.tokenName = NUM;
						strcpy(num,d);
						strcat(num,".");
						strcat(num,yy);
						strcat(num,"E");
						strcat(num,zz);
						attr.attrString = num;
					}
					numToken.attribute = &attr;
					fsa.currToken = &numToken;
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
					struct token errToken;
					errToken.tokenName = LEXERR;
					union Attribute attr;
					attr.attrInt = UNRECOGNIZEDNUM;
					errToken.attribute = &attr;
					fsa.currToken = &errToken;
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
	printf("addop got '%c'\n",c);
	fsa.f++;
	if(c == '+') {
		struct token addopToken;
		addopToken.tokenName = ADDOP;
		union Attribute attr;
		attr.attrInt = ADD;
		addopToken.attribute = &attr;
		fsa.currToken = &addopToken;
		fsa.state = 101;
		return;
	}
	else if(c == '-') {
		struct token addopToken;
		addopToken.tokenName = ADDOP;
		union Attribute attr;
		attr.attrInt = SUBTRACT;
		addopToken.attribute = &attr;
		fsa.currToken = &addopToken;
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
			fsa.state = 19;
			return;
		}
		else if(count == 2 && c == '=') {
			//if second char is '=', return token
			fsa.state = 101;
			struct token assignopToken;
			assignopToken.tokenName = ASSIGNOP;
			fsa.currToken = &assignopToken;
			return;
		}
	}
}

void mulopMachine(char *line) {
	char c = line[fsa.f];
	// printf("mulop got '%c'\n",c);
	fsa.f++;
	if(c == '*') {
		struct token mulopToken;
		mulopToken.tokenName = MULOP;
		union Attribute attr;
		attr.attrInt = MULTIPLY;
		mulopToken.attribute = &attr;
		fsa.currToken = &mulopToken;
		fsa.state = 101;
		return;
	}
	else if(c == '/') {
		struct token mulopToken;
		mulopToken.tokenName = MULOP;
		union Attribute attr;
		attr.attrInt = DIVIDE;
		mulopToken.attribute = &attr;
		fsa.currToken = &mulopToken;
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
				puts("int machine fail");
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
				struct token intToken;
				union Attribute attr;
				if(length > maxLength) {
					intToken.tokenName = LEXERR;
					attr.attrInt = INTTOOLONG;
				}
				else if(leadingZeroError) {
					intToken.tokenName = LEXERR;
					attr.attrInt = LEADINGZEROES;
				}
				else {
					intToken.tokenName = NUM;
					attr.attrString = num;
				}
				intToken.attribute = &attr;
				fsa.currToken = &intToken;
				return;
			}
		}
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
		printf("id '%s' added to root of symbol table\n",id);
		resWordsRoot->next = 0;
		resWordsRoot->id = id;
	}
	else {
		struct node *currNode = resWordsRoot;
		if(!strcmp(id,resWordsRoot->id)) {
			printf("id '%s' already exists in root of symbol table as '%s'\n",id,resWordsRoot->id);
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

int checkInResWordTable(char *id) {
	struct node *currNode = resWordsRoot;
	if(!strcmp(id,resWordsRoot->id)) {
		printf("id '%s' already exists in reserved word table as '%s'\n",id,resWordsRoot->id);
		return 1;
	}
	while(currNode->next != 0) {
		currNode = currNode->next;
		if(!strcmp(id,currNode->id)) {
			printf("id '%s' already exists in reserved word table\n",id);
			return 1; //found id in table, don't add
		}
	}
	return 0; //not found
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
			return "Extra Long Decimal";
		case REALTOOLONG:
			return "Extra Long Real";
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
			return "or";
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
		default:
			return "unknown";
	}
}
