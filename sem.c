#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
int type();
int stdType();
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
int varPrime(int type);
struct intListNode *expLst();
struct intLstNode *expLstPrime(struct intLstNode* firstNode, struct intLstNode* lastNode);
int expr();
int exprPrime(int type);
int simExp();
int simExpPrime(int type);
int term();
int termPrime();
int factor();
int matchNum(int nt);
int matchId(int nt);
int factorPrime(char *id, int type);
void sign();
int synch(struct token * tok, int syncingType);
void synchType(int syncingType);
void printSynerr(char * neededTypes, char * nonterminal);
void printMatchSynerr(char * matchType);
int typeToFPType(int type);
int checkAddBlueNode(struct blueNode * newBlue);
int checkAddGreenNode(struct greenNode * newGreen);
struct greenNode *popGreenNode();
void pushGreenNode(struct greenNode *newGreen);
void checkAddType(char *lexeme, int type);
char * typeToString(int type);
int typesEquivalent(int type1, int type2);
extern int errno;
int tokenizingLine = 0;
FILE * listingFile;
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
	tok = getNextToken();
	program();
	match(ENDOFFILE);
	//match end of file
}

void program() {
	if(tok->attribute->attrInt == PROGRAM){
		if(!match(PROGRAM)) {synchType(NT_PROGRAM); return;}
		
		char *lexeme = NULL;
		if(tok->tokenName == ID) {
			lexeme = tok->attribute->attrString;
		}
		else {
			puts("failed to add to stack");
			synchType(NT_PROGRAM); return;
		}
		struct greenNode *newGreen = (struct greenNode *)malloc(sizeof(struct greenNode));
		newGreen->id = lexeme;
		newGreen->type = PROGRAM_TYPE;
		tok = getNextToken();
		int success = checkAddGreenNode(newGreen);
		if(success == 0) {
			char *err =  malloc(sizeof(char) * 80);
			strcpy(err, "SEM ERR: identifier with name '");
			strcat(err, lexeme);
			strcat(err, "' has already been declared in this scope\n");
			appendError(err, tokenizingLine);
		}

		//if(!match(ID)) {synchType(NT_PROGRAM); return;}

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
			struct greenNode *popped = popGreenNode();
			if(popped == NULL) {
				puts("Stack is empty");
				return;
			}
			else {
				struct blueNode *currBlue = popped->firstBlue;
				printf("Popped a green node, here are its contents, name %s:\n", popped->id);
				while(currBlue != NULL) {
					printf("%s,%d\n", currBlue->id, currBlue->type);
					currBlue = currBlue->next;
				}
			}
			break;
		case FUNCTION:
			subPrgDecls();
			cpdStmt();
			if(!match(PERIOD)) {synchType(NT_PROGRAM); break;}
			struct greenNode *popped2 = popGreenNode();
			if(popped2 == NULL) {
				puts("Stack is empty");
				return;
			}
			else {
				struct blueNode *currBlue = popped2->firstBlue;
				printf("Popped a green node, here are its contents, name %s:\n", popped2->id);
				while(currBlue != NULL) {
					printf("%s,%d\n", currBlue->id, currBlue->type);
					currBlue = currBlue->next;
				}
			}
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
			struct greenNode *popped = popGreenNode();
			if(popped == NULL) {
				puts("Stack is empty");
				return;
			}
			else {
				struct blueNode *currBlue = popped->firstBlue;
				printf("Popped a green node, here are its contents, name %s:\n", popped->id);
				while(currBlue != NULL) {
					printf("%s,%d\n", currBlue->id, currBlue->type);
					currBlue = currBlue->next;
				}
			}
			break;
		case FUNCTION:
			subPrgDecls();
			cpdStmt();
			if(!match(PERIOD)) {synchType(NT_PROGRAM); break;}
			struct greenNode *popped2 = popGreenNode();
			if(popped2 == NULL) {
				puts("Stack is empty");
				return;
			}
			else {
				struct blueNode *currBlue = popped2->firstBlue;
				printf("Popped a green node, here are its contents, name %s:\n", popped2->id);
				while(currBlue != NULL) {
					printf("%s,%d\n", currBlue->id, currBlue->type);
					currBlue = currBlue->next;
				}
			}
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
			else {
				synchType(NT_PROGRAM); return;
			}
			struct blueNode *newBlue = (struct blueNode *)malloc(sizeof(struct blueNode));
			newBlue->id = lexeme;
			int success = checkAddBlueNode(newBlue);
			if(success == 0) {
				char *err =  malloc(sizeof(char) * 80);
				strcpy(err, "SEM ERR: identifier with name '");
				strcat(err, newBlue->id);
				strcat(err, "' has already been declared in this scope\n");
				appendError(err, tokenizingLine);
			}

			tok = getNextToken();
			//if(!match(ID)) {synchType(NT_DECLS); break;}
			if(!match(COLON)) {synchType(NT_DECLS); break;}
			int tType = type();
			if(!match(SEMICOLON)) {synchType(NT_DECLS); break;}
			checkAddType(lexeme, tType);
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
			else {
				synchType(NT_PROGRAM); return;
			}
			struct blueNode *newBlue = (struct blueNode *)malloc(sizeof(struct blueNode));
			newBlue->id = lexeme;
			int success = checkAddBlueNode(newBlue);
			if(success == 0) {
				char *err =  malloc(sizeof(char) * 80);
				strcpy(err, "SEM ERR: identifier with name '");
				strcat(err, newBlue->id);
				strcat(err, "' has already been declared in this scope\n");
				appendError(err, tokenizingLine);
			}
			
			tok = getNextToken();
			//if(!match(ID)) {synchType(NT_DECLS); break;}
			if(!match(COLON)) {synchType(NT_DECLS); break;}
			int tType = type();
			if(!match(SEMICOLON)) {synchType(NT_DECLS); break;}
			checkAddType(lexeme, tType);
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

int type() {
	if(tok->attribute->attrInt == INTEGER || tok->attribute->attrInt == REAL) {
		return stdType();
	}
	else if(tok->attribute->attrInt == ARRAY) {
		if(!match(ARRAY)) {
			synchType(NT_TYPE); 
			return ERROR_TYPE;
		}
		int currLine =tokenizingLine;
		if(!match(OPENBRACKET)) {
			synchType(NT_TYPE); 
			return ERROR_TYPE;
		}
		if(!match(NUM)) {
			synchType(NT_TYPE); 
			return ERROR_TYPE;
		}
		if(!match(ARRAYRANGE)) {
			synchType(NT_TYPE); 
			return ERROR_TYPE;
		}
		if(!match(NUM)) {
			synchType(NT_TYPE); 
			return ERROR_TYPE;
		}
		if(!match(CLOSEBRACKET)) {
			synchType(NT_TYPE); 
			return ERROR_TYPE;
		}
		if(!match(OF)) {
			synchType(NT_TYPE); 
			return ERROR_TYPE;
		}
		int sType = stdType();
		if(sType == INTEGER || sType == INT_TYPE || sType == FP_INT_TYPE) {
			return ARRAY_INT_TYPE;
		}
		else if(sType == REAL || sType == REAL_TYPE || sType == FP_REAL_TYPE) {
			return ARRAY_REAL_TYPE;
		}
		else {
			char *err =  malloc(sizeof(char) * 80);
			snprintf(err, sizeof(char) * 80, "SEM ERR: Cannot create array containing values of type %s\n", typeToString(sType));
			appendError(err, currLine);
			return ERROR_TYPE;
		}
	}
	else {
		printSynerr("intger, real, or array", "type");
		synchType(NT_TYPE);

		return ERROR_TYPE;
	}
} 

int stdType() {
	if(tok->attribute->attrInt == INTEGER) {
		if(!match(INTEGER)) {
			synchType(NT_STDTYPE); 
			return ERROR_TYPE;
		}
		return INT_TYPE;
	}
	else if(tok->attribute->attrInt == REAL) {
		if(!match(REAL)) {
			synchType(NT_STDTYPE); 
			return ERROR_TYPE;
		}
		return REAL_TYPE;
	}
	else {
		printSynerr("integer or real", "standard_type");
		synchType(NT_STDTYPE);
		return ERROR_TYPE;
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
		subPrgDecl();
        if(!match(SEMICOLON)) {puts("messed up subprgdecls'"); synchType(NT_SUBPRGDECLS); return;}
		struct greenNode *popped = popGreenNode();
		if(popped == NULL) {
			puts("Stack is empty");
			return;
		}
		else {
			struct blueNode *currBlue = popped->firstBlue;
			printf("Popped a green node, here are its contents, name %s:\n", popped->id);
			while(currBlue != NULL) {
				printf("%s,%d\n", currBlue->id, currBlue->type);
				currBlue = currBlue->next;
			}
		}
		subPrgDeclsPrime();
	}
	else if(tok->attribute->attrInt == BEGIN){
 		struct greenNode *popped = popGreenNode();
		if(popped == NULL) {
			puts("Stack is empty");
			return;
		}
		else {
			struct blueNode *currBlue = popped->firstBlue;
			printf("Popped a green node, here are its contents, name %s:\n", popped->id);
			while(currBlue != NULL) {
				printf("%s,%d\n", currBlue->id, currBlue->type);
				currBlue = currBlue->next;
			}
		}
		return; //epsilon
	}
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
		char *lexeme = NULL;
		if(tok->tokenName == ID) {
			lexeme = tok->attribute->attrString;
		}
		else {
			synchType(NT_PROGRAM); return;
		}
		struct greenNode *newGreen = (struct greenNode *)malloc(sizeof(struct greenNode));
		newGreen->id = lexeme;
		newGreen->type = FUNCTION_TYPE;
		int success = checkAddGreenNode(newGreen);
		if(success == 0) {
			char *err =  malloc(sizeof(char) * 80);
			strcpy(err, "SEM ERR: identifier with name '");
			strcat(err, newGreen->id);
			strcat(err, "' has already been declared in this scope\n");
			appendError(err, tokenizingLine);
		}
		tok = getNextToken();
		//if(!match(ID)) {synchType(NT_SUBPRGHEAD); return;}
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
		char* lexeme = NULL;
		if(tok->tokenName == ID)
			lexeme = tok->attribute->attrString;
		else {
			synchType(NT_PROGRAM); return;
		}

		struct blueNode *newBlue = (struct blueNode *)malloc(sizeof(struct blueNode));
		newBlue->id = lexeme;
		int success = checkAddBlueNode(newBlue);
		if(success == 0) {
			char *err =  malloc(sizeof(char) * 80);
			strcpy(err, "SEM ERR: identifier with name '");
			strcat(err, newBlue->id);
			strcat(err, "' has already been declared in this scope\n");
			appendError(err, tokenizingLine);
		}
		topGreen->numParams = topGreen->numParams + 1;
		tok = getNextToken();
		//if(!match(ID)) {synchType(NT_PARAMLST); return;}
		if(!match(COLON)) {synchType(NT_PARAMLST); return;}
		int tType = type();
		int fpType = typeToFPType(tType);
		checkAddType(lexeme, fpType);
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
		char* lexeme = NULL;
		if(tok->tokenName == ID)
			lexeme = tok->attribute->attrString;
		else {
			synchType(NT_PROGRAM); return;
		}
		struct blueNode *newBlue = (struct blueNode *)malloc(sizeof(struct blueNode));
		newBlue->id = lexeme;
		int success = checkAddBlueNode(newBlue);
		if(success == 0) {
			char *err =  malloc(sizeof(char) * 80);
			strcpy(err, "SEM ERR: identifier with name '");
			strcat(err, newBlue->id);
			strcat(err, "' has already been declared in this scope\n");
			appendError(err, tokenizingLine);
		}
		topGreen->numParams = topGreen->numParams + 1;
		tok = getNextToken();
		//if(!match(ID)) {synchType(NT_PARAMLST); return;}
		if(!match(COLON)) {synchType(NT_PARAMLST); return;}
		int tType = type();
		int fpType = typeToFPType(tType);
		checkAddType(lexeme, tType);
		paramLstPrime();
	}
	else if(tok->tokenName == CLOSEPAREN) 
		return; //epsilon
	else {
		printSynerr("semicolon or close paren", "param_list'");
		synchType(NT_PARAMLST); 
	}
}

int typeToFPType(int type) {
	switch(type) {
		case INT_TYPE:
			return FP_INT_TYPE;
		case BOOL_TYPE:
			return FP_BOOL_TYPE;
		case ARRAY_INT_TYPE:
			return FP_ARRAY_INT_TYPE;
		case ARRAY_REAL_TYPE:
			return FP_ARRAY_REAL_TYPE;
		case REAL_TYPE:
			return FP_REAL_TYPE;
		default:
			//err, should not happen
			return ERROR_TYPE;
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
		//keep track of the line this starts on so you can print an error on that line
		int currLine = tokenizingLine; 
		struct varReturn *vType = var();
		if(!match(ASSIGNOP)) {synchType(NT_STMT); return;}
		int eType = expr();
		if(eType != ERROR_TYPE && vType->type != ERROR_TYPE) {
			if(typesEquivalent(vType->type, eType) == 0) {
				char *err =  malloc(sizeof(char) * 160);
				snprintf(err, sizeof(char) * 160, "SEM ERR: Cannot assign value of type %s,%d to variable of type %s,%d.\n", typeToString(eType),eType, typeToString(vType->type),vType->type);
				appendError(err, currLine);
			}
		}

	} 
	else if(tok->attribute->attrInt == BEGIN) {
		cpdStmt();
	} 
	else if( tok->attribute->attrInt == IF) {
		if(!match(IF)) {synchType(NT_STMT); return;}
		int eType = expr();
		if(eType != BOOL_TYPE && eType != ERROR_TYPE) {
			puts("Got error in if");
			char *err = malloc(sizeof(char) * 80);
			strcpy(err, "SEM ERR: Expecting bool type in 'if' condition");
			appendError(err, tokenizingLine);
		}
		if(!match(THEN)) {synchType(NT_STMT); return;}
		stmt();
		stmtPrime();
	} 
	else if( tok->attribute->attrInt == WHILE) {
		if(!match(WHILE)) {synchType(NT_STMT); return;}
		int eType = expr();
		if(eType != BOOL_TYPE && eType != ERROR_TYPE) {
			appendError("SEM ERR: Expecting bool type in 'while' condition\n", tokenizingLine);
		}
		if(!match(DO)) {synchType(NT_STMT); return;}
		stmt();
	}
	else {
		printSynerr("id, begin, if, or while", "statement");
		synchType(NT_STMT);
	}
}

int typesEquivalent(int type1, int type2) {
	if(type1 == INT_TYPE || type1 == FP_INT_TYPE)
		return (type2 == INT_TYPE || type2 == FP_INT_TYPE);
	if(type1 == REAL_TYPE || type2 == FP_REAL_TYPE)
		return (type2 == REAL_TYPE || type2 == FP_REAL_TYPE);
	if(type1 == BOOL_TYPE || type1 == FP_BOOL_TYPE) 
		return (type2 == BOOL_TYPE || type2 == FP_BOOL_TYPE); 
	if(type1 == ARRAY_INT_TYPE || type1 == FP_ARRAY_INT_TYPE)
		return (type2 == ARRAY_INT_TYPE || type2 == FP_ARRAY_INT_TYPE);
	if(type1 == ARRAY_REAL_TYPE || type1 == FP_ARRAY_REAL_TYPE)
		return (type2 == ARRAY_REAL_TYPE || type2 == FP_ARRAY_REAL_TYPE);
	return 0;
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
		struct varReturn *ret = (struct varReturn *) malloc(sizeof(struct varReturn));
		char* lexeme = tok->attribute->attrString;
		int idType = matchId(NT_VAR); 
		if(idType == ERROR_TYPE) {
			char *err =  malloc(sizeof(char) * 80);
			strcpy(err, "SEM ERR: identifier '");
			strcat(err, lexeme);
			strcat(err, "' has not been declared\n");
			appendError(err, tokenizingLine);
		}
		int vType = varPrime(idType);
		ret->type = vType;
		ret->lexeme = lexeme;
		return ret;
	}
	else{
		printSynerr("id", "variable");
		synchType(NT_VAR);
		struct varReturn *ret = (struct varReturn *) malloc(sizeof(struct varReturn));
		ret->type = ERROR_TYPE;
		return ret;
	}
}

