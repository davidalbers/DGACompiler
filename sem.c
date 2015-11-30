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
struct typeReturn *type();
struct typeReturn *stdType();
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
struct varReturn *var();
struct typeReturn *varPrime();
void expLst();
void expLstPrime();
struct typeReturn *expr();
struct typeReturn *exprPrime(int type);
struct typeReturn *simExp();
struct typeReturn *simExpPrime(int type);
struct typeReturn *term();
void termPrime();
struct typeReturn *factor();
struct typeReturn *matchNum(int nt);
struct typeReturn *matchId(int nt);
struct typeReturn *factorPrime(int type);
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
			char* lexeme = NULL;
			if(tok->tokenName == ID)
				lexeme = tok->attribute->attrString;
			if(!match(ID)) {synchType(NT_DECLS); break;}
			if(!match(COLON)) {synchType(NT_DECLS); break;}
			struct typeReturn *tType = type();
			//todo lexeme.type = type
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
			char* lexeme = NULL;
			if(tok->tokenName == ID)
				lexeme = tok->attribute->attrString;
			if(!match(ID)) {synchType(NT_DECLS); break;}
			if(!match(COLON)) {synchType(NT_DECLS); break;}
			type();
			//todo lexeme.type = type
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

struct typeReturn *type() {
	if(tok->attribute->attrInt == INTEGER || tok->attribute->attrInt == REAL) {
		return stdType();
	}
	else if(tok->attribute->attrInt == ARRAY) {
		if(!match(ARRAY)) {synchType(NT_TYPE); return;}
		if(!match(OPENBRACKET)) {synchType(NT_TYPE); return;}
		if(!match(NUM)) {synchType(NT_TYPE); return;}
		if(!match(ARRAYRANGE)) {synchType(NT_TYPE); return;}
		if(!match(NUM)) {synchType(NT_TYPE); return;}
		if(!match(CLOSEBRACKET)) {synchType(NT_TYPE); return;}
		if(!match(OF)) {synchType(NT_TYPE); return;}
		struct typeReturn *sType = stdType();
		struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
		if(sType->type == INT_TYPE) {
			ret->type = ARRAY_INT_TYPE;
			return ret;
		}
		else if(sType->type == REAL_TYPE) {
			ret->type = ARRAY_REAL_TYPE;
			return ret;
		}
		else {
			ret->encounteredError = 1;
			return ret;
		}
	}
	else {
		printSynerr("intger, real, or array", "type");
		synchType(NT_TYPE);

		struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
		ret->encounteredError = 1;
		return ret;
	}
} 

