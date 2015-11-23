#include <stdio.h>
#include <stdlib.h>
#include "lex.h"

struct token *tok;
void parse();
void sign();
int match(int type);
void finish();
void program();
void programPrime();
void programPrime2();
void idLst();
void idLstPrime();
void decls();
void declsPrime();
void type();
void stdType();
void subPrgDecls();
void subPrgDeclsPrime();
void subPrgDecl();
void subPrgDeclPrime();
void subPrgDeclPrime2();
void subPrgHead();
void subPrgHeadPrime();
void args();
void paramLst();
void paramLstPrime();
void cpdStmt();
void cpdStmtPrime();
void optStmts();
void stmtLst();
void stmtLstPrime();
void stmt();
void stmtPrime();
void var();
void varPrime();
void expLst();
void expLstPrime();
void expr();
void exprPrime();
void simExp();
void simExpPrime();
void term();
void termPrime();
void factor();
void factorPrime();
void sign();
int synch(struct token * tok, int syncingType);
void synchType(int syncingType);
void printSynerr(char * neededTypes, char * nonterminal);
void printMatchSynerr(char * matchType);

FILE * listingFile;
extern int errno;

int main(int argc, char *argv[]) 
{
	if(argc == 2) 
	{
		listingFile = fopen("listing.txt", "w");
		if(listingFile != NULL) {
			loadFiles(argv[1]);
			parse();
		}
		else
			perror("Could not open listing file!");
	}
	else
	{
		puts("Invalid number of arguments. Valid syntax is 'proj1 /path/to/source'");
		return -1;
	}
	return 0;
}

void parse() {
	tok = getNextToken(listingFile);
	program();
	match(ENDOFFILE);
	//match end of file
}

void program() {
	if(tok->attribute->attrInt == PROGRAM){
		if(!match(PROGRAM)) {synchType(NT_PROGRAM); return;}
		if(!match(ID)) {synchType(NT_PROGRAM); return;}
		if(!match(OPENPAREN)) {synchType(NT_PROGRAM); return;}
		idLst();
		if(!match(CLOSEPAREN)) {synchType(NT_PROGRAM); return;}
		if(!match(SEMICOLON)) {synchType(NT_PROGRAM); return;}
		programPrime();
	}
	else {
		printSynerr("program", "program");
		synchType(NT_PROGRAM);
	}
}


void programPrime() {
	switch(tok->attribute->attrInt) {
		case VAR:
			decls();
			programPrime2();
			break;
		case BEGIN:
			cpdStmt();
			if(!match(PERIOD)) {synchType(NT_PROGRAM); break;}
			break;
		case FUNCTION:
			subPrgDecls();
			cpdStmt();
			if(!match(PERIOD)) {synchType(NT_PROGRAM); break;}
			break;
		default:
			printSynerr("var, begin, or function", "program'");
			synchType(NT_PROGRAM);
	}
}

void programPrime2() {
		switch(tok->attribute->attrInt) {
		case BEGIN:
			cpdStmt();
			if(!match(PERIOD)) {synchType(NT_PROGRAM); break;}
			break;
		case FUNCTION:
			subPrgDecls();
			cpdStmt();
			if(!match(PERIOD)) {synchType(NT_PROGRAM); break;}
			break;
		default:
			printSynerr("begin of function", "program''");
			synchType(NT_PROGRAM);	
	}
}
void idLst() {
	if(tok->tokenName == ID) {
		if(!match(ID)) {synchType(NT_IDLST); return;}
		idLstPrime();
	}
	else {
		printSynerr("id", "id_list");
		synchType(NT_IDLST);
	}
}

void idLstPrime() {
	switch(tok->tokenName) {
		case COMMA:
			if(!match(COMMA)) {synchType(NT_IDLST); break;}
			if(!match(ID)) {synchType(NT_IDLST); break;}
			idLstPrime();
			break;
		case CLOSEPAREN:
			break; //epsilon
		default:
			printSynerr("comma or close paren", "id_list'");
			synchType(NT_IDLST);
	}

}

