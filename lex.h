#define MAX_LINE_LENGTH 72

//attr ints
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
#define ARRAY 34
#define OF 35
//types
#define ID 40
#define NUM 41
#define RELOP 42
#define ADDOP 43
#define MULOP 44
#define ASSIGNOP 45
#define OPENPAREN 46
#define CLOSEPAREN 47
#define COMMA 48
#define SEMICOLON 49
#define COLON 50
#define PERIOD 51
#define ARRAYRANGE 52
#define ENDOFFILE 53
#define OPENBRACKET 54
#define CLOSEBRACKET 55
#define ENDSTATE 100
#define RESERVED 0
#define LEXERR 99

extern void loadFiles(char* src);
extern struct token *getNextToken();
extern char *tokenNameToString(int);
extern char *attributeToString(int);
#ifndef LEX_H
#define LEX_H

union Attribute {
	char *attrString;
	int attrInt;
};

struct token {
	int tokenName;
	union Attribute *attribute;
};



#endif