int varPrime(int type) {
	if(tok->tokenName == ID) {
		return type;
	}
	else if(tok->tokenName == OPENBRACKET) {

		if(!match(OPENBRACKET)) { synchType(NT_VAR); return ERROR_TYPE; }
		int eType = expr();
		int currLine = tokenizingLine;
		if(!match(CLOSEBRACKET)) { synchType(NT_VAR); return ERROR_TYPE; }
		//array indices have to be ints
		if(eType != INT_TYPE) { 
			char *err =  malloc(sizeof(char) * 80);
			snprintf(err, sizeof(char) * 80, "SEM ERR: Array indices are of type int, you're using type %s\n", typeToString(eType));
			appendError(err, currLine);
			return ERROR_TYPE; 
		}
		//return the type which is being contained in the array
		if(type == ARRAY_INT_TYPE || type == FP_ARRAY_INT_TYPE) { return INT_TYPE; }
		else if(type == ARRAY_REAL_TYPE || type == FP_ARRAY_REAL_TYPE) { return REAL_TYPE; }
		else { 
			char *err =  malloc(sizeof(char) * 80);
			snprintf(err, sizeof(char) * 80, "SEM ERR: Type %s is not an array and is not indexed\n", typeToString(type));
			appendError(err, currLine);
			return ERROR_TYPE; 
		}
	}
	else if(tok->tokenName == ASSIGNOP) {
		return type;
	}
	else {
		printSynerr("id, open bracket, or assign op", "variable'");
		synchType(NT_VAR);
		return ERROR_TYPE;
	}
}