void decls() {
	switch(tok->attribute->attrInt) {
		case VAR:
			if(!match(VAR)) {synchType(NT_DECLS); break;}
			if(!match(ID)) {synchType(NT_DECLS); break;}
			if(!match(COLON)) {synchType(NT_DECLS); break;}
			type();
			if(!match(SEMICOLON)) {synchType(NT_DECLS); break;}
			declsPrime();
			break;
		default: 
			printSynerr("var", "declarations");
			synchType(NT_DECLS);
	}
}

void declsPrime() {
	switch(tok->attribute->attrInt) {
		case VAR:
			if(!match(VAR)) {synchType(NT_DECLS); break;}
			if(!match(ID)) {synchType(NT_DECLS); break;}
			if(!match(COLON)) {synchType(NT_DECLS); break;}
			type();
			if(!match(SEMICOLON)) {synchType(NT_DECLS); break;}
			declsPrime();
			break;
		case BEGIN:
		case FUNCTION:
			break; //epsilon
		default:
			printSynerr("var, begin, or function", "declarations'");
			synchType(NT_DECLS);
	}
}

void type() {
	if(tok->attribute->attrInt == INTEGER || tok->attribute->attrInt == REAL) {
		stdType();
	}
	else if(tok->attribute->attrInt == ARRAY) {
		if(!match(ARRAY)) {synchType(NT_TYPE); return;}
		if(!match(OPENBRACKET)) {synchType(NT_TYPE); return;}
		if(!match(NUM)) {synchType(NT_TYPE); return;}
		if(!match(ARRAYRANGE)) {synchType(NT_TYPE); return;}
		if(!match(NUM)) {synchType(NT_TYPE); return;}
		if(!match(CLOSEBRACKET)) {synchType(NT_TYPE); return;}
		if(!match(OF)) {synchType(NT_TYPE); return;}
		stdType();
	}
	else {
		printSynerr("intger, real, or array", "type");
		synchType(NT_TYPE);
	}
} 

void stdType() {
	if(tok->attribute->attrInt == INTEGER) {
		if(!match(INTEGER)) {synchType(NT_STDTYPE); return;}
	}
	else if(tok->attribute->attrInt == REAL) {
		if(!match(REAL)) {synchType(NT_STDTYPE); return;}
	}
	else {
		printSynerr("integer or real", "standard_type");
		synchType(NT_STDTYPE);
	}
}

void subPrgDecls() {
	if(tok->attribute->attrInt == FUNCTION) {
		subPrgDecl();
		if(!match(SEMICOLON)) {synchType(NT_SUBPRGDECLS); return;}
		subPrgDeclsPrime();
	}
	else {
		printSynerr("function", "subprogram_declarations");
		synchType(NT_SUBPRGDECLS);
	}
}

void subPrgDeclsPrime() {
	if(tok->attribute->attrInt == FUNCTION) {
		subPrgDecls();
		if(!match(SEMICOLON)) {synchType(NT_SUBPRGDECLS); return;}
		subPrgDeclsPrime();
	}
	else if(tok->attribute->attrInt == BEGIN)
		return; //epsilon
	else {
		printSynerr("function or begin", "subprogram_declarations'");
		synchType(NT_SUBPRGDECLS);
	}
}

void subPrgDecl() {
	if(tok->attribute->attrInt == FUNCTION) {
		subPrgHead();
		subPrgDeclPrime();
	}
	else {
		printSynerr("function", "subprogram_declaration");
		synchType(NT_SUBPRGDECL);
	}
}

void subPrgDeclPrime() {
	if(tok->attribute->attrInt == VAR) {
		decls();
		subPrgDeclPrime2();
	}
	else if(tok->attribute->attrInt == BEGIN) {
		cpdStmt();
	}
	else if(tok->attribute->attrInt == FUNCTION) {
		subPrgDecls();
		cpdStmt();
	}
	else {
		printSynerr("var, begin, or function", "subprogram_declaration'");
		synchType(NT_SUBPRGDECL);
	}
}