struct typeReturn *stdType() {
	struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
	if(tok->attribute->attrInt == INTEGER) {
		if(!match(INTEGER)) {
			synchType(NT_STDTYPE); 
			ret->encounteredError = 1;
			return ret;
		}
		ret->type = INT_TYPE;
		return ret;
	}
	else if(tok->attribute->attrInt == REAL) {
		if(!match(REAL)) {
			synchType(NT_STDTYPE); 
			ret->encounteredError = 1;
			return ret;
		}
		ret->type = REAL_TYPE;
		return ret;
	}
	else {
		printSynerr("integer or real", "standard_type");
		synchType(NT_STDTYPE);
		ret->encounteredError = 1;
		return ret;
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
		char *lexeme = tok->attribute->attrString;
		if(!match(ID)) {synchType(NT_PARAMLST); return;}
		if(!match(COLON)) {synchType(NT_PARAMLST); return;}
		struct typeReturn *tType = type();
		//todo lexeme.type = type, fp_type
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
		char *lexeme = tok->attribute->attrString;
		if(!match(ID)) {synchType(NT_PARAMLST); return;}
		if(!match(COLON)) {synchType(NT_PARAMLST); return;}
		struct typeReturn *tType = type();
		//todo lexeme.type = type, fp_type
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
		struct varReturn *vType = var();
		if(!match(ASSIGNOP)) {synchType(NT_STMT); return;}
		struct typeReturn *eType = expr();
		if(eType->encounteredError == 0 && vType->encounteredError == 0) {
			if(vType->type == ARRAY_TYPE && (eType->type == ARRAY_INT_TYPE || eType->type == ARRAY_REAL_TYPE)) {
				//todo create var with type
			}
			else {
				//todo err
			}
		}
		else {
			//todo err
		}

	} 
	else if(tok->attribute->attrInt == BEGIN) {
		cpdStmt();
	} 
	else if( tok->attribute->attrInt == IF) {
		if(!match(IF)) {synchType(NT_STMT); return;}
		struct typeReturn *eType = expr();
		if(eType->type != BOOL_TYPE) {
			//todo err
		}
		if(!match(THEN)) {synchType(NT_STMT); return;}
		stmt();
		stmtPrime();
	} 
	else if( tok->attribute->attrInt == WHILE) {
		if(!match(WHILE)) {synchType(NT_STMT); return;}
		struct typeReturn *eType = expr();
		if(eType->type != BOOL_TYPE) {
			//todo err
		}
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

struct varReturn *var() {
	if(tok->tokenName == ID) {
		char* lexeme = tok->attribute->attrString;
		if(!match(ID)) {synchType(NT_VAR); return;}
		struct typeReturn *vType = varPrime();
		struct varReturn *ret = (struct varReturn *) malloc(sizeof(struct varReturn));
		ret->type = vType->type;
		ret->lexeme = lexeme;
		return ret;
	}
	else{
		printSynerr("id", "variable");
		synchType(NT_VAR);
		struct varReturn *ret = (struct varReturn *) malloc(sizeof(struct varReturn));
		ret->encounteredError = 1;
		return ret;
	}
}

struct typeReturn *varPrime() {
	struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
	if(tok->tokenName == ID) {
		ret->type = ANY_TYPE;
		return ret;//epsilon
	}
	else if(tok->tokenName == OPENBRACKET) {
		if(!match(OPENBRACKET)) {synchType(NT_VAR); return;}
		expr();
		if(!match(CLOSEBRACKET)) {synchType(NT_VAR); return;}
		ret->type = ARRAY_TYPE;
		return ret;//epsilon
	}
	else if(tok->tokenName == ASSIGNOP) {
		ret->type = ANY_TYPE;
		return ret;//epsilon
	}
	else {
		printSynerr("id, open bracket, or assign op", "variable'");
		synchType(NT_VAR);
		ret->encounteredError = 1;
		return ret;
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

struct typeReturn *expr() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM ||
		tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT || tok->attribute->attrInt == NOT) {
		struct typeReturn *sType = simExp();
		return exprPrime(sType->type);
	}
	else {
		printSynerr("open paren, id, num, plus, minus, not", "expression");
		synchType(NT_EXPR);
		struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
		ret->encounteredError = 1;
		return ret;
	}
}

struct typeReturn *exprPrime(int type) { 
	struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == SEMICOLON || tok->attribute->attrInt == END || tok->tokenName == CLOSEBRACKET ||
	 tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == COMMA || 
	 tok->attribute->attrInt == ELSE ) {
	 	ret->type = type;
		return ret;//epsilon
	}
	else if(tok->tokenName == RELOP) {
		if(type != INT_TYPE && type != REAL_TYPE) {
			ret->encounteredError = 1;
			return ret;
		}
		if(!match(RELOP)) {
			synchType(NT_EXPR); 
			ret->encounteredError = 1;
			return ret;
		}
		struct typeReturn *sType = simExp();
		if(sType->type != INT_TYPE && sType->type != REAL_TYPE) {
			ret->encounteredError = 1;
			return ret;
		}
		ret->type = BOOL_TYPE;
		return ret;
	}
	else {
		printSynerr("relop, close paren, semicolon, end, CLOSEBRACKET, then, do, comma, else", "expression");
		synchType(NT_EXPR);
		ret->encounteredError = 1;
		return ret;
	}
}

struct typeReturn *simExp() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM || tok->attribute->attrInt == NOT) {
		struct typeReturn *tType = term();
		return simExpPrime(tType->type);
	}
	else if(tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT) {
		sign();
		struct typeReturn *sType = term();
		if(sType->type == INT_TYPE || sType->type == REAL_TYPE) {
			return simExpPrime(sType->type);
		}
		else {
			struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
			ret->encounteredError = 1;
			return ret;
		}
	}
	else {
		printSynerr("open paren, id, num, plus, minus, not", "simple_expression");
		synchType(NT_SIMEXP);

		struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
		ret->encounteredError = 1;
		return ret;
	}
}