struct intLstNode *expLst() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM ||
		tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT || tok->attribute->attrInt == NOT) {
		int eType = expr();
		struct intLstNode *firstNode = (struct intLstNode *)malloc(sizeof(struct intLstNode));
		firstNode->val = eType;
		return expLstPrime(firstNode, firstNode);
	}
	else {
		printSynerr("open paren, id, num, plus, minus, not", "expression_list");
		synchType(NT_EXPLST);
		return NULL;
	}
}

struct intLstNode *expLstPrime(struct intLstNode* firstNode, struct intLstNode* lastNode) {
	if(tok->tokenName == COMMA) {
		if(!match(COMMA)) {synchType(NT_EXPLST); return NULL;}
		int eType = expr();
		struct intLstNode *newNode = (struct intLstNode *)malloc(sizeof(struct intLstNode));
		newNode->val = eType;
		lastNode->next = newNode;
		return expLstPrime(firstNode, newNode);	
	}
	else if(tok->tokenName == CLOSEPAREN) {
		return firstNode; //epsilon
	}
	else {
		printSynerr("comma, close paren", "expression_list");
		synchType(NT_EXPLST);
		return NULL;
	}
}

int expr() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM ||
		tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT || tok->attribute->attrInt == NOT) {
		int sType = simExp();
		return exprPrime(sType);
	}
	else {
		printSynerr("open paren, id, num, plus, minus, not", "expression");
		synchType(NT_EXPR);
		return ERROR_TYPE;
	}
}

