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
		puts("synerr in program expecting id");
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
			if(!match(PERIOD)) {synchType(NT_PROGRAM); return;}
			break;
		default:
			puts("synerr in program' expecting var, begin, or fun");
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
			puts("synerr in program' expecting var, begin, or fun");
			synchType(NT_PROGRAM);	
	}
}
void idLst() {
	if(tok->tokenName == ID) {
		if(!match(ID)) {synchType(NT_IDLST); break;}
		idLstPrime();
	}
	else {
		puts("synerr in idLst expecting id");
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
			puts("synerr in idlst' expecting id or semicolon");
			synchType(NT_IDLST);
	}

}

void decls() {
	switch(tok->attribute->attrInt) {
		case VAR:
			if(!match(VAR)) {synchType(NT_DECLS); break;}
			if(!match(ID) {synchType(NT_DECLS); break;}
			if(!match(COLON)) {synchType(NT_DECLS); break;}
			type();
			if(!match(SEMICOLON)) {synchType(NT_DECLS); break;}
			declsPrime();
			break;
		default: 
			puts("synerr in decls expecting var");
			synchType(NT_DECLS);
	}
}

void declsPrime() {
	switch(tok->attribute->attrInt) {
		case VAR:
			if(!match(VAR)) {synchType(NT_DECLS); break;}
			if(!match(ID) {synchType(NT_DECLS); break;}
			if(!match(COLON)) {synchType(NT_DECLS); break;}
			type();
			if(!match(SEMICOLON)) {synchType(NT_DECLS); break;}
			declsPrime();
			break;
			break;
		case BEGIN:
		case FUNCTION:
			break; //epsilon
		default:
			puts("synerr in program' expecting var, begin, or fun");
			synchType(NT_DECLS);
	}
}

void type() {
	if(tok->attribute->attrInt == INTEGER || tok->attribute->attrInt == REAL) {
		stdType();
	}
	else if(tok->attribute->attrInt == ARRAY) {
		if(!match(ARRAY)) {synchType(NT_TYPE); break;}
		if(!match(OPENBRACKET)) {synchType(NT_TYPE); break;}
		if(!match(NUM)) {synchType(NT_TYPE); break;}
		if(!match(ARRAYRANGE)) {synchType(NT_TYPE); break;}
		if(!match(NUM)) {synchType(NT_TYPE); break;}
		if(!match(CLOSEBRACKET)) {synchType(NT_TYPE); break;}
		if(!match(OF)) {synchType(NT_TYPE); break;}
		stdType();
	}
	else {
		puts("synerr in type expecting int or real or array");
		printf("\nmore %d %d\n",tok->tokenName,tok->attribute->attrInt);
	
		synchType(NT_TYPE);
	}
} 

void stdType() {
	if(tok->attribute->attrInt == INTEGER)
		if(!match(INTEGER)) {synchType(NT_STDTYPE); break;}
	else if(tok->attribute->attrInt == REAL) 
		if(!match(REAL)) {synchType(NT_STDTYPE); break;}
	else {
		puts("synerr in stdType expecting int or real");	
		synchType(NT_STDTYPE);
	}
}

void subPrgDecls() {
	if(tok->attribute->attrInt == FUNCTION) {
		subPrgDecl();
		if(!match(SEMICOLON)) {synchType(NT_SUBPRGDECLS); break;}
		subPrgDeclsPrime();
	}
	else {
		puts("synerr in subPrgDecls expecting fun");
		synchType(NT_SUBPRGDECLS);
	}
}

void subPrgDeclsPrime() {
	if(tok->attribute->attrInt == FUNCTION) {
		subPrgDecls();
		if(!match(SEMICOLON)) {synchType(NT_SUBPRGDECLS); break;}
		subPrgDeclsPrime();
	}
	else if(tok->attribute->attrInt == BEGIN)
		return; //epsilon
	else {
		puts("synerr in subPrgDecls' expecting fun or epsilon");
		synchType(NT_SUBPRGDECLS);
	}
}

void subPrgDecl() {
	if(tok->attribute->attrInt == FUNCTION) {
		subPrgHead();
		subPrgDeclPrime();
	}
	else {
		puts("synerr subprgdecl fun");
		int syncSet[] = {SEMICOLON,ENDOFFILE};
		while(!synch(tok,syncSet,2)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
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
		puts("synerr subprgdecl' var begin fun");
		int syncSet[] = {SEMICOLON,ENDOFFILE};
		while(!synch(tok,syncSet,2)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
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
		puts("synerr subprgdecl''  begin fun");
		int syncSet[] = {SEMICOLON,ENDOFFILE};
		while(!synch(tok,syncSet,2)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void subPrgHead() {
	if(tok->attribute->attrInt == FUNCTION) {
		match(FUNCTION);
		match(ID);
		subPrgHeadPrime();
	}
	else {
		puts("synerr in subPrgHead expecting fun");
		int syncSet[] = {VAR,BEGIN,FUNCTION,ENDOFFILE};
		while(!synch(tok,syncSet,4)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void subPrgHeadPrime() {
	if(tok->tokenName == OPENPAREN) {
		args();
		match(COLON);
		stdType();
		match(SEMICOLON);
	}
	else if(tok->tokenName == COLON) {
		match(COLON);
		stdType();
		match(SEMICOLON);
	}
	else {
		puts("synerr in subPrgHeadPrime expecting ( or : ");
		int syncSet[] = {VAR,BEGIN,FUNCTION,ENDOFFILE};
		while(!synch(tok,syncSet,4)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void args() {
	if(tok->tokenName == OPENPAREN) {
		match(OPENPAREN);
		paramLst();
		match(CLOSEPAREN);
	}
	else {
		puts("synerr in args expecting (");
		int syncSet[] = {COLON, ENDOFFILE};
		while(!synch(tok,syncSet,2)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void paramLst() {
	if(tok->tokenName == ID) {
		match(ID);
		match(COLON);
		type();
		paramLstPrime();
	}
	else {
		puts("synerr in paramLst expecting ID");
		int syncSet[] = {CLOSEPAREN, ENDOFFILE};
		while(!synch(tok,syncSet,2)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void paramLstPrime() {
	if(tok->tokenName == SEMICOLON) {
		match(SEMICOLON);
		match(ID);
		match(COLON);
		type();
		paramLstPrime();
	}
	else if(tok->tokenName == CLOSEPAREN) 
		return; //epsilon
	else {
		puts("synerr in paramLst' expecting : or )");
		int syncSet[] = {CLOSEPAREN, ENDOFFILE};
		while(!synch(tok,syncSet,2)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}

	}
}

void cpdStmt() {
	if(tok->attribute->attrInt == BEGIN) {
		match(BEGIN);
		cpdStmtPrime();
	}
	else {
		puts("synerr in cpdStmt, begin");
		int syncSet[] = {PERIOD , SEMICOLON , END, ENDOFFILE};
		while(!synch(tok,syncSet,3)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void cpdStmtPrime() {
	if(tok->tokenName == ID || tok->attribute->attrInt == BEGIN || tok->attribute->attrInt == IF || tok->attribute->attrInt == WHILE) {
		optStmts();
		match(END);
	}
	else if(tok->attribute->attrInt == END) {
		match(END);
	}
	else {
		puts("synerr cpdStmt, id begin, if while, end");
		int syncSet[] = {PERIOD , SEMICOLON , END, ENDOFFILE};
		while(!synch(tok,syncSet,3)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void optStmts() {
	if(tok->tokenName == ID || tok->attribute->attrInt == BEGIN || tok->attribute->attrInt == IF || tok->attribute->attrInt == WHILE) {
		stmtLst();
	}
	else {
		puts("synerr optStmt, id begin, if while");
		int syncSet[] = {END, ENDOFFILE};
		while(!synch(tok,syncSet,2)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void stmtLst() {
	if(tok->tokenName == ID || tok->attribute->attrInt == BEGIN || tok->attribute->attrInt == IF || tok->attribute->attrInt == WHILE) {
		stmt();
		stmtLstPrime();
	}
	else {
		puts("synerr stmtLst, id begin, if while");
		int syncSet[] = {END, ENDOFFILE};
		while(!synch(tok,syncSet,2)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void stmtLstPrime() {
	if(tok->tokenName == SEMICOLON) {
		match(SEMICOLON);
		stmt();
		stmtLstPrime();
	}
	else if (tok->attribute->attrInt == END) 
		return;//epsilon
	else {
		puts("synerr stmtLstPrime, ; end ");
		int syncSet[] = {END, ENDOFFILE};
		while(!synch(tok,syncSet,2)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void stmt() {
	if(tok->tokenName == ID) {
		var();
		match(ASSIGNOP);
		expr();
	} 
	else if(tok->attribute->attrInt == BEGIN) {
		cpdStmt();
	} 
	else if( tok->attribute->attrInt == IF) {
		match(IF);
		expr();
		match(THEN);
		stmt();
		stmtPrime();
	} 
	else if( tok->attribute->attrInt == WHILE) {
		match(WHILE);
		expr();
		match(DO);
		stmt();
	}
	else {
		puts("synerr stmt, id begin, if while");
		int syncSet[] = {END, ELSE, SEMICOLON, ENDOFFILE};
		while(!synch(tok,syncSet,4)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void stmtPrime() {
	if(tok->attribute->attrInt == END || tok->tokenName == SEMICOLON) 
		return;//epsilon
	else if(tok->attribute->attrInt == ELSE) {
		match(ELSE);
		stmt();
	}
	else {
		puts("synerr stmtPrime, end else");
		int syncSet[] = {END, ELSE, SEMICOLON, ENDOFFILE};
		while(!synch(tok,syncSet,4)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void var() {
	if(tok->tokenName == ID) {
		match(ID);
		varPrime();
	}
	else{
		puts("synerr var, ID");
		int syncSet[] = {ASSIGNOP, ID, ENDOFFILE};
		while(!synch(tok,syncSet,3)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void varPrime() {
	if(tok->tokenName == ID) 
		return;//epsilon
	else if(tok->tokenName == OPENBRACKET) {
		match(OPENBRACKET);
		expr();
		match(CLOSEBRACKET);
	}
	else if(tok->tokenName == ASSIGNOP) {
		return;//epsilon
	}
	else {
		puts("synerr var', id [ :=");
		int syncSet[] = {ASSIGNOP, ID, ENDOFFILE};
		while(!synch(tok,syncSet,3)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}

	}
}


void expLst() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM ||
		tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT || tok->attribute->attrInt == NOT) {
		expr();
		expLstPrime();
	}
	else {
		puts("synerr expLst ( id num + - NOT");
		int syncSet[] = {CLOSEPAREN,ENDOFFILE};
		while(!synch(tok,syncSet,2)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void expLstPrime() {
	if(tok->tokenName == COMMA) {
		match(COMMA);
		expr();
		expLstPrime();	
	}
	else if(tok->tokenName == CLOSEPAREN) {
		return; //epsilon
	}
	else {
		puts("synerr explst' , )");
		int syncSet[] = {CLOSEPAREN,ENDOFFILE};
		while(!synch(tok,syncSet,2)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void expr() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM ||
		tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT || tok->attribute->attrInt == NOT) {
		simExp();
		exprPrime();
	}
	else {
		puts("synerr exp ( id num + - not");
		int syncSet[] = {CLOSEPAREN,CLOSEBRACKET,THEN,DO,END,ELSE,COMMA,SEMICOLON,ENDOFFILE};
		while(!synch(tok,syncSet,9)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void exprPrime() { 
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == SEMICOLON || tok->attribute->attrInt == END || tok->tokenName == CLOSEBRACKET ||
	 tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == COMMA || 
	 tok->attribute->attrInt == ELSE ) {
		return;//epsilon
	}
	else if(tok->tokenName == RELOP) {
		match(RELOP);
		simExp();
	}
	else {
		puts("synerr exprPrime");
		int syncSet[] = {CLOSEPAREN,CLOSEBRACKET,THEN,DO,END,ELSE,COMMA,SEMICOLON,ENDOFFILE};
		while(!synch(tok,syncSet,9)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
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
	else
		puts("synerr exp ( id num + - not");
		int syncSet[] = {CLOSEPAREN,CLOSEBRACKET,THEN,DO,END,ELSE,COMMA,RELOP,SEMICOLON,ENDOFFILE};
		while(!synch(tok,syncSet,10)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
}

void simExpPrime() {
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == SEMICOLON || tok->tokenName == RELOP || tok->tokenName == CLOSEBRACKET || tok->tokenName == COMMA || tok->attribute->attrInt == END ||
	  tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->attribute->attrInt == ELSE ) {
		return;//epsilon
	}
	else if(tok->tokenName == ADDOP) {
		match(ADDOP);
		term();
		simExpPrime();
	}
	else {
		int syncSet[] = {CLOSEPAREN,CLOSEBRACKET,THEN,DO,END,ELSE,COMMA,RELOP,SEMICOLON,ENDOFFILE};
		puts("synerr simexp' ) end ] then do relop");
		while(!synch(tok,syncSet,10)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void term() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM || tok->attribute->attrInt == NOT) {
		factor();
		termPrime();
	}
	else {
		int syncSet[] = {ADDOP, CLOSEPAREN, CLOSEBRACKET, THEN, DO, END, ELSE, COMMA, RELOP, SEMICOLON, ENDOFFILE};
		puts("synerr term ( id num not");
		while(!synch(tok,syncSet,11)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void termPrime() {
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == RELOP || tok->tokenName == SEMICOLON || tok->tokenName == CLOSEBRACKET || 
			tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == ADDOP ||
			 tok->tokenName == COMMA || tok->attribute->attrInt == ELSE ||tok->attribute->attrInt == END ) {
		return;//epsilon
	}
	else if(tok->tokenName == MULOP) {
		match(MULOP);
		factor();
		term();
	}
	else {
		int syncSet[] = {ADDOP, CLOSEPAREN, CLOSEBRACKET, THEN, DO, END, ELSE, COMMA, RELOP, SEMICOLON, ENDOFFILE};
		puts("synerr term' ) end ] then do addop mulop");
		while(!synch(tok,syncSet,11)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void factor() {
	if(tok->tokenName == OPENPAREN) {
		match(OPENPAREN);
		expr();
		match(CLOSEPAREN);
	}
	else if(tok->tokenName == ID) {
		match(ID);
		factorPrime();
	}
	else if(tok->tokenName == NUM) {
		match(NUM);
	}
	else if(tok->attribute->attrInt == NOT) {
		match(NOT);
		factor();
	}
	else {
		int syncSet[] = {MULOP, ADDOP, CLOSEPAREN, CLOSEBRACKET, THEN, DO, END, ELSE, COMMA, RELOP, SEMICOLON, ENDOFFILE};
		puts("synerr factor ( id num not");
		while(!synch(tok,syncSet,12)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void factorPrime() {
	if(tok->tokenName == OPENPAREN) {
		match(OPENPAREN);
		expr();
		match(CLOSEPAREN);
	}
	else if(tok->tokenName == OPENBRACKET) {
		match(OPENBRACKET);
		expr();
		match(CLOSEBRACKET);
	}
	else if(tok->tokenName == CLOSEPAREN || tok->attribute->attrInt == END || tok->tokenName == CLOSEBRACKET ||
		tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == ADDOP ||
		 tok->attribute->attrInt == MULOP || tok->tokenName == COMMA || tok->tokenName == ELSE || tok->tokenName == RELOP) {
		return;//epsilon
	}
	else {
		int syncSet[] = {MULOP, ADDOP, CLOSEPAREN, CLOSEBRACKET, THEN, DO, END, ELSE, COMMA, RELOP, SEMICOLON, ENDOFFILE};
		puts("synerr factor' ( [ ) end ] then do addop mulop");
		while(!synch(tok,syncSet,12)) {
			printf("syncing %d", tok->tokenName);
			tok = getNextToken();
		}
	}
}

void sign() {
	int syncSet[] = {ID, NUM, NOT, OPENPAREN, ENDOFFILE};
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
			printf("synerr sign");
			while(!synch(tok,syncSet,4)) {
				printf("syncing %d", tok->tokenName);
				tok = getNextToken();
			}
	}
}

int match(int type) {
	if(type == ID || type == NUM || type == ASSIGNOP || type == OPENPAREN || type == CLOSEPAREN ||
		type == COMMA || type == SEMICOLON || type == COLON || type == PERIOD || type == ARRAYRANGE || 
		type == ENDOFFILE || type == RELOP || type == ADDOP || type == MULOP || type == OPENBRACKET || type == CLOSEBRACKET) {
		if(type == tok->tokenName && type != ENDOFFILE) {
			printf("matched type %s\n", tokenNameToString(type));
			tok = getNextToken();
			return 1;
		}
		else if (type == tok->tokenName && type == ENDOFFILE) {
			finish();
			return 1;
		}
		else if(type != tok->tokenName) {
			printf("synerr tok type is %d and type to match is %d, type name is %s \n", tok->tokenName, type, tokenNameToString(type));
			return 0;
		}
	}
	else {
		if(type == tok->attribute->attrInt && type != ENDOFFILE) {
			printf("matched type %s\n", attributeToString(type));
			tok = getNextToken();
			return 1;
		}
		else if (type == tok->attribute->attrInt && type == ENDOFFILE) {
			finish();
			return 1;
		}
		else if(type != tok->attribute->attrInt) {
			printf("synerr tok type is %d and type to match is %d, attr name is %s\n", tok->attribute->attrInt, type, attributeToString(type));
			return 0;
		}
	}
	return 0;
}
void synchType(int syncingType) {
	while(!synch(tok,syncingType)) {
		printf("syncing %d", tok->tokenName);
		tok = getNextToken();
	}	
}

int synch(struct token * tok, int syncingType) {
	int tokType = 0;
	int * syncSet;
	if(tok->tokenName == ID || tok->tokenName == NUM || tok->tokenName == ASSIGNOP || tok->tokenName == OPENPAREN || tok->tokenName == CLOSEPAREN ||
		tok->tokenName == COMMA || tok->tokenName == SEMICOLON || tok->tokenName == COLON || tok->tokenName == PERIOD || tok->tokenName == ARRAYRANGE || 
		tok->tokenName == ENDOFFILE || tok->tokenName == RELOP || tok->tokenName == ADDOP || tok->tokenName == MULOP || tok->tokenName == OPENBRACKET || tok->tokenName == CLOSEBRACKET) {
		tokType = tok->tokenName;
		switch(syncingType) {
			case PROGRAM:
				syncSet = (int[]){ENDOFFILE};
				break;
			case PROGRAM
		}
	}
	else
		tokType = tok->attribute->attrInt;
	int index = 0;
	while(index < count) {
		if(syncSet[index] == tokType) 
			return 1;
		index++;
	}
	return 0;
}

void finish() {
	puts("program finished");
}