void subPrgDeclPrime2() {
	if(tok->attribute->attrInt == BEGIN) {
		cpdStmt();
	}
	else if(tok->attribute->attrInt == FUNCTION) {
		subPrgDecls();
		cpdStmt();
	}
	else {
		printSynerr("var or function", "subprogram_declaration''");
		synchType(NT_SUBPRGDECL);
	}
}

void subPrgHead() {
	if(tok->attribute->attrInt == FUNCTION) {
		if(!match(FUNCTION)) {synchType(NT_SUBPRGHEAD); return;}
		if(!match(ID)) {synchType(NT_SUBPRGHEAD); return;}
		subPrgHeadPrime();
	}
	else {
		printSynerr("function", "subPrgHead");
		synchType(NT_SUBPRGHEAD);
	}
}

void subPrgHeadPrime() {
	if(tok->tokenName == OPENPAREN) {
		args();
		if(!match(COLON)) {synchType(NT_SUBPRGHEAD); return;}
		stdType();
		if(!match(SEMICOLON)) {synchType(NT_SUBPRGHEAD); return;}
	}
	else if(tok->tokenName == COLON) {
		if(!match(COLON)) {synchType(NT_SUBPRGHEAD); return;}
		stdType();
		if(!match(SEMICOLON)) {synchType(NT_SUBPRGHEAD); return;}
	}
	else {
		printSynerr("open paren or colon", "subPrgHead'");
		synchType(NT_SUBPRGHEAD);
	}
}

void args() {
	if(tok->tokenName == OPENPAREN) {
		if(!match(OPENPAREN)) {synchType(NT_ARGS); return;}
		paramLst();
		if(!match(CLOSEPAREN)) {synchType(NT_ARGS); return;}
	}
	else {
		printSynerr("open paren", "args");
		synchType(NT_ARGS);
	}
}

void paramLst() {
	if(tok->tokenName == ID) {
		if(!match(ID)) {synchType(NT_PARAMLST); return;}
		if(!match(COLON)) {synchType(NT_PARAMLST); return;}
		type();
		paramLstPrime();
	}
	else {
		printSynerr("id", "param_list");
		synchType(NT_PARAMLST);
	}
}

void paramLstPrime() {
	if(tok->tokenName == SEMICOLON) {
		if(!match(SEMICOLON)) {synchType(NT_PARAMLST); return;}
		if(!match(ID)) {synchType(NT_PARAMLST); return;}
		if(!match(COLON)) {synchType(NT_PARAMLST); return;}
		type();
		paramLstPrime();
	}
	else if(tok->tokenName == CLOSEPAREN) 
		return; //epsilon
	else {
		printSynerr("semicolon or close paren", "param_list'");
		synchType(NT_PARAMLST); 
	}
}

void cpdStmt() {
	if(tok->attribute->attrInt == BEGIN) {
		if(!match(BEGIN)) {synchType(NT_CPDSTMT); return;};
		cpdStmtPrime();
	}
	else {
		printSynerr("begin", "compound_statement");
		synchType(NT_CPDSTMT);	
	}
}

void cpdStmtPrime() {
	if(tok->tokenName == ID || tok->attribute->attrInt == BEGIN || tok->attribute->attrInt == IF || tok->attribute->attrInt == WHILE) {
		optStmts();
		if(!match(END)) {synchType(NT_CPDSTMT); return;}
	}
	else if(tok->attribute->attrInt == END) {
		if(!match(END)) {synchType(NT_CPDSTMT); return;}
	}
	else {
		printSynerr("id, begin, if, while, or end", "compound_statement'");
		synchType(NT_CPDSTMT);
	}
}

void optStmts() {
	if(tok->tokenName == ID || tok->attribute->attrInt == BEGIN || tok->attribute->attrInt == IF || tok->attribute->attrInt == WHILE) {
		stmtLst();
	}
	else {
		printSynerr("id, begin, if, while, or end", "optional_statement");
		synchType(NT_OPTSTMTS);
	}
}

void stmtLst() {
	if(tok->tokenName == ID || tok->attribute->attrInt == BEGIN || tok->attribute->attrInt == IF || tok->attribute->attrInt == WHILE) {
		stmt();
		stmtLstPrime();
	}
	else {
		printSynerr("id, begin, if, while, or end", "statement_list");
		synchType(NT_STMTLST);
	}
}