int exprPrime(int type) { 
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == SEMICOLON || tok->attribute->attrInt == END || tok->tokenName == CLOSEBRACKET ||
	 tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == COMMA || 
	 tok->attribute->attrInt == ELSE ) {
	 	return type;
	}
	else if(tok->tokenName == RELOP) {
		if(!match(RELOP)) {synchType(NT_EXPR); return ERROR_TYPE;}
		int currLine =tokenizingLine;
		int sType = simExp();
		int generatedType = BOOL_TYPE;
		if(sType != INT_TYPE && sType != REAL_TYPE && sType != ERROR_TYPE) {
			char *err =  malloc(sizeof(char) * 160);
			snprintf(err, sizeof(char) * 160, "SEM ERR: cannot apply relation operators to type %s\n", typeToString(sType));
			appendError(err, currLine);
			generatedType = ERROR_TYPE;
		}
		if (type != INT_TYPE && type != REAL_TYPE && type != ERROR_TYPE) {
			char *err =  malloc(sizeof(char) * 160);
			snprintf(err, sizeof(char) * 160, "SEM ERR: cannot apply relation operators to type %s\n", typeToString(type));
			appendError(err, currLine);
			generatedType = ERROR_TYPE;
		}
		return generatedType;
	}
	else {
		printSynerr("relop, close paren, semicolon, end, CLOSEBRACKET, then, do, comma, else", "expression");
		synchType(NT_EXPR);
		return ERROR_TYPE;
	}
}

