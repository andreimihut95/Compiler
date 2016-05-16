#ifndef _ASIN_
#define _ASIN_
#include "compiler.h"

int unit()
{
	while (1)
	{
		if (declStruct()) 
		{}
		else if (declFunc())
		{}
		else if (declVar()) 
		{}
		else
			break;
	}
	if (consume(END))
	{
		printf("-------------Nicio eroare de sintaxa!-------------\n");
		return 1;
	}
	tkerr(currentToken, "-------------Declarare gresita structura/functie/variabila!-------------\n");
}

int declStruct()
{
	Token *tkName;
	Token *startToken = currentToken;
	if (consume(STRUCT))
	{
		tkName = currentToken;
		if (consume(ID))
		{
			if (consume(LACC))
			{
				if (findSymbol(&symbols, tkName->text))
					tkerr(currentToken, "Symbol redefinition: %s", tkName->text);
				currentStruct = addSymbol(&symbols, tkName->text, CLS_STRUCT);
				initSymbols(&currentStruct->members);
				while (1)
				{
					if (declVar()) 
					{}
					else 
						break;
				}
				if (consume(RACC)) 
				{
					if (consume(SEMICOLON))
					{
						currentStruct = NULL;
						return 1;
					}
					else
						tkerr(currentToken, "-------------Lipseste ;(SEMICOLON) dupa }(RACC)-------------\n");
				}
				else
					tkerr(currentToken, "-------------Lipseste }(RACC)-------------\n");
			}
			else
			{
				currentToken = startToken;
				return 0;
			}
		}
		else
			tkerr(currentToken, "-------------Lipseste ID dupa struct-------------\n");
	}
	currentToken = startToken;
	return 0;
}

int declVar()
{
	Token *startToken = currentToken;
	Token *tkname;
	Type t;
	if (typeBase(&t))
	{
		tkname = currentToken;
		if (consume(ID))
		{
			if(arrayDecl(&t))
			{}
			else 
				t.nElements = -1;
			addVar(tkname, &t);
			while (1)
			{
				if (consume(COMMA))
				{
					tkname = currentToken;
					if (consume(ID))
					{
						if (arrayDecl(&t))
						{
						}
						else
							t.nElements = -1;
						addVar(tkname, &t);
					}
					else
						tkerr(currentToken, "-------------Lipseste ID(sau ID[parametru]) dupa ,(COMMA)-------------\n");
				}
				else break;
			}
			if (consume(SEMICOLON))
			{
				return 1;
			}
			else
				tkerr(currentToken, "-------------Lipseste ;(SEMICOLON) dupa ID(sau ID[parametru])-------------\n");
		}
		else
			tkerr(currentToken, "-------------Lipseste ID dupa typeBase(int,char,...)-------------\n");
	}
	currentToken = startToken;
	return 0;
}