void stmtLstPrime() {
	if(tok->tokenName == SEMICOLON) {
		if(!match(SEMICOLON)) {synchType(NT_STMTLST); return;}
		stmt();
		stmtLstPrime();
	}
	else if (tok->attribute->attrInt == END) 
		return;//epsilon
	else {
		printSynerr("semicolon or end", "statement_list'");
		synchType(NT_STMTLST);
	}
}

void stmt() {
	if(tok->tokenName == ID) {
		var();
		if(!match(ASSIGNOP)) {synchType(NT_STMT); return;}
		expr();
	} 
	else if(tok->attribute->attrInt == BEGIN) {
		cpdStmt();
	} 
	else if( tok->attribute->attrInt == IF) {
		if(!match(IF)) {synchType(NT_STMT); return;}
		expr();
		if(!match(THEN)) {synchType(NT_STMT); return;}
		stmt();
		stmtPrime();
	} 
	else if( tok->attribute->attrInt == WHILE) {
		if(!match(WHILE)) {synchType(NT_STMT); return;}
		expr();
		if(!match(DO)) {synchType(NT_STMT); return;}
		stmt();
	}
	else {
		printSynerr("id, begin, if, or while", "statement");
		synchType(NT_STMT);
	}
}

void stmtPrime() {
	if(tok->attribute->attrInt == END || tok->tokenName == SEMICOLON) 
		return;//epsilon
	else if(tok->attribute->attrInt == ELSE) {
		if(!match(ELSE)) {synchType(NT_STMT); return;}
		stmt();
	}
	else {
		printSynerr("id, begin, if, or while", "statement'");
		synchType(NT_STMT);
	}
}

void var() {
	if(tok->tokenName == ID) {
		if(!match(ID)) {synchType(NT_VAR); return;}
		varPrime();
	}
	else{
		printSynerr("id", "variable");
		synchType(NT_VAR);
	}
}

void varPrime() {
	if(tok->tokenName == ID) 
		return;//epsilon
	else if(tok->tokenName == OPENBRACKET) {
		if(!match(OPENBRACKET)) {synchType(NT_VAR); return;}
		expr();
		if(!match(CLOSEBRACKET)) {synchType(NT_VAR); return;}
	}
	else if(tok->tokenName == ASSIGNOP) {
		return;//epsilon
	}
	else {
		printSynerr("id, open bracket, or assign op", "variable'");
		synchType(NT_VAR);
	}
}


void expLst() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM ||
		tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT || tok->attribute->attrInt == NOT) {
		expr();
		expLstPrime();
	}
	else {
		printSynerr("open paren, id, num, plus, minus, not", "expression_list");
		synchType(NT_EXPLST);
	}
}

void expLstPrime() {
	if(tok->tokenName == COMMA) {
		if(!match(COMMA)) {synchType(NT_EXPLST); return;}
		expr();
		expLstPrime();	
	}
	else if(tok->tokenName == CLOSEPAREN) {
		return; //epsilon
	}
	else {
		printSynerr("comma, close paren", "expression_list");
		synchType(NT_EXPLST);
	}
}

void expr() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM ||
		tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT || tok->attribute->attrInt == NOT) {
		simExp();
		exprPrime();
	}
	else {
		printSynerr("open paren, id, num, plus, minus, not", "expression");
		synchType(NT_EXPR);
	}
}

void exprPrime() { 
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == SEMICOLON || tok->attribute->attrInt == END || tok->tokenName == CLOSEBRACKET ||
	 tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == COMMA || 
	 tok->attribute->attrInt == ELSE ) {
		return;//epsilon
	}
	else if(tok->tokenName == RELOP) {
		if(!match(RELOP)) {synchType(NT_EXPR); return;}
		simExp();
	}
	else {
		printSynerr("relop, close paren, semicolon, end, CLOSEBRACKET, then, do, comma, else", "expression");
		synchType(NT_EXPR);
	}
}

