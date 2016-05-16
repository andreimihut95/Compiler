#ifndef _COMPILER_
#define _COMPILER_
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<ctype.h>
#include<string.h>
#include<conio.h>
#include<sys/stat.h>
#include<sys/types.h>
#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");
enum 
{
	INT = 0, CHAR = 1, FLOAT = 2, DOUBLE = 3, ENUM = 4, IF = 5, ELSE = 6, BREAK = 7, FOR = 8, STRUCT = 9, 
	VOID = 10, WHILE = 11, RETURN = 12, DO = 13, CONTINUE = 14, SWITCH = 15, CASE = 16, LONG = 17, ID = 18, 
	CT_INT = 19, CT_REAL = 20, CT_CHAR = 21, CT_STRING = 22, COMMA = 23, SEMICOLON = 24, LPAR = 25, RPAR = 26, 
	LBRACKET = 27, RBRACKET = 28, LACC = 29, RACC = 30, ADD = 31, SUB = 32, MUL = 33, DIV = 34, DOT = 35, AND = 36, 
    OR = 37, NOT = 38, ASSIGN = 39, EQUAL = 40, NOTEQ = 41, LESS = 42, LESSEQ = 43, GREATER = 44, GREATEREQ = 45, END = 46
};
enum {TB_INT , TB_DOUBLE , TB_CHAR , TB_STRUCT , TB_VOID};
enum {CLS_VAR = 0, CLS_FUNC = 1 , CLS_EXTFUNC = 2, CLS_STRUCT = 3};
enum { MEM_GLOBAL, MEM_ARG, MEM_LOCAL };
struct _Token;
typedef struct _Token Token;

void err(const char *fmt, ...);
void tkerr(const Token *tk, const char *fmt, ...);



struct _Type;
typedef struct _Type Type;

struct _Symbol;
typedef struct _Symbol Symbol;

typedef struct _Type
{
	int typeBase;
	Symbol *s;
	int nElements;
}Type;

typedef struct _Symbols
{
	Symbol **begin;
	Symbol **end;
	Symbol **after;
}Symbols;

typedef struct _Symbol
{
	const char *name; 
	int cls;
	int mem;
	Type type;
	int depth;
	union
	{
		Symbols *args;
		Symbols *members;
	};
}Symbol;

char name[][13] = { "INT" , "CHAR" , "FLOAT" , "DOUBLE" , "ENUM" , "IF" ,"ELSE" , "BREAK" , "FOR" ,"STRUCT" , "VOID" ,
					"WHILE" , "RETURN" , "DO" , "CONTINUE" , "SWITCH" , "CASE" , "LONG" , "ID" , "CT_INT" , "CT_REAL" , 
				    "CT_CHAR" , "CT_STRING" , "COMMA" , "SEMICOLON" , "LPAR" , "RPAR" , "LBRACKET" , "RBRACKET" , "LACC" , 
					"RACC" , "ADD" , "SUB" , "MUL" , "DIV" , "DOT" , "AND" , "OR" , "NOT" , "ASSIGN" , "EQUAL" , "NOTEQ" , 
					"LESS" , "LESSEQ" , "GREATER" , "GREATEREQ" , "END" };
int line = 1; // linia
FILE  *f; //fisier
char text[2048]; //unde salvam textul
char *pointer_urmator; //unde pointeaza
typedef struct _Token
{
	int code; // codul (numele)
	union
	{
		double r; // folosit pentru CT_REAL
		char *text; // folosit pentru ID, CT_STRING,CT_CHAR (alocat dinamic)
		long int i; // folosit pentru CT_INT
	};
	int line; // linia din fisierul de intrare
	struct _Token *next; // inlantuire la urmatorul AL
}Token;

int crtDepth;
Token *lastToken = NULL, *tokens = NULL, *consumedToken = NULL, *currentToken = NULL;//pointers inceput, final, consumat si curent
Symbols symbols;

int consume(int code)
{
	if (currentToken->code == code)
	{
		consumedToken = currentToken;
		currentToken = currentToken->next;
		return 1;
	}
	return 0;
}
void initSymbols(Symbols *symbols)
{
	symbols->begin = NULL;
	symbols->after = NULL;
	symbols->end = NULL;
}
Symbol *addSymbol(Symbols *symbols, const char *name, int cls)
{
	Symbol *s;
	if (symbols->end == symbols->after)
	{
		int count = symbols->after - symbols->begin;
		int n = count * 2;
		if (n == 0)	n = 1;
		symbols->begin = (Symbol**)realloc(symbols->begin, n*sizeof(Symbol*));
		if (symbols->begin == NULL)	err("Not enough memory");
		symbols->end = symbols->begin + count;
		symbols->after = symbols->begin + n;
	}
	SAFEALLOC(s, Symbol);
	*symbols->end++ = s;
	s->name = name;
	s->cls = cls;
	s->depth = crtDepth;
	return s;
}

Symbol *findSymbol(Symbols *symbols, const char *name)
{
	Symbol **p;
	if (symbols->begin == NULL)	return NULL;
	for(p = symbols->end - 1; p >= symbols->begin ; p--)
		if (strcmp(name, (*p)->name) == 0)
			return *p;
	return NULL;
}
void deleteSymbolsAfter(Symbols *symbols, Symbol *symbol)
{
	char *name = symbol->name;
	Symbol **p;
	Symbol **r;
	for (p = symbols->begin; p != symbols->end; p++)
	{
		if (strcmp(name, (*p) ->name) == 0)
		{
			for (r = p + 1; r != symbols->end; r++)
				free(r);
			symbols->end = p + 1;
			break;
		}
	}
}
void afisareSymbols(Symbols *symbols)
{
	Symbol **p = symbols->begin;
	if (p == NULL)
	{
		printf("Nu sunt simboluri in lista!\n");
		exit(0);
	}
	for (p = symbols->begin; p != symbols->end; p++)
		printf("%s ", (*p)->name);
	printf("\n");
}
void err(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, fmt, va);
	fputc('\n', stderr);
	va_end(va);
	exit(-1);
}

void tkerr(const Token *tk, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "Error in line %d: ", tk->line);
	vfprintf(stderr, fmt, va);
	fputc('\n', stderr);
	va_end(va);
	exit(-1);
}
Symbols symbols;
Symbol *currentStruct;
Symbol *currentFunction;
void        addVar(Token *tkName, Type *t)
{
	Symbol      *s;
	if (currentStruct) {
		if (findSymbol(&currentStruct->members, tkName->text))
			tkerr(currentToken, "symbol redefinition: %s", tkName->text);
		s = addSymbol(&currentStruct->members, tkName->text, CLS_VAR);
	}
	else if (currentFunction) {
		s = findSymbol(&symbols, tkName->text);
		if (s&&s->depth == crtDepth)
			tkerr(currentToken, "symbol redefinition: %s", tkName->text);
		s = addSymbol(&symbols, tkName->text, CLS_VAR);
		s->mem = MEM_LOCAL;
	}
	else {
		if (findSymbol(&symbols, tkName->text))
			tkerr(currentToken, "symbol redefinition: %s", tkName->text);
		s = addSymbol(&symbols, tkName->text, CLS_VAR);
		s->mem = MEM_GLOBAL;
	}
	s->type = *t;
}
#endif