int simExp() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM || tok->attribute->attrInt == NOT) {
		int tType = term();
		return simExpPrime(tType);
	}
	else if(tok->attribute->attrInt == ADD || tok->attribute->attrInt == SUBTRACT) {
		sign();
        int currLine = tokenizingLine;
		int sType = term();
		if(sType == INT_TYPE || sType == REAL_TYPE || sType == FP_INT_TYPE || sType == FP_REAL_TYPE) {
			return simExpPrime(sType);
		}
		else {
            char *err =  malloc(sizeof(char) * 80);
            snprintf(err, sizeof(char) * 80, "SEM ERR: type %s cannot be signed\n", typeToString(sType));
            appendError(err, currLine);
			return ERROR_TYPE;
		}
	}
	else {
		printSynerr("open paren, id, num, plus, minus, not", "simple_expression");
		synchType(NT_SIMEXP);

		return ERROR_TYPE;
	}
}

int simExpPrime(int type) {
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == SEMICOLON || tok->tokenName == RELOP || tok->tokenName == CLOSEBRACKET || tok->tokenName == COMMA || tok->attribute->attrInt == END ||
	  tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->attribute->attrInt == ELSE ) {
		return type;
	}
	else if(tok->tokenName == ADDOP) {
		int addOp = tok->attribute->attrInt;
		if(!match(ADDOP)) {
			synchType(NT_SIMEXP);
			return ERROR_TYPE;
		}
		int currLine = tokenizingLine;
		int sType = term();
		int generatedType = ERROR_TYPE;
		//make sure only nums are being added or subtracted
		if(addOp == ADD || addOp == SUBTRACT) {
			if(!(type == INT_TYPE || type == REAL_TYPE || type == FP_INT_TYPE || type == FP_REAL_TYPE) || 
				!(sType == INT_TYPE || sType == REAL_TYPE || sType == FP_INT_TYPE || sType == FP_REAL_TYPE))  {
				char *err =  malloc(sizeof(char) * 80);
				if(addOp == ADD)
					snprintf(err, sizeof(char) * 80, "SEM ERR: cannot add type %s to type %s\n", typeToString(type), typeToString(sType));
				else 
					snprintf(err, sizeof(char) * 80, "SEM ERR: cannot subtract type %s to type %s\n", typeToString(type), typeToString(sType));
				if(sType != ERROR_TYPE && type != ERROR_TYPE)
					appendError(err, currLine);
				generatedType = ERROR_TYPE;
			} 
			else{
				if(sType == REAL_TYPE || sType == FP_REAL_TYPE || type == REAL_TYPE || type == FP_REAL_TYPE)
					generatedType = REAL_TYPE; //any type real? total expr becomes real
				else
					generatedType = INT_TYPE; //else stays int
			}
		}
		//make sure only bools are being OR'd
		if(addOp == OR) {
			if(type != BOOL_TYPE || sType != BOOL_TYPE) {
				char *err =  malloc(sizeof(char) * 80);
				snprintf(err, sizeof(char) * 80, "SEM ERR: cannot or type %s to type %s\n", typeToString(type), typeToString(sType));
				if(sType != ERROR_TYPE && type != ERROR_TYPE)
					appendError(err, currLine);
				generatedType = ERROR_TYPE;
			}
			else {
				generatedType = BOOL_TYPE;
			}
		}

		return simExpPrime(generatedType);
	}
	else {
		printSynerr("addop, open paren, id, num, plus, minus, not", "simple_expression'");
		synchType(NT_SIMEXP);
		return ERROR_TYPE;
	}
}