void simExp() {

	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM || tok->attribute->attrInt == NOT) {
		term();
		simExpPrime();
	}
	else if(tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT) {
		sign();
		term();
		simExpPrime();
	}
	else {
		printSynerr("open paren, id, num, plus, minus, not", "simple_expression");
		synchType(NT_SIMEXP);
	}
}

void simExpPrime() {
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == SEMICOLON || tok->tokenName == RELOP || tok->tokenName == CLOSEBRACKET || tok->tokenName == COMMA || tok->attribute->attrInt == END ||
	  tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->attribute->attrInt == ELSE ) {
		return;//epsilon
	}
	else if(tok->tokenName == ADDOP) {
		if(!match(ADDOP)) {synchType(NT_SIMEXP); return;}
		term();
		simExpPrime();
	}
	else {
		printSynerr("addop, open paren, id, num, plus, minus, not", "simple_expression'");
		synchType(NT_SIMEXP);
	}
}

void term() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM || tok->attribute->attrInt == NOT) {
		factor();
		termPrime();
	}
	else {
		printSynerr("open paren, id, num, not", "term");
		synchType(NT_TERM);
	}
}

void termPrime() {
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == RELOP || tok->tokenName == SEMICOLON || tok->tokenName == CLOSEBRACKET || 
			tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == ADDOP ||
			 tok->tokenName == COMMA || tok->attribute->attrInt == ELSE ||tok->attribute->attrInt == END ) {
		return;//epsilon
	}
	else if(tok->tokenName == MULOP) {
		if(!match(MULOP)) {synchType(NT_TERM); return;}
		factor();
		term();
	}
	else {
		printSynerr("mulop, close paren, relop, semicolon, close bracket, then, do, addop, comma, else, end", "term'");
		synchType(NT_TERM);
	}
}

void factor() {
	if(tok->tokenName == OPENPAREN) {
		if(!match(OPENPAREN)) {synchType(NT_FACTOR); return;}
		expr();
		if(!match(CLOSEPAREN)) {synchType(NT_FACTOR); return;}
	}
	else if(tok->tokenName == ID) {
		if(!match(ID)) {synchType(NT_FACTOR); return;}
		factorPrime();
	}
	else if(tok->tokenName == NUM) {
		if(!match(NUM)) {synchType(NT_FACTOR); return;}
	}
	else if(tok->attribute->attrInt == NOT) {
		if(!match(NOT)) {synchType(NT_FACTOR); return;}
		factor();
	}
	else {
		printSynerr("open paren, id, num, not", "factor");
		synchType(NT_FACTOR);
	}
}

void factorPrime() {
	if(tok->tokenName == OPENPAREN) {
		if(!match(OPENPAREN)) {synchType(NT_FACTOR); return;}
		expr();
		if(!match(CLOSEPAREN)) {synchType(NT_FACTOR); return;}
	}
	else if(tok->tokenName == OPENBRACKET) {
		if(!match(OPENBRACKET)) {synchType(NT_FACTOR); return;}
		expr();
		if(!match(CLOSEBRACKET)) {synchType(NT_FACTOR); return;}
	}
	else if(tok->tokenName == CLOSEPAREN || tok->attribute->attrInt == END || tok->tokenName == CLOSEBRACKET ||
		tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == ADDOP ||
		 tok->attribute->attrInt == MULOP || tok->tokenName == COMMA || tok->tokenName == ELSE || tok->tokenName == RELOP) {
		return;//epsilon
	}
	else {
		printSynerr("open paren, open bracket, close paren, end, close bracket, then, do, addop, mulop, comma, else, relop", "factor'");
		synchType(NT_FACTOR);
	}
}

void sign() {
	switch(tok->attribute->attrInt) {
		case ADD:
			if(!match(ADD)) {synchType(NT_SIGN); break;}
		
			break;
		case SUBTRACT:
			if(!match(SUBTRACT)) {synchType(NT_SIGN); break;}
	
			break;
		default:
			printSynerr("plus or minus", "sign");
			synchType(NT_SIGN);
	}
}

