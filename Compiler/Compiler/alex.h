#ifndef _ALEX_
#define _ALEX_
#include "compiler.h"

Token *addTk(int code)
{
	Token *tk;
	SAFEALLOC(tk, Token);
	tk->code = code;
	tk->line = line;
	tk->next = NULL;
	if (lastToken)
		lastToken->next = tk;
	else
		tokens = tk;
	lastToken = tk;
	return tk;
}

void showAtoms()
{
	Token *tk;
	for (tk = tokens; tk != NULL; tk = tk->next)
	{
		printf("%s(%d)", name[tk -> code] , tk->code);
		switch (tk->code)
		{
		case ID:
		case CT_STRING:
		case CT_CHAR: printf(":%s", tk->text); break;
		case CT_INT: printf(":%d", tk->i); break;
		case CT_REAL: printf(":%f", tk->r); break;
		}
		printf("\n");
	}
}
char *createString(const char *start, int lungime)
{
	int i;
	char *sir, *aux;
	char c;
	int poz = 0;
	sir = (char *)malloc(lungime * sizeof(char));
	if (sir == NULL)
		err(sir, "");
	for (i = 0; i < lungime; i++)
		*(sir + i) = *(start + i);
	*(sir + lungime) = '\0';
	aux = sir;
	c = *aux;
	while (c != '\0')
	{
		if (c == '\\')
		{
			aux++;
			poz++;
			c = *(aux);
			if (c == 'n')
			{
				*(sir + poz - 1) = '\n';
				strcpy(aux, aux + 1);
				aux--;
				poz--;
			}
			else if (c == 't')
			{
				*(sir + poz - 1) = '\t';
				strcpy(aux, aux + 1);
				aux--;
				poz--;
			}
			else if (c == 'r')
			{
				*(sir + poz - 1) = '\r';
				strcpy(aux, aux + 1);
				aux--;
				poz--;
			}
			else if (c == '0')
			{
				*(sir + poz - 1) = '\0';
				strcpy(aux, aux + 1);
				aux--;
				poz--;
			}
			else if (c == 'a')
			{
				*(sir + poz - 1) = '\a';
				strcpy(aux, aux + 1);
				aux--;
				poz--;
			}
			else if (c == 'b')
			{
				*(sir + poz - 1) = '\b';
				strcpy(aux, aux + 1);
				aux--;
				poz--;
			}
			else if (c == 'f')
			{
				*(sir + poz - 1) = '\f';
				strcpy(aux, aux + 1);
				aux--;
				poz--;
			}
			else if (c == 'r')
			{
				*(sir + poz - 1) = '\r';
				strcpy(aux, aux + 1);
				aux--;
				poz--;
			}
			else if (c == '\\')
			{
				*(sir + poz - 1) = '\\';
				strcpy(aux, aux + 1);
				aux--;
				poz--;
			}
			else if (c == '\?')
			{
				*(sir + poz - 1) = '\?';
				strcpy(aux, aux + 1);
				aux--;
				poz--;
			}
			else if (c == '\"')
			{
				*(sir + poz - 1) = '\"';
				strcpy(aux, aux + 1);
				aux--;
				poz--;
			}
		}
		aux++;
		c = *(aux);
		poz++;
	}
	return sir;

}
int getNextToken()
{
	int state = 0, lungime;
	char ch;
	const char *pointer_inceput;
	Token *tk;
	SAFEALLOC(tk, Token);
	char number[30];
	int nr = 0;
	int base;
	while (1)
	{

		ch = *pointer_urmator;
		switch (state)
		{
		case 0:
			if (ch >= '1' && ch <= '9')
			{
				pointer_urmator++;
				state = 1;
				number[nr++] = ch;
				base = 10;
			}
			else if (ch == '0')
			{
				pointer_urmator++;
				state = 2;
			}
			else if (isalpha(ch) || ch == '_')
			{
				pointer_inceput = pointer_urmator;
				pointer_urmator++;
				state = 13;
			}
			else if (ch == '\'')
			{

				pointer_urmator++;
				state = 15;
			}
			else if (ch == '"')
			{
				pointer_urmator++;
				pointer_inceput = pointer_urmator;
				state = 19;
			}
			else if (ch == '+')
			{
				pointer_urmator++;
				state = 23;
			}
			else if (ch == '-')
			{
				pointer_urmator++;
				state = 24;
			}
			else if (ch == '*')
			{
				pointer_urmator++;
				state = 25;
			}
			else if (ch == '.')
			{
				pointer_urmator++;
				state = 26;
			}
			else if (ch == '&')
			{
				pointer_urmator++;
				state = 27;
			}
			else if (ch == '|')
			{
				pointer_urmator++;
				state = 29;
			}
			else if (ch == '!')
			{
				pointer_urmator++;
				state = 31;
			}
			else if (ch == '=')
			{
				pointer_urmator++;
				state = 34;
			}
			else if (ch == '<')
			{
				pointer_urmator++;
				state = 37;
			}
			else if (ch == '>')
			{
				pointer_urmator++;
				state = 40;
			}
			else if (ch == ',')
			{
				pointer_urmator++;
				state = 43;
			}
			else if (ch == ';')
			{
				pointer_urmator++;
				state = 44;
			}
			else if (ch == '(')
			{
				pointer_urmator++;
				state = 45;
			}
			else if (ch == ')')
			{
				pointer_urmator++;
				state = 46;
			}
			else if (ch == '[')
			{
				pointer_urmator++;
				state = 47;
			}
			else if (ch == ']')
			{
				pointer_urmator++;
				state = 48;
			}
			else if (ch == '{')
			{
				pointer_urmator++;
				state = 49;
			}
			else if (ch == '}')
			{
				pointer_urmator++;
				state = 50;
			}
			else if (ch == '/')
			{
				pointer_urmator++;
				state = 51;
			}
			else if (ch == ' ' || ch == '\r' || ch == '\t')
			{
				pointer_urmator++;
			}
			else if (ch == '\n')
			{
				line++;
				pointer_urmator++;
			}
			else if (ch == '\0')
			{
				addTk(END);
				return END;
			}
			else
			{
				tkerr(addTk(END), "Caracter gresit!");
			}
			break;
		case 1:
			if (ch >= '0' && ch <= '9')
			{
				pointer_urmator++;
				number[nr++] = ch;
			}
			else if (ch == '.')
			{
				number[nr++] = ch;
				pointer_urmator++;
				state = 7;
			}
			else if (ch == 'E' || ch == 'e')
			{
				number[nr++] = ch;
				pointer_urmator++;
				state = 9;
			}
			else
				state = 6;
			break;
		case 2:
			base = 8;
			if (ch >= '0' && ch <= '7')
			{
				number[nr++] = ch;
				pointer_urmator++;
				state = 3;
			}
			else if (ch == 'x')
			{
				pointer_urmator++;
				state = 4;
			}
			else if (ch == '.')
			{
				number[nr++] = ch;
				pointer_urmator++;
				state = 7;
			}
			else if (ch == 'e' || ch == 'E')
			{
				number[nr++] = ch;
				pointer_urmator++;
				state = 9;
			}
			else
				state = 6;
			break;
		case 3:
			if (ch >= '0' && ch <= '7')
			{
				pointer_urmator++;
				number[nr++] = ch;

			}
			else
				state = 6;
			break;
		case 4:
			base = 16;
			if (ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D' || ch == 'E' || ch == 'F' || ch == 'a' || ch == 'b' || ch == 'c' || ch == 'd' || ch == 'e' || ch == 'f' || ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9')
			{
				number[nr++] = ch;
				pointer_urmator++;
				state = 5;
			}
			else
				tkerr(addTk(END), "Caracter hexa invalid!");
			break;
		case 5:
			if (ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D' || ch == 'E' || ch == 'F' || ch == 'a' || ch == 'b' || ch == 'c' || ch == 'd' || ch == 'e' || ch == 'f' || ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9')
			{
				pointer_urmator++;
				number[nr++] = ch;
			}
			else
				state = 6;
			break;
		case 6:
			tk = addTk(CT_INT);
			number[nr] = '\0';
			tk->i = strtol(number, NULL, base);
			return tk->code;

		case 7:
			if (ch >= '0' && ch <= '9')
			{
				number[nr++] = ch;
				pointer_urmator++;
				state = 8;
			}
			else
				tkerr(addTk(END), "Trebuie cifra dupa punct!");
			break;
		case 8:
			if (ch >= '0' && ch <= '9')
			{
				number[nr++] = ch;
				pointer_urmator++;
			}
			else if (ch == 'e' || ch == 'E')
			{
				number[nr++] = ch;
				pointer_urmator++;
				state = 9;
			}
			else
				state = 12;
			break;
		case 9:
			if (ch == '-' || ch == '+')
			{
				number[nr++] = ch;
				pointer_urmator++;
				state = 10;
			}
			else
				state = 10;
			break;
		case 10:
			if (ch >= '0' && ch <= '9')
			{
				number[nr++] = ch;
				pointer_urmator++;
				state = 11;
			}
			else
				tkerr(addTk(END), "Trebuie sa fie cifra dupa +, - sau E, e!");
			break;
		case 11:
			if (ch >= '0' && ch <= '9')
			{
				number[nr++] = ch;
				pointer_urmator++;
			}
			else
				state = 12;
			break;
		case 12:
			tk = addTk(CT_REAL);
			number[nr] = '\0';
			tk->r = strtod(number, NULL);
			return CT_REAL;
		case 13:
			if (isalnum(ch) || ch == '_')
				pointer_urmator++;
			else
				state = 14;
			break;
		case 14:
			lungime = pointer_urmator - pointer_inceput;
			if (lungime == 3 && !memcmp(pointer_inceput, "int", 3))
				tk = addTk(INT);
			else if (lungime == 5 && !memcmp(pointer_inceput, "float", 5))
				tk = addTk(FLOAT);
			else if (lungime == 2 && !memcmp(pointer_inceput, "do", 2))
				tk = addTk(DO);
			else if (lungime == 8 && !memcmp(pointer_inceput, "continue", 8))
				tk = addTk(CONTINUE);
			else if (lungime == 6 && !memcmp(pointer_inceput, "switch", 6))
				tk = addTk(SWITCH);
			else if (lungime == 4 && !memcmp(pointer_inceput, "case", 4))
				tk = addTk(CASE);
			else if (lungime == 5 && !memcmp(pointer_inceput, "long", 5))
				tk = addTk(LONG);
			else if (lungime == 4 && !memcmp(pointer_inceput, "enum", 4))
				tk = addTk(ENUM);
			else if (lungime == 4 && !memcmp(pointer_inceput, "char", 4))
				tk = addTk(CHAR);
			else if (lungime == 6 && !memcmp(pointer_inceput, "double", 6))
				tk = addTk(DOUBLE);
			else if (lungime == 4 && !memcmp(pointer_inceput, "else", 4))
				tk = addTk(ELSE);
			else if (lungime == 3 && !memcmp(pointer_inceput, "for", 3))
				tk = addTk(FOR);
			else if (lungime == 2 && !memcmp(pointer_inceput, "if", 2))
				tk = addTk(IF);
			else if (lungime == 6 && !memcmp(pointer_inceput, "return", 6))
				tk = addTk(RETURN);
			else if (lungime == 6 && !memcmp(pointer_inceput, "struct", 6))
				tk = addTk(STRUCT);
			else if (lungime == 4 && !memcmp(pointer_inceput, "void", 4))
				tk = addTk(VOID);
			else if (lungime == 5 && !memcmp(pointer_inceput, "while", 5))
				tk = addTk(WHILE);
			else if (lungime == 5 && !memcmp(pointer_inceput, "break", 5))
				tk = addTk(BREAK);
			else
			{
				int lungime = pointer_urmator - pointer_inceput;
				tk = addTk(ID);
				tk->text = createString(pointer_inceput, lungime);
			}
			return tk->code;
		case 15:
			if (ch != '\'' && ch != '\\')
			{
				pointer_inceput = pointer_urmator;
				pointer_urmator++;
				state = 16;
			}
			else if (ch == '\\')
			{
				pointer_inceput = pointer_urmator;
				pointer_urmator++;
				state = 17;
			}
			else
				if (ch == '\0' || ch == '\'')
					tkerr(addTk(END), "Eroare la apostrof!");
			break;
		case 16:
			if (ch == '\'')
			{
				pointer_urmator++;
				state = 18;
			}
			else
				tkerr(addTk(END), "Eroare la apostrof!");
			break;
		case 17:
			if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v' || ch == '\'' || ch == '?' || ch == '"' || ch == '\\' || ch == '0')
			{
				pointer_urmator++;
				state = 16;
			}
			else
				tkerr(addTk(END), "Dupa backslash poate urma a b f n r t v ' ? \" \\ 0");
			break;
		case 18:
			lungime = pointer_urmator - pointer_inceput;
			tk = addTk(CT_CHAR);
			tk->text = createString(pointer_inceput, lungime - 1);
			return tk->code;
		case 19:
			if (ch != '"' && ch != '\\' && ch != '\0')
			{
				pointer_urmator++;
				state = 21;
			}
			else if (ch == '\\')
			{
				pointer_urmator++;
				state = 20;
			}
			else if (ch == '\0')
				tkerr(addTk(END), "Nu s-au inchis ghilimelele!");
			else
				state = 21;
			break;
		case 20:
			if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v' || ch == '\'' || ch == '?' || ch == '"' || ch == '\\' || ch == '0')
			{
				pointer_urmator++;
				state = 21;
			}
			else
				tkerr(addTk(END), "Dupa backslash apare doar a b f n r t v \' \" \\ 0");
			break;
		case 21:
			if (ch == '"')
			{
				pointer_urmator++;
				state = 22;
			}
			else
				state = 19;
			break;
		case 22:
			lungime = pointer_urmator - pointer_inceput;
			tk = addTk(CT_STRING);
			tk->text = createString(pointer_inceput, lungime - 1);
			return CT_STRING;
		case 23:
			addTk(ADD);
			return ADD;
		case 24:
			addTk(SUB);
			return SUB;
		case 25:
			addTk(MUL);
			return MUL;
		case 26:
			addTk(DOT);
			return DOT;
		case 27:
			if (ch == '&')
			{
				pointer_urmator++;
				state = 28;
			}
			else
				tkerr(addTk(END), "Dupa & poate urma doar &");
			break;
		case 28:
			addTk(AND);
			return AND;
		case 29:
			if (ch == '|')
			{
				pointer_urmator++;
				state = 30;
			}
			else
				tkerr(addTk(END), "Dupa | poate urma doar |");
			break;
		case 30:
			addTk(OR);
			return OR;
		case 31:
			if (ch == '=')
			{
				pointer_urmator++;
				state = 33;
			}
			else
				state = 32;
			break;
		case 32:
			addTk(NOT);
			return(NOT);
		case 33:
			addTk(NOTEQ);
			return NOTEQ;
		case 34:
			if (ch == '=')
			{
				pointer_urmator++;
				state = 36;
			}
			else
				state = 35;
			break;
		case 35:
			addTk(ASSIGN);
			return ASSIGN;
		case 36:
			addTk(EQUAL);
			return EQUAL;
		case 37:
			if (ch == '=')
			{
				pointer_urmator++;
				state = 39;
			}
			else
				state = 38;
			break;
		case 38:
			addTk(LESS);
			return LESS;
		case 39:
			addTk(LESSEQ);
			return LESSEQ;
		case 40:
			if (ch == '=')
			{
				pointer_urmator++;
				state = 42;
			}
			else
				state = 41;
			break;
		case 41:
			addTk(GREATER);
			return GREATER;
		case 42:
			addTk(GREATEREQ);
			return GREATEREQ;
		case 43:
			addTk(COMMA);
			return COMMA;
		case 44:
			addTk(SEMICOLON);
			return SEMICOLON;
		case 45:
			addTk(LPAR);
			return LPAR;
		case 46:
			addTk(RPAR);
			return RPAR;
		case 47:
			addTk(LBRACKET);
			return LBRACKET;
		case 48:
			addTk(RBRACKET);
			return RBRACKET;
		case 49:
			addTk(LACC);
			return LACC;
		case 50:
			addTk(RACC);
			return RACC;
		case 51:
			if (ch == '/')
			{
				pointer_urmator++;
				state = 52;
			}
			else if (ch == '*')
			{
				pointer_urmator++;
				state = 54;
			}
			else
				state = 53;
			break;
		case 52:
			if (ch != '\n' && ch != '\r' && ch != '\0')
			{
				pointer_urmator++;
			}
			else
				state = 0;
			break;
		case 53:
			addTk(DIV);
			return DIV;
		case 54:
			if (ch != '*' &&  ch != '\0')
			{
				pointer_urmator++;
			}
			else if (ch == '*')
			{
				pointer_urmator++;
				state = 55;
			}
			else
				if (ch == '\0')
					tkerr(addTk(END), "N-ai inchis comentariul!");
			break;
		case 55:
			if (ch == '*')
			{
				pointer_urmator++;
			}
			else if (ch == '/')
			{
				pointer_urmator++;
				state = 0;
			}
			else if (ch != '*' && ch != '\\' && ch != '\0')
			{
				pointer_urmator++;
				state = 54;
			}
			else if (ch == '\0')
				tkerr(addTk(END), "N-ai inchis comentariul!");
			break;
		}
	}
}
void terminare()
{
	free(lastToken);
	free(tokens);
}
#endif