int term() {
	if(tok->tokenName == OPENPAREN || tok->tokenName == ID || tok->tokenName == NUM || tok->attribute->attrInt == NOT) {
		int tval = factor();
		return termPrime(tval);
	}
	else {
		printSynerr("open paren, id, num, not", "term");
		synchType(NT_TERM);
		return ERROR_TYPE;
	}
}

int termPrime(int termPIn) {
	if(tok->tokenName == CLOSEPAREN || tok->tokenName == RELOP || tok->tokenName == SEMICOLON || tok->tokenName == CLOSEBRACKET || 
			tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == ADDOP ||
			 tok->tokenName == COMMA || tok->attribute->attrInt == ELSE ||tok->attribute->attrInt == END ) {
		return termPIn;
	}
	else if(tok->tokenName == MULOP) {
		
		int mulop = tok->attribute->attrInt;
		if(!match(MULOP)) {
			synchType(NT_TERM); 
			return ERROR_TYPE;
		}
		int currLine = tokenizingLine;
		int fType = factor(termPIn);
		int generatedType = ERROR_TYPE;
		if(mulop == MULTIPLY || mulop == DIVIDE) {
			if(!(termPIn == INT_TYPE || termPIn == REAL_TYPE || termPIn == FP_INT_TYPE || termPIn == FP_REAL_TYPE) || 
				!(fType == INT_TYPE || fType == REAL_TYPE || fType == FP_INT_TYPE || fType == FP_REAL_TYPE)) {
				char *err =  malloc(sizeof(char) * 80);
				if(mulop == MULTIPLY)
					snprintf(err, sizeof(char) * 80, "SEM ERR: cannot multiply type %s to type %s\n", typeToString(termPIn), typeToString(fType));
				else 
					snprintf(err, sizeof(char) * 80, "SEM ERR: cannot divide type %s to type %s\n", typeToString(termPIn), typeToString(fType));
				if(fType != ERROR_TYPE && termPIn != ERROR_TYPE)
					appendError(err, currLine);
				generatedType = ERROR_TYPE;
			} 
			else {
				if(fType == REAL_TYPE || fType == FP_REAL_TYPE || termPIn == REAL_TYPE || termPIn == FP_REAL_TYPE)
					generatedType = REAL_TYPE; //any type real? total expr becomes real
				else
					generatedType = INT_TYPE; //else stays int
			}
		}
		if(mulop == DIV || mulop == MOD) {
			if(!(termPIn == INT_TYPE || termPIn == FP_INT_TYPE) || !(fType == INT_TYPE || fType == FP_INT_TYPE) ) {
				char *err =  malloc(sizeof(char) * 80);
				if(mulop == DIV)
					snprintf(err, sizeof(char) * 80, "SEM ERR: cannot div type %s to type %s\n", typeToString(termPIn), typeToString(fType));
				else 
					snprintf(err, sizeof(char) * 80, "SEM ERR: cannot mod type %s to type %s\n", typeToString(termPIn), typeToString(fType));
				if(fType != ERROR_TYPE && termPIn != ERROR_TYPE)
					appendError(err, currLine);
				generatedType = ERROR_TYPE;
			}
			else {
				generatedType = INT_TYPE;
			}
		}
		if(mulop == AND) {
			if(!(termPIn == BOOL_TYPE || termPIn == FP_BOOL_TYPE) || !(fType == BOOL_TYPE || fType == FP_BOOL_TYPE)) {
				char *err =  malloc(sizeof(char) * 80);
				snprintf(err, sizeof(char) * 80, "SEM ERR: cannot and type %s to type %s\n", typeToString(termPIn), typeToString(fType));
				if(fType != ERROR_TYPE && termPIn != ERROR_TYPE)
					appendError(err, currLine);
				generatedType = ERROR_TYPE;
			}
			else
				generatedType = BOOL_TYPE;
		}
		return termPrime(generatedType);
	}
	else {
		printSynerr("mulop, close paren, relop, semicolon, close bracket, then, do, addop, comma, else, end", "term'");
		synchType(NT_TERM);
		return ERROR_TYPE;
	}
}

int factor() {
	if(tok->tokenName == OPENPAREN) {
		if(!match(OPENPAREN)) {synchType(NT_FACTOR); return ERROR_TYPE;}
		int fval = expr();
		if(!match(CLOSEPAREN)) {synchType(NT_FACTOR); return ERROR_TYPE;}
		return fval;
	}
	else if(tok->tokenName == ID) {
		int currLine = tokenizingLine;
		char *lexeme = tok->attribute->attrString;
		int idType = matchId(NT_FACTOR);		
		if(idType == ERROR_TYPE) {
			char *err =  malloc(sizeof(char) * 80);
			strcpy(err, "SEM ERR: identifier '");
			strcat(err, lexeme);
			strcat(err, "' has not been declared\n");
			appendError(err, currLine);
		}
		return factorPrime(idType);
	}
	else if(tok->tokenName == NUM) {

		int fval = matchNum(NT_FACTOR);
		return fval;
		
	}
	else if(tok->attribute->attrInt == NOT) {
		if(!match(NOT)) {synchType(NT_FACTOR); return ERROR_TYPE;}
		int fval = factor();
		if(fval != BOOL_TYPE) {
			return ERROR_TYPE;
		}
		return fval;
	}
	else {
		printSynerr("open paren, id, num, not", "factor");
		synchType(NT_FACTOR);
		return ERROR_TYPE;
	}
	
}