int typeBase(Type *ret)
{
	Token *startToken = currentToken;
	Token *tkName;
	if (consume(INT))
	{
		ret->typeBase = TB_INT;
		return 1;
	}
	if (consume(DOUBLE))
	{
		ret->typeBase = TB_DOUBLE;
		return 1;
	}
	if (consume(CHAR))
	{
		ret->typeBase = TB_CHAR;
		return 1;
	}
	if (consume(STRUCT))
	{
		tkName = currentToken;
		if (consume(ID))
		{
			Symbol      *s = findSymbol(&symbols, tkName->text);
			if (s == NULL)	tkerr(currentToken, "Undefined symbol: %s", tkName->text);
			if (s->cls != CLS_STRUCT)tkerr(currentToken, "%s is not a struct", tkName->text);
			ret->typeBase = TB_STRUCT;
			ret->s = s;
			return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste  ID dupa struct-------------\n");
	}
	currentToken = startToken;
	return 0;
}

int arrayDecl(Type *ret)
{
	Token *startToken = currentToken;
	if (consume(LBRACKET))
	{
		expr();
		ret->nElements = 0;
		if (consume(RBRACKET))
		{
			return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste ](RBRACKET) dupa expresie-------------\n");
	}
	currentToken = startToken;
	return 0;

}

int typeName(Type *ret)
{
	if (typeBase(ret))
	{
		if (arrayDecl(ret))
		{

		}
		else
			ret->nElements = -1;
		return 1;
	}
	return 0;
}

int declFuncHeader(Type *t)
{
	Token *startToken = currentToken;
	Token *tkName;
	tkName = currentToken;
	if (consume(ID))
	{
		if (consume(LPAR))
		{
			if (findSymbol(&symbols, tkName->text))
				tkerr(currentToken, "Symbol redefinition: %s", tkName->text);
			currentFunction = addSymbol(&symbols, tkName->text, CLS_FUNC);
			initSymbols(&currentFunction->args);
			currentFunction->type = *t;
			crtDepth++;
			if (funcArg())
			{
				while (1)
				{
					if (consume(COMMA))
					{
						if (funcArg())
						{
						}
						else
							tkerr(currentToken, "-------------Lipseste argument de functie dupa ,(COMMA)!-------------\n");
					}
					else
						break;
				}
			}
			if (consume(RPAR))
			{
				crtDepth--;
				if (stmCompound())
				{
					deleteSymbolsAfter(&symbols, currentFunction);
					currentFunction = NULL;
					return 1;
				}
				else
					tkerr(currentToken, "-------------Lipseste {(LACC) dupa )(RPAR)-------------\n");
			}
			else
				tkerr(currentToken, "------------- Lipseste )(RPAR) dupa argument de functie-------------\n");
		}
		else
		{
			currentToken = startToken;
			return 0;
		}
	}
	else
		tkerr(currentToken, "Missing ID");

}

int declFunc()
{
	Token *startToken = currentToken;
	Type t;
	if (typeBase(&t))
	{
		if (consume(MUL))
		{
			t.nElements = 0;
		}
		else
			t.nElements = -1;

		if (declFuncHeader(&t))
		{
			return 1;
		}
	}
	if (consume(VOID))
	{
		t.typeBase = TB_VOID;
		if (declFuncHeader(&t))
		{
			return 1;
		}
	}
	currentToken = startToken;
	return 0;
}

int funcArg()
{
	Token * tkName;
	Type t;
	if (typeBase(&t))
	{
		tkName = currentToken;
		if (consume(ID))
		{
			if (arrayDecl(&t))
			{

			}
			else
				t.nElements = -1;
			Symbol  *s = addSymbol(&symbols, tkName->text, CLS_VAR);
			s->mem = MEM_ARG;
			s->type = t;
			s = addSymbol(&currentFunction->args, tkName->text, CLS_VAR);
			s->mem = MEM_ARG;
			s->type = t;
			return 1;
		}
		else tkerr(currentToken, "-------------Lipseste ID(sau ID[parametru]) dupa typeBase(int,char,...)-------------\n");
	}
	return 0;

}

int stmCompound()
{
	Token *startToken = currentToken;
	Symbol *start = symbols.end[-1];
	if (consume(LACC))
	{ 
		crtDepth++;
		while (1)
		{
			if (declVar())
			{}
			else if (stm())
			{}
			else
				break;
		}
		if (consume(RACC))
		{
			crtDepth--;
			deleteSymbolsAfter(&symbols, start);
			return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste }(RACC) dupa {(LACC), declarari variabile sau statement-------------\n");
	}
	currentToken = startToken;
	return 0;

}

int stm()
{
	Token *startToken = currentToken;
	if (stmCompound())
	{
		return 1;
	}
	if (consume(IF))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				if (consume(RPAR))
				{
					if (stm())
					{
						if (consume(ELSE))
						{
							if (stm())
							{
								return 1;
							}
							else
								tkerr(currentToken, "-------------Lipseste statement dupa else-------------\n");
						}
						return 1;
					}
					else
						tkerr(currentToken, "-------------Lipseste {(LACC) dupa )(RPAR)-------------\n");
				}
				else
					tkerr(currentToken, "-------------Lipseste )(RPAR) dupa expresie-------------\n");
			}
			else
				tkerr(currentToken, "-------------Lipseste expresia din if-------------\n");
		}
		else
			tkerr(currentToken, "-------------Lipseste ( (LPAR) dupa if-------------\n");

	}
	if (consume(WHILE))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				if (consume(RPAR))
				{
					if (stm())
					{
						return 1;
					}
					else
						tkerr(currentToken, "-------------Lipseste {(LACC) dupa )(RPAR)-------------\n");
				}
				else
					tkerr(currentToken, "-------------Lipseste )(RPAR) dupa expresie-------------\n");
			}
			else
				tkerr(currentToken, "-------------Lipseste expresie dupa ( (LPAR)-------------\n");
		}
		else
			tkerr(currentToken, "------------- Lipseste ( (LPAR) dupa while-------------\n");
	}

	if (consume(FOR))
	{
		if (consume(LPAR))
		{
			expr();
			if (consume(SEMICOLON))
			{
				expr();
				if (consume(SEMICOLON))
				{
					expr();
					if (consume(RPAR))
					{
						if (stm())
						{
							return 1;
						}
						else
							tkerr(currentToken, "-------------Lipseste {(LACC) dupa )(RPAR)-------------\n");
					}
					else
						tkerr(currentToken, "-------------Lipseste )(RPAR) dupa expresie sau ;(SEMICOLON)-------------\n");
				}
				else
					tkerr(currentToken, "-------------Lipseste ;(SEMICOLON) dupa expresie sau ;(SEMICOLON)-------------\n");
			}
			else
				tkerr(currentToken, "-------------Lipseste ;(SEMICOLON) dupa expresie sau ( (LPAR)\n");
		}
		else
			tkerr(currentToken, "-------------Lipseste ( (LPAR) dupa for-------------\n");
	}

	if (consume(BREAK))
	{
		if (consume(SEMICOLON))
		{
			return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste ;(SEMICOLON) dupa break\n");
	}

	if (consume(RETURN))
	{
		expr();
		if (consume(SEMICOLON))
		{
			return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste ;(SEMICOLON) dupa expresie sau return\n");
	}

	if (expr())
	{
		if (consume(SEMICOLON))
		{
			return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste ;(SEMICOLON) dupa expresie-------------\n");
	}

	if (consume(SEMICOLON))
		return 1;
	currentToken = startToken;
	return 0;
}


int expr()
{
	if (exprAssign())
		return 1;
	return 0;
}

int exprAssign()
{
	if (exprOr())
	{
		exprAssign1();
		return 1;
	}
	return 0;
}
int exprAssign1()
{
	Token *startToken = currentToken;
	if (consume(ASSIGN))
	{
		if (exprOr())
		{
			if (exprAssign1())
				return 1;
		}
	}
	currentToken = startToken;
	return 1;
}
int exprOr()
{
	if (exprAnd())
	{
		exprOr1();
		return 1;
	}
	return 0;
}

int exprOr1()
{
	Token *startToken = currentToken;
	if (consume(OR))
	{
		if (exprAnd())
		{
			if (exprOr1())
			{
				return 1;
			}
		}
		else
			tkerr(currentToken, "-------------Lipseste expresieAND dupa ||(OR)-------------\n");
	}
	currentToken = startToken;
	return 1;
}

int exprAnd()
{
	if (exprEq())
	{
		exprAnd1();
		return 1;
	}
	return 0;
}

int exprAnd1()
{
	Token *startToken = currentToken;
	if (consume(AND))
	{
		if (exprEq())
		{
			if (exprAnd1())
			{
				return 1;
			}
		}
		else
			tkerr(currentToken, "-------------Lipseste exprEQ dupa &&(AND)-------------\n");
	}
	currentToken = startToken;
	return 1;
}


int exprEq()
{
	if (exprRel())
	{
		exprEq1();
		return 1;
	}
	return 0;
}

int exprEq1()
{
	Token *startToken = currentToken;
	if (consume(EQUAL) || consume(NOTEQ))
	{
		if (exprRel())
		{
			if (exprEq1())
				return 1;
		}
		else tkerr(currentToken, "-------------Lipseste exprREL dupa ==(EQUAL) sau !=(NOTEQ)-------------\n");
	}
	currentToken = startToken;
	return 1;

}

int exprRel()
{
	if (exprAdd())
	{
		exprRel1();
		return 1;
	}
	return 0;
}

int exprRel1()
{
	Token *startToken = currentToken;
	if (consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
	{
		if (exprAdd())
		{
			if (exprRel1())
				return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste exprADD dupa LESS(<), LESSEQ(<=), GREATER(>) sau GREATEREQ(>=)-------------\n");
	}
	currentToken = startToken;
	return 1;
}

int exprAdd()
{
	if (exprMul())
	{
		exprAdd1();
		return 1;
	}
	return 0;
}

int exprAdd1()
{
	Token *startToken = currentToken;
	if (consume(ADD) || consume(SUB))
	{
		if (exprMul())
		{
			if (exprAdd1())
				return 1;
		}
		else tkerr(currentToken, "-------------Lipseste exprMUL dupa +(ADD) sau -(SUB)-------------\n");
	}
	currentToken = startToken;
	return 1;
}

int exprMul()
{
	if (exprCast())
	{
		exprMul1();
		return 1;
	}
	return 0;
}

int exprMul1()
{
	Token *startToken = currentToken;
	if (consume(MUL) || consume(DIV))
	{
		if (exprCast())
		{
			if (exprMul1())
				return 1;
		}
		else 
			tkerr(currentToken, "-------------Lipseste exprCAST dupa *(MUL) sau /(DIV)-------------\n");
	}
	currentToken = startToken;
	return 1;
}

int exprCast()
{
	Token *startToken = currentToken;
	Type t;
	if (consume(LPAR))
	{
		if (typeName(&t))
		{
			if (consume(RPAR))
			{
				if (exprCast())
				{
					return 1;
				}
				else
					tkerr(currentToken, "-------------Lipseste exprCAST dupa RPAR-------------\n");
			}
			else
				tkerr(currentToken, "-------------Lipseste )(RPAR) dupa typeName-------------\n");
		}
		else
			tkerr(currentToken, "-------------Lipseste typeName dupa ( (LPAR)-------------\n");
	}
	if (exprUnary())
		return 1;
	currentToken = startToken;
	return 0;
}

int exprUnary()
{
	Token *startToken = currentToken;
	if (consume(NOT) || consume(SUB))
	{
		if (exprUnary())
		{
			return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste exprUNARY dupa !(NOT) sau -(SUBSTRACT)-------------\n");
	}
	if (exprPostfix())
		return 1;
	currentToken = startToken;
	return 0;
}

int exprPostfix()
{
	if (exprPrimary())
	{
		exprPostfix1();
		return 1;
	}
	return 0;
}

int exprPostfix1()
{
	Token *startToken = currentToken;
	if (consume(LBRACKET))
	{
		if (expr())
		{
			if (consume(RBRACKET))
			{
				if (exprPostfix1())
				{
					return 1;
				}
			}
			else
				tkerr(currentToken, "-------------Lipseste ](RBRACKET) dupa expresie-------------\n");
		}
		else
			tkerr(currentToken, "-------------Lipseste expresie dupa [(LBRACKET)-------------\n");
	}
	if (consume(DOT))
	{
		if (consume(ID))
		{
			if (exprPostfix1())
			{
				return 1;
			}
		}
		else
			tkerr(currentToken, "-------------Lipseste ID dupa .(DOT)-------------\n");
	}
	currentToken = startToken;
	return 1;
}

int exprPrimary()
{
	Token *startToken = currentToken;
	if (consume(ID))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				while (1)
				{
					if (consume(COMMA))
					{
						if (expr())
						{
						}
						else
							tkerr(currentToken, "-------------Lipseste expresie dupa ,(COMMA)-------------\n");
					}
					else break;
				}

			}
			if (consume(RPAR))
			{
				return 1;
			}
			else
				tkerr(currentToken, "-------------Lipseste )(RPAR) dupa expresie-------------\n");
		}
		return 1;
	}

	if (consume(CT_INT) || consume(CT_REAL) || consume(CT_CHAR) || consume(CT_STRING))
	{
		return 1;
	}
	if (consume(LPAR))
	{
		if (expr())
		{
			if (consume(RPAR))
			{
				return 1;
			}
			else
				tkerr(currentToken, "-------------Lipseste )(RPAR) dupa expresie-------------\n");
		}
		tkerr(currentToken, "-------------Lipseste expresie dupa ( (LPAR)-------------\n");
	}
	currentToken = startToken;
	return 0;
}

#endif