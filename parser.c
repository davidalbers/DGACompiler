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
	else if(tok->attribute->attrInt == ARRAY) {
		match(ARRAY);
		match(OPENBRACKET);
		match(NUM);
		match(ARRAYRANGE);
		match(NUM);
		match(CLOSEBRACKET);
		match(OF);
		stdType();
	}
	else 
		puts("synerr in type expecting int or real or array");
} 

void stdType() {
	if(tok->tokenName == INT)
		match(INT);
	else if(tok->tokenName == REAL) 
		match(REAL);
	else 
		puts("synerr in stdType expecting int or real");	
}

void subPrgDecls() {
	if(tok->attribute->attrInt == FUNCTION) {
		subPrgDecl();
		match(SEMICOLON);
		subPrgDeclsPrime();
	}
	else 
		puts("synerr in subPrgDecls expecting fun");
}

void subPrgDeclsPrime() {
	if(tok->attribute->attrInt == FUNCTION) {
		subPrgDecls();
		match(SEMICOLON);
		subPrgDeclsPrime();
	}
	else if(tok->attribute->attrInt == BEGIN)
		return; //epsilon
	else 
		puts("synerr in subPrgDecls' expecting fun or epsilon");
}

void subPrgDecl() {
	if(tok->attribute->attrInt == FUNCTION) {
		subPrgHead();
		subPrgDeclPrime();
	}
	else 
		puts("synerr subprgdecl fun");
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
	else 
		puts("synerr subprgdecl' var begin fun");
}

void subPrgDeclPrime2() {
	if(tok->attribute->attrInt == BEGIN) {
		cpdStmt();
	}
	else if(tok->attribute->attrInt == FUNCTION) {
		subPrgDecls();
		cpdStmt();
	}
	else 
		puts("synerr subprgdecl''  begin fun");
}

void subPrgHead() {
	if(tok->attribute->attrInt == FUNCTION) {
		match(FUNCTION);
		match(ID);
		subPrgHeadPrime();
	}
	else 
		puts("synerr in subPrgHead expecting fun");
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
	else
		puts("synerr in subPrgHeadPrime expecting ( or : ");
}

void args() {
	if(tok->tokenName == OPENPAREN) {
		match(OPENPAREN);
		paramLst();
		match(CLOSEPAREN);
	}
	else
		puts("synerr in args expecting (");
}

void paramLst() {
	if(tok->tokenName == ID) {
		match(ID);
		match(COLON);
		type();
		paramLstPrime();
	}
	else
		puts("synerr in paramLst expecting ID");
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
	else 
		puts("synerr in paramLst' expecting : or )");
}

void cpdStmt() {
	if(tok->attribute->attrInt == BEGIN) {
		match(BEGIN);
		cpdStmtPrime();
	}
	else 
		puts("synerr in cpdStmt, begin");
}

void cpdStmtPrime() {
	if(tok->tokenName == ID || tok->attribute->attrInt == BEGIN || tok->attribute->attrInt == IF || tok->attribute->attrInt == WHILE) {
		optStmts();
		match(END);
	}
	else if(tok->attribute->attrInt == END) {
		match(END);
	}
	else 
		puts("synerr cpdStmt, id begin, if while, end");
}

void optStmts() {
	if(tok->tokenName == ID || tok->attribute->attrInt == BEGIN || tok->attribute->attrInt == IF || tok->attribute->attrInt == WHILE) {
		stmtLst();
	}
	else 
		puts("synerr optStmt, id begin, if while");
}

void stmtLst() {
	if(tok->tokenName == ID || tok->attribute->attrInt == BEGIN || tok->attribute->attrInt == IF || tok->attribute->attrInt == WHILE) {
		stmt();
		stmtLstPrime();
	}
	else 
		puts("synerr stmtLst, id begin, if while");
}

void stmtLstPrime() {
	if(tok->tokenName == SEMICOLON) {
		match(SEMICOLON);
		stmt();
		stmtLstPrime();
	}
	else if (tok->attribute->attrInt == END) 
		return;//epsilon
	else 
		puts("synerr stmtLstPrime, ; end ");
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
	else 
		puts("synerr stmt, id begin, if while");
}

void stmtPrime() {
	if(tok->attribute->attrInt == END) 
		return;//epsilon
	else if(tok->attribute->attrInt == ELSE) {
		match(ELSE);
		stmt();
	}
	else 
		puts("synerr stmtPrime, end else");
}

void var() {
	if(tok->tokenName == ID) {
		match(ID);
		varPrime();
	}
	else
		puts("synerr var, ID");
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
	else 
		puts("synerr var', id [ :=");
}


void expLst() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM ||
		tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT || tok->attribute->attrInt == NOT) {
		expr();
		expLstPrime();
	}
	else 
		puts("synerr expLst ( id num + - NOT");
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
	else 
		puts("synerr explst' , )");
}

void expr() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM ||
		tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT || tok->attribute->attrInt == NOT) {
		simExp();
		exprPrime();
	}
	else
		puts("synerr exp ( id num + - not");
}

void exprPrime() {
	if(tok->tokenName == OPENPAREN || tok->attribute->attrInt == END || tok->tokenName == CLOSEBRACKET ||
	 tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO) {
		return;//epsilon
	}
	else if(tok->tokenName == RELOP) {
		match(RELOP);
		simExp();
	}
	else 
		puts("synerr");
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
}

void simExpPrime() {
	if(tok->tokenName == CLOSEPAREN || tok->attribute->attrInt == END || tok->tokenName == CLOSEBRACKET || tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO) {
		return;//epsilon
	}
	else if(tok->tokenName == ADDOP) {
		match(ADDOP);
		term();
		simExpPrime();
	}
	else 
		puts("simexp' ) end ] then do relop");
}

void term() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM || tok->attribute->attrInt == NOT) {
		factor();
		termPrime();
	}
	else
		puts("synerr term ( id num not");
}

void termPrime() {
	if(tok->tokenName == CLOSEPAREN || tok->attribute->attrInt == END || tok->tokenName == CLOSEBRACKET || 
			tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == ADDOP) {
		return;//epsilon
	}
	else if(tok->tokenName == MULOP) {
		match(MULOP);
		factor();
		term();
	}
	else 
		puts("simexp' ) end ] then do relop");
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
	else 
		puts("synerr factor ( id num not");
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
		tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->attribute->attrInt == ADDOP || tok->attribute->attrInt == MULOP){
		return;//epsilon
	}
	else 
		puts("synerr factor' ( [ ) end ] then do addop mulop");
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
		type == COMMA || type == SEMICOLON || type == COLON || type == PERIOD || type == ARRAYRANGE || 
		type == ENDOFFILE || type == REAL || type == INT) {
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