int matchNum(int nt) {
	int ret = ERROR_TYPE;
	if(tok->tokenName == NUM) {
		ret = tok->subName;
		tok = getNextToken();
		if(ret == REAL)
			return REAL_TYPE;
		else if(ret == INTEGER)
			return INT_TYPE;
		return ret;
	}
	else {
		synchType(nt);
		ret = ERROR_TYPE;
	}
	return ret;
}

int matchId(int nt) {
	if(tok->tokenName == ID) {
		struct greenNode *currGreen = topGreen;
        while(currGreen != NULL) {
  			if((strcmp(currGreen->id, tok->attribute->attrString) == 0) && (currGreen->duplicate == 0)) {
                    tok = getNextToken();
                    return currGreen->type;
            }
            struct blueNode *currBlue = currGreen->firstBlue;
            //check matching blue
            while(currBlue != NULL) {
                if(strcmp(currBlue->id, tok->attribute->attrString) == 0) {
                    tok = getNextToken();
                    return currBlue->type;
                }
                currBlue = currBlue->next;
            }
            //no matching blue
            currGreen = currGreen->prev;
        }
		//did not find, throw error
		tok = getNextToken();
		return ERROR_TYPE;
	}
	else {
		synchType(nt);
		return ERROR_TYPE;
	}
}