struct typeReturn *simExpPrime(int type) {
	struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == SEMICOLON || tok->tokenName == RELOP || tok->tokenName == CLOSEBRACKET || tok->tokenName == COMMA || tok->attribute->attrInt == END ||
	  tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->attribute->attrInt == ELSE ) {
		ret->type = type;
		return ret;//epsilon
	}
	else if(tok->tokenName == ADDOP) {
		int addOp = tok->attribute->attrInt;
		if(!match(ADDOP)) {
			synchType(NT_SIMEXP);
			ret->encounteredError = 1;
			return ret;
		}
		struct typeReturn *sType = term();
		//make sure only nums are being added or subtracted
		if(addop == ADD || addop == SUBTRACT)
			if(!(type == INT_TYPE || type == REAL_TYPE) || 
				!(sType->type == INT_TYPE || sType->type == REAL_TYPE))  {
				ret->encounteredError = 1;
				return ret;
			}
		}
		//make sure only bools are being OR'd
		if(addop == OR) {
			if(type != BOOL_TYPE || sType->type != BOOL_TYPE) {
				ret->encounteredError = 1;
				return ret;
			}
		}
		return simExpPrime(sType->type);
	}
	else {
		printSynerr("addop, open paren, id, num, plus, minus, not", "simple_expression'");
		synchType(NT_SIMEXP);
		ret->encounteredError = 1;
		return ret;
	}
}

struct typeReturn *term() {
	struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM || tok->attribute->attrInt == NOT) {
		struct typeReturn *tval = factor();
		return termPrime(tval->val);
	}
	else {
		printSynerr("open paren, id, num, not", "term");
		synchType(NT_TERM);
		ret->encounteredError = 1;
		return ret;
	}
}

struct typeReturn *termPrime(int termPIn) {
	struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == RELOP || tok->tokenName == SEMICOLON || tok->tokenName == CLOSEBRACKET || 
			tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == ADDOP ||
			 tok->tokenName == COMMA || tok->attribute->attrInt == ELSE ||tok->attribute->attrInt == END ) {
		ret->val = termPIn;
		return ret;//epsilon
	}
	else if(tok->tokenName == MULOP) {
		int mulop = tok->attribute->attrInt;
		if(!match(MULOP)) {
			synchType(NT_TERM); 
			ret->encounteredError = 1;
			return ret;
		}
		struct typeReturn *fType = factor(termPIn);
		int generatedType = -1;
		if(mulop == MULTIPLY || mulop == DIVIDE) {
			if(!(termPIn == INT_TYPE || termPIn == REAL_TYPE) || 
				!(fType->type == INT_TYPE || fType->type == REAL_TYPE)) {
				ret->encounteredError = 1;
				return ret;
			}
			else 
				generatedType = REAL_TYPE;
		}
		if(mulop == DIV || mulop == MOD) {
			if(termPIn != INT_TYPE || fType->type != INT_TYPE) {
				ret->encounteredError = 1;
				return ret;
			}
			else 
				generatedType = INT_TYPE;
		}
		if(mulop == AND) {
			if(termPrime != BOOL_TYPE || fType-> != BOOL_TYPE) {
				ret->encounteredError = 1;
				return ret;
			}
			else
				generatedType = BOOL_TYPE;
		}
		return termPrime(generatedType);
	}
	else {
		printSynerr("mulop, close paren, relop, semicolon, close bracket, then, do, addop, comma, else, end", "term'");
		synchType(NT_TERM);
		ret->encounteredError = 1;
		return ret;
	}
}