int match(int type) {
	if(type == ID || type == NUM || type == ASSIGNOP || type == OPENPAREN || type == CLOSEPAREN ||
		type == COMMA || type == SEMICOLON || type == COLON || type == PERIOD || type == ARRAYRANGE || 
		type == ENDOFFILE || type == RELOP || type == ADDOP || type == MULOP || type == OPENBRACKET || type == CLOSEBRACKET) {
		if(type == tok->tokenName && type != ENDOFFILE) {
			//printf("%s, ", tokenNameToString(type));
			tok = getNextToken(listingFile);
			return 1;
		}
		else if (type == tok->tokenName && type == ENDOFFILE) {
			finish();
			return 1;
		}
		else if(type != tok->tokenName) {
			printMatchSynerr(tokenNameToString(type));
			//fprintf("synerr tok type is %d and type to match is %d, type name is %s \n", tok->tokenName, type, tokenNameToString(type));
			return 0;
		}
	}
	else {
		if(type == tok->attribute->attrInt && type != ENDOFFILE) {
			//printf("%s, ", attributeToString(type));
			tok = getNextToken(listingFile);
			return 1;
		}
		else if (type == tok->attribute->attrInt && type == ENDOFFILE) {
			finish();
			return 1;
		}
		else if(type != tok->attribute->attrInt) {
			printMatchSynerr(attributeToString(type));
			//printf("synerr tok type is %d and type to match is %d, attr name is %s\n", tok->attribute->attrInt, type, attributeToString(type));
			return 0;
		}
	}
	return 0;
}
void synchType(int syncingType) {
	while(!synch(tok,syncingType)) {
		//printf("syncing %d", tok->tokenName);
		tok = getNextToken(listingFile);
	}	
}

