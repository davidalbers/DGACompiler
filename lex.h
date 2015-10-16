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
#define ARRAYRANGE 42
#define ENDOFFILE 43
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
#define ENDSTATE 100

extern void loadFiles(char* src);
extern struct token *getNextToken();

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