struct typeReturn *factor() {
	struct typeReturn *fail = (struct typeReturn *) malloc(sizeof(struct typeReturn));
	fail->encounteredError = 1;
	if(tok->tokenName == OPENPAREN) {
		if(!match(OPENPAREN)) {synchType(NT_FACTOR); return fail;}
		struct typeReturn *fval = expr();
		if(fval->encounteredError == 1)
			return fail;
		if(!match(CLOSEPAREN)) {synchType(NT_FACTOR); return fail;}
		return fval;
	}
	else if(tok->tokenName == ID) {
		struct typeReturn *idType = matchId(NT_FACTOR);
		if(idType->encounteredError == 1)
			return fail;		
		return factorPrime(idType->type);
	}
	else if(tok->tokenName == NUM) {
		struct typeReturn *fval = matchNum(NT_FACTOR);
		return fval;
		
	}
	else if(tok->attribute->attrInt == NOT) {
		if(!match(NOT)) {synchType(NT_FACTOR); return fail;}
		struct typeReturn *fval = factor();
		if(fval->type != BOOL_TYPE) {
			fval->encounteredError = 1;
		}
		return fval;
	}
	else {
		printSynerr("open paren, id, num, not", "factor");
		synchType(NT_FACTOR);

	}
	return fail;
}

struct typeReturn *matchNum(int nt) {
	struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
	if(tok->tokenName == NUM) {
		ret->val = tok->subName;
	}
	else {
		synchType(nt);
		ret->encounteredError = 1;
	}
	return ret;
}

struct typeReturn *matchId(int nt) {
	struct typeReturn *ret = (struct typeReturn *) malloc(sizeof(struct typeReturn));
	if(tok->tokenName = ID) {
		struct node *sym = getSymbol(tok->attribute->attrString);
		if(sym == NULL) {
			ret->encounteredError = 1;
			return ret;
		}
		ret->type = node->type;
	}
	else {
		synchType(nt);
		ret->encounteredError = 1;
	}
	return ret;
}


struct typeReturn *factorPrime(int type) {
	struct typeReturn *fail = (struct typeReturn *) malloc(sizeof(struct typeReturn));
	fail->encounteredError = 1;
	if(tok->tokenName == OPENPAREN) {
		if(!match(OPENPAREN)) {synchType(NT_FACTOR); return fail;}
		struct typeReturn *eType = expr();
		if(!match(CLOSEPAREN)) {synchType(NT_FACTOR); return fail;}
		return eType; //todo what is id(expLst) in pascal?
	}
	else if(tok->tokenName == OPENBRACKET) {
		//factor->id[exp], if exp is an int and id is an array return type stored in array
		if(!match(OPENBRACKET)) {synchType(NT_FACTOR); return fail;}
		struct typeReturn *eType = expr();
		if(!match(CLOSEBRACKET)) {synchType(NT_FACTOR); return fail;}
		struct typeReturn *fType = (struct typeReturn *) malloc(sizeof(struct typeReturn));

		if(eType->type == INT_TYPE) {
			if(type == ARRAY_INT_TYPE) 
				fType->type = INT_TYPE;
			else if(type == ARRAY_REAL_TYPE)
				fType->type = REAL_TYPE;
			else //neither int or real array
				fType->encounteredError=1;
		}
		else //exp is not an int
			fType->encounteredError=1;
		return fType;
	}
	else if(tok->tokenName == CLOSEPAREN || tok->attribute->attrInt == END || tok->tokenName == CLOSEBRACKET ||
		tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == ADDOP ||
		tok->attribute->attrInt == MULOP || tok->tokenName == COMMA || tok->tokenName == ELSE || tok->tokenName == RELOP) {
		struct typeReturn *fType = (struct typeReturn *) malloc(sizeof(struct typeReturn));
		fType->type = type;
		return fType;//epsilon
	}
	else {
		printSynerr("open paren, open bracket, close paren, end, close bracket, then, do, addop, mulop, comma, else, relop", "factor'");
		synchType(NT_FACTOR);
		return fail;
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
