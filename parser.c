#include <stdio.h>
#include <stdlib.h>
#include "lex.h"

struct token *tok;
void parse();
void sign();
void match(int type);
void finish();
void program();
void programPrime();
void programPrime2();
void idLst();
void idLstPrime();

int main(int argc, char *argv[]) 
{
	if(argc == 2) 
	{
		loadFiles(argv[1]);
		parse();
	}
	else
	{
		puts("Invalid number of arguments. Valid syntax is 'proj1 /path/to/source'");
		return -1;
	}
	return 0;
}

void parse() {
	tok = getNextToken();
	program();
	match(ENDOFFILE);
	//match end of file
}

void program() {
	if(tok->tokenName == ID) {
		match(PROGRAM);
		match(ID);
		match(OPENPAREN);
		idLst();
		match(CLOSEPAREN);
		match(SEMICOLON);
		programPrime();
	}
	else 
		puts("synerr in program expecting id");
}

void programPrime() {
	switch(tok->attribute->attrInt) {
		case VAR:
			decls();
			programPrime2();
			break;
		case BEGIN:
			cpdStmt();
			match(PERIOD);
			break;
		case FUNCTION:
			subPrgDecls();
			cpdStmt();
			match(PERIOD);
			break;
		default:
			puts("synerr in program' expecting var, begin, or fun");
	}
}

void programPrime2() {
		switch(tok->attribute->attrInt) {
		case BEGIN:
			cpdStmt();
			match(PERIOD);
			break;
		case FUNCTION:
			subPrgDecls();
			cpdStmt();
			match(PERIOD);
			break;
		default:
			puts("synerr in program' expecting var, begin, or fun");
	}
}
void idLst() {
	if(tok->tokenName == ID) {
		match(ID);
		idLstPrime();
	}
	else 
		puts("synerr in idLst expecting id");
}

void idLstPrime() {
	switch(tok->tokenName) {
		case ID:
			match(COMMA);
			match(ID);
			idLstPrime();
			break;
		case SEMICOLON:
		case CLOSEPAREN:
			break; //epsilon
		default:
			puts("synerr in idlst' expecting id or semicolon");
	}

}

void decls() {
	switch(tok->attribute->attrInt) {
		case VAR:
			idLst();
			match(COLON);
			type();
			match(SEMICOLON);
			declsPrime();
			break;
		default:
			puts("synerr in decls expecting var");
	}
}

void declsPrime() {
	switch(tok->attribute->attrInt) {
		case VAR:
			idLst();
			match(COLON);
			type();
			match(SEMICOLON);
			declsPrime();
			break;
		case BEGIN:
		case FUNCTION:
			break; //epsilon
		default:
			puts("synerr in program' expecting var, begin, or fun");
	}
}

void type() {
	if(tok->tokenName == INT || tok->tokenName == REAL) {
		stdType();
	}
	else if(tok->attrInt == ARRAY) {
		match(ARRAY);
		match(OPENBRACKET);
		match(NUM);
		match(ARRAYRANGE);
		match(NUM);
		match(CLOSEBRACKET);
		match(OF);
		stdType();z
	}
} 
void sign() {
	switch(tok->attribute->attrInt) {
		case ADD:
			match(ADD);
			puts("add");
			break;
		case SUBTRACT:
			match(SUBTRACT);
			puts("SUBTRACT");
			break;
		default:
			//error
			break;
	}
}

void match(int type) {
	if(type == ID || type == NUM || type == ASSIGNOP || type == OPENPAREN || type == CLOSEPAREN ||
		type == COMMA || type == SEMICOLON || type == COLON || type == PERIOD || type == ARRAYRANGE || type == ENDOFFILE) {
		if(type == tok->tokenName && type != ENDOFFILE) {
			printf("matched type %s\n", tokenNameToString(type));
			tok = getNextToken();
		}
		else if (type == tok->tokenName && type == ENDOFFILE) 
			finish();
		else if(type != tok->tokenName)
			printf("synerr tok type is %d and type to match is %d, type name is %s \n", tok->tokenName, type, tokenNameToString(type));
	}
	else {
		if(type == tok->attribute->attrInt && type != ENDOFFILE) {
			printf("matched type %s\n", attributeToString(type));
			tok = getNextToken();
		}
		else if (type == tok->attribute->attrInt && type == ENDOFFILE) 
			finish();
		else if(type != tok->attribute->attrInt)
			printf("synerr tok type is %d and type to match is %d, attr name is %s\n", tok->attribute->attrInt, type, attributeToString(type));
	}
}

void finish() {
	puts("program finished");
}
