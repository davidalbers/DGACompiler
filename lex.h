#define MAX_LINE_LENGTH 72

//attr ints
#define NONE 0
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
#define WHILE 36
#define NOT 37
#define DO 38
#define REAL 40
//types
#define ID 50
#define NUM 51
#define RELOP 52
#define ADDOP 53
#define MULOP 54
#define ASSIGNOP 55
#define OPENPAREN 56
#define CLOSEPAREN 57
#define COMMA 58
#define SEMICOLON 59
#define COLON 60
#define PERIOD 61
#define ARRAYRANGE 62
#define ENDOFFILE 63
#define OPENBRACKET 64
#define CLOSEBRACKET 65
#define ENDSTATE 100
#define RESERVED 0
#define LEXERR 99
//nonterminals
#define NT_PROGRAM 0
#define NT_IDLST 1
#define NT_DECLS 2
#define NT_TYPE 3
#define NT_STDTYPE 4
#define NT_SUBPRGDECLS 5 
#define NT_SUBPRGDECL 6
#define NT_SUBPRGHEAD 7
#define NT_PARAMLST 8
#define NT_CPDSTMT 9
#define NT_ARGS 10
#define NT_OPTSTMTS 12
#define NT_STMTLST 13
#define NT_STMT 14
#define NT_VAR 15
#define NT_EXPLST 16
#define NT_EXPR 17
#define NT_SIMEXP 18
#define NT_TERM 19
#define NT_FACTOR 20
#define NT_SIGN 21
//types
#define INT_TYPE = 0
#define BOOL_TYPE = 1
#define ARRAY_INT_TYPE = 2
#define ARRAY_BOOL_TYPE = 3
//a variable was made in variable production,will be constrained later
#define ANY_TYPE = 4
//in variable production, an array was made, will be constrained later
#define ANY_ARRAY_TYPE = 5 

extern void loadFiles(char* src);
extern struct token *getNextToken(FILE * listingFile);
extern char *tokenNameToString(int);
extern char *attributeToString(int);
extern struct node *getSymbol(char* id);

#ifndef LEX_H
#define LEX_H

union Attribute {
	char *attrString;
	int attrInt;
};

struct token {
	int tokenName;
	int subName;
	union Attribute *attribute;
};


struct typeReturn {
	int encounteredError;
	int val = -1;
};

struct varReturn {
	int encounteredError;
	int val = -1;
	char *lexeme;
};

struct node {
	char *id;
	int type;
	struct node *next;
} *symbolTableRoot;

#endif