int synch(struct token * tok, int syncingType) {
	int tokType = 0;
	int * syncSet;
	int count = 0;
	if(tok->tokenName == ID || tok->tokenName == NUM || tok->tokenName == ASSIGNOP || tok->tokenName == OPENPAREN || tok->tokenName == CLOSEPAREN ||
		tok->tokenName == COMMA || tok->tokenName == SEMICOLON || tok->tokenName == COLON || tok->tokenName == PERIOD || tok->tokenName == ARRAYRANGE || 
		tok->tokenName == ENDOFFILE || tok->tokenName == RELOP || tok->tokenName == ADDOP || tok->tokenName == MULOP || tok->tokenName == OPENBRACKET || tok->tokenName == CLOSEBRACKET) {
		tokType = tok->tokenName;
	}
	else
		tokType = tok->attribute->attrInt;
	switch(syncingType) {
		case NT_PROGRAM:
			syncSet = (int[]){ENDOFFILE};
			count = 1;
			break;
		case NT_IDLST:
			syncSet = (int[]){CLOSEPAREN,ENDOFFILE};
			count = 2;
			break;
		case NT_DECLS:
			syncSet = (int[]){VAR,BEGIN,ENDOFFILE};
			count = 3;
			break;
		case NT_TYPE:
			syncSet = (int[]) {SEMICOLON , CLOSEPAREN, ENDOFFILE };
			count = 3;
			break;
		case NT_STDTYPE:
			syncSet = (int[]) {SEMICOLON , CLOSEPAREN, ENDOFFILE };
			count = 3;
			break;
		case NT_SUBPRGDECLS:
			syncSet = (int[]) {BEGIN, ENDOFFILE};
			count = 2;
			break;
		case NT_SUBPRGDECL:
			syncSet = (int[]){SEMICOLON,ENDOFFILE};
			count = 2;
			break;
		case NT_SUBPRGHEAD:
			syncSet = (int[]){VAR,BEGIN,FUNCTION,ENDOFFILE};
			count = 4;
			break;
		case NT_ARGS:
			syncSet = (int[]){COLON, ENDOFFILE};
			count = 2;
			break;
		case NT_PARAMLST:
			syncSet = (int[]){CLOSEPAREN, ENDOFFILE};
			count = 2;
			break;
		case NT_CPDSTMT:
			syncSet = (int[]){PERIOD , SEMICOLON , END, ENDOFFILE};
			count = 4;
			break;
		case NT_OPTSTMTS:
			syncSet = (int[]){END, ENDOFFILE};
			count = 2;
			break;
		case NT_STMTLST:
			syncSet = (int[]){END, ENDOFFILE};
			count = 2;
			break;
		case NT_STMT:
			syncSet = (int[]){END, ELSE, SEMICOLON, ENDOFFILE};
			count = 4;
			break;
		case NT_VAR:
			syncSet = (int[]){ASSIGNOP, ID, ENDOFFILE};
			count = 3;
			break;
		case NT_EXPLST:
			syncSet = (int[]){CLOSEPAREN,ENDOFFILE};
			count = 2;
			break;
		case NT_EXPR:
			syncSet = (int[]){CLOSEPAREN,CLOSEBRACKET,THEN,DO,END,ELSE,COMMA,SEMICOLON,ENDOFFILE};
			count = 9;
			break;
		case NT_SIMEXP:
			syncSet = (int[]){CLOSEPAREN,CLOSEBRACKET,THEN,DO,END,ELSE,COMMA,RELOP,SEMICOLON,ENDOFFILE};
			count = 10;
			break;
		case NT_TERM:
			syncSet = (int[]){ADDOP, CLOSEPAREN, CLOSEBRACKET, THEN, DO, END, ELSE, COMMA, RELOP, SEMICOLON, ENDOFFILE};
			count = 11;
			break;
		case NT_FACTOR:
			syncSet = (int[]){MULOP, ADDOP, CLOSEPAREN, CLOSEBRACKET, THEN, DO, END, ELSE, COMMA, RELOP, SEMICOLON, ENDOFFILE};
			count = 12;
			break;
		case NT_SIGN:
			syncSet = (int[]){ID, NUM, NOT, OPENPAREN, ENDOFFILE};
			count = 5;
			break;
	}
	int index = 0;
	while(index < count) {
		if(syncSet[index] == tokType) 
			return 1;
		index++;
	}
	return 0;
}
void printMatchSynerr(char * matchType) {
	if(tok->tokenName == LEXERR)
		return;//don't print lexerrs here
	char * tokStr;
	if(tok->tokenName == ID || tok->tokenName == NUM || tok->tokenName == ASSIGNOP || tok->tokenName == OPENPAREN || tok->tokenName == CLOSEPAREN ||
		tok->tokenName == COMMA || tok->tokenName == SEMICOLON || tok->tokenName == COLON || tok->tokenName == PERIOD || tok->tokenName == ARRAYRANGE || 
		tok->tokenName == ENDOFFILE || tok->tokenName == RELOP || tok->tokenName == ADDOP || tok->tokenName == MULOP || tok->tokenName == OPENBRACKET || tok->tokenName == CLOSEBRACKET) {
		tokStr = tokenNameToString(tok->tokenName);
	}
	else {
		tokStr = attributeToString(tok->attribute->attrInt);
	}
	fprintf(listingFile, "SYNERR expecting %s, received %s\n", matchType, tokStr);
}

void printSynerr(char * neededTypes, char * nonterminal) {
	if(tok->tokenName == LEXERR)
		return;//don't print lexerrs here
	char * tokStr;
	if(tok->tokenName == ID || tok->tokenName == NUM || tok->tokenName == ASSIGNOP || tok->tokenName == OPENPAREN || tok->tokenName == CLOSEPAREN ||
		tok->tokenName == COMMA || tok->tokenName == SEMICOLON || tok->tokenName == COLON || tok->tokenName == PERIOD || tok->tokenName == ARRAYRANGE || 
		tok->tokenName == ENDOFFILE || tok->tokenName == RELOP || tok->tokenName == ADDOP || tok->tokenName == MULOP || tok->tokenName == OPENBRACKET || tok->tokenName == CLOSEBRACKET) {
		tokStr = tokenNameToString(tok->tokenName);
	}
	else {
		tokStr = attributeToString(tok->attribute->attrInt);
	}
	fprintf(listingFile, "SYNERR in %s, expecting %s, received %s\n", nonterminal, neededTypes, tokStr);
}

void finish() {
	puts("program finished");
}