int factorPrime(char *id, int type) {
	if(tok->tokenName == OPENPAREN) {
		int generatedType = ERROR_TYPE;
		if(!match(OPENPAREN)) {synchType(NT_FACTOR); return ERROR_TYPE;}
		struct intListNode *eRet = expLst(); 
		if(!match(CLOSEPAREN)) {synchType(NT_FACTOR); return ERROR_TYPE;}
		//todo get green node from id
		//todo match eRet types to greenNode.bluenodes.type
		return eType; 
	}
	else if(tok->tokenName == OPENBRACKET) {
		//factor->id[exp], if exp is an int and id is an array return type stored in array
		if(!match(OPENBRACKET)) {synchType(NT_FACTOR); return ERROR_TYPE;}
        int currLine = tokenizingLine;
		int eType = expr();
        
		if(!match(CLOSEBRACKET)) {synchType(NT_FACTOR); return ERROR_TYPE;}
		int fType = ERROR_TYPE;

		if(eType == INT_TYPE || eType == FP_INT_TYPE) {
			if(type == ARRAY_INT_TYPE || type == FP_ARRAY_INT_TYPE) 
				fType = INT_TYPE;
			else if(type == ARRAY_REAL_TYPE || type == FP_ARRAY_REAL_TYPE)
				fType = REAL_TYPE;
            else {
                //neither int or real array
                char *err =  malloc(sizeof(char) * 80);
                snprintf(err, sizeof(char) * 80, "SEM ERR type %s is not an array and is not indexed\n", typeToString(type));
                appendError(err, currLine);
				return ERROR_TYPE;
            }
		}
        else {
            //exp is not an int
            char *err =  malloc(sizeof(char) * 80);
            snprintf(err, sizeof(char) * 80, "SEM ERR arrays can only be accessed with ints you're using type %s\n", typeToString(eType));
            appendError(err, currLine);
			return ERROR_TYPE;
        }
		return fType;
	}
	else if(tok->tokenName == CLOSEPAREN || tok->attribute->attrInt == END || tok->tokenName == CLOSEBRACKET ||
		tok->attribute->attrInt == THEN || tok->attribute->attrInt == DO || tok->tokenName == ADDOP ||
		tok->tokenName == MULOP || tok->tokenName == COMMA || tok->tokenName == ELSE || tok->tokenName == RELOP || tok->tokenName == SEMICOLON) {
		return type;//epsilon
	}
	else {
		printSynerr("open paren, open bracket, close paren, end, close bracket, then, do, addop, mulop, comma, else, relop", "factor'");
		synchType(NT_FACTOR);
		return ERROR_TYPE;
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

void checkAddType(char *lexeme, int type) {
	//? do you need to add type in both places?

	//add type to symbol table
	struct node *matched = getSymbol(lexeme);
	matched->type = type;
	//add type in blue nodes
	if(topGreen != NULL) {
	    struct blueNode *currBlue = topGreen->firstBlue;
	    //check matching blue
	    while(currBlue != NULL) {
	        if(strcmp(currBlue->id, lexeme) == 0) {
	            currBlue->type = type;
	            break;
	        }
	        currBlue = currBlue->next;
	    }
	}

}

int match(int type) {
	if(type == ID || type == NUM || type == ASSIGNOP || type == OPENPAREN || type == CLOSEPAREN ||
		type == COMMA || type == SEMICOLON || type == COLON || type == PERIOD || type == ARRAYRANGE || 
		type == ENDOFFILE || type == RELOP || type == ADDOP || type == MULOP || type == OPENBRACKET || type == CLOSEBRACKET) {
		if(type == tok->tokenName && type != ENDOFFILE) {
			//printf("%s, ", tokenNameToString(type));
			tok = getNextToken();
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
			tok = getNextToken();
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
	//printf("syncing %d\n", syncingType);
	while(!synch(tok,syncingType)) {
		//printf("syncing %d\n", tok->tokenName);
		tok = getNextToken();
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
	char *err =  malloc(sizeof(char) * 80);
	strcpy(err, "SYNERR: expecting ");
	strcat(err, matchType);
	strcat(err, ", received ");
	strcat(err, tokStr);
	strcat(err, "\n");
	appendError(err, tokenizingLine);
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
	char *err =  malloc(sizeof(char) * 240);
	snprintf(err, sizeof(char) * 240, "SYNERR in %s, expecting %s, received %s\n", nonterminal, neededTypes, tokStr);
	appendError(err, tokenizingLine);
}

struct greenNode *popGreenNode() {
    if(topGreen == NULL) {
        return NULL;
    }
    else if(topGreen->prev == NULL) {
    	puts("just removed last thing from stack");
        struct greenNode *ret = topGreen;
        topGreen = NULL;
        return ret;
    }
    else {
        struct greenNode *ret = topGreen;
        topGreen = topGreen->prev;
        return ret;
    }
}

void pushGreenNode(struct greenNode *newGreen) {
    if(topGreen == NULL) 
        topGreen = newGreen;
    else {
        newGreen->prev = topGreen;
        topGreen = newGreen;
    }
}

int checkAddGreenNode(struct greenNode *newGreen) {
    if(topGreen == NULL) {
    	//no greens? don't need to check
    	printf("Adding green node id='%s'\n", newGreen->id);
        pushGreenNode(newGreen);
	}
    else {
        struct greenNode *currGreen = topGreen;
        //check matching green
        while(currGreen != NULL) {
            if(strcmp(currGreen->id, newGreen->id) == 0) {
            	newGreen->duplicate = 1;
				pushGreenNode(newGreen);
				printf("Adding green node id='%s'\n", newGreen->id);
                return 0;
            }
            
            struct blueNode *currBlue = currGreen->firstBlue;
            //check matching blue
            while(currBlue != NULL) {
                if(strcmp(currBlue->id, newGreen->id) == 0) {
                	newGreen->duplicate = 1;
            	 	pushGreenNode(newGreen);
    				printf("Adding green node id='%s'\n", newGreen->id);
                    return 0;
                }
                currBlue = currBlue->next;
            }
            //no matching blue
            currGreen = currGreen->prev;
        }
        //no matching blue or green, good to push on to stack
        pushGreenNode(newGreen);
        printf("Adding green node id='%s'\n", newGreen->id);
    }
    return 1;
}

int checkAddBlueNode(struct blueNode *newBlue) {

    //check matching green
    if(strcmp(topGreen->id, newBlue->id) == 0) {
        return 0;
    }

    struct blueNode *currBlue = topGreen->firstBlue;
    //check matching blue
    while(currBlue != NULL) {
        if(strcmp(currBlue->id, newBlue->id) == 0) {
            return 0;
        }
        currBlue = currBlue->next;
    }

    //no matching blue or green, good to add to linked list
   if(topGreen->firstBlue == NULL) {
       topGreen->firstBlue = newBlue;
       return 1;
   }
   struct blueNode *emptyBlue = topGreen->firstBlue;
   while(emptyBlue->next != NULL) {
       emptyBlue = emptyBlue->next;
   }
   emptyBlue->next = newBlue;
   printf("Adding blue node id='%s'\n", newBlue->id);
   return 1;
}

char * typeToString(int type) {
	switch(type) {
		case INT_TYPE:
		case INTEGER:
		case FP_INT_TYPE:
			return "int";
		case REAL_TYPE:
		case REAL:
		case FP_REAL_TYPE:
			return "real";
		case BOOL_TYPE:
		case FP_BOOL_TYPE:
			return "bool";
		case ARRAY_INT_TYPE:
		case FP_ARRAY_INT_TYPE:
			return "int array";
		case ARRAY_BOOL_TYPE:
			return "bool array";
		case ARRAY_REAL_TYPE:
		case FP_ARRAY_REAL_TYPE:
			return "real array";
		case ERROR_TYPE:
			return "error";
		case FUNCTION_TYPE:
			return "function";
		case PROGRAM_TYPE:
			return "program";
		default:
			return "undefined";
	}
}


void finish() {
	puts("program finished");
	printListing(listingFile);
}
