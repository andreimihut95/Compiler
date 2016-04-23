#ifndef _ASIN_
#define _ASIN_
#include "compiler.h"

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

	Token *startToken = currentToken;
	if (consume(STRUCT))
	{
		if (consume(ID))
		{
			if (consume(LACC))
			{
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
	if (typeBase())
	{
		if (consume(ID))
		{
			arrayDecl();
			while (1)
			{
				if (consume(COMMA))
				{
					if (consume(ID))
					{
						arrayDecl();
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

int typeBase()
{

	Token *startToken = currentToken;
	if (consume(INT) || consume(DOUBLE) || consume(CHAR))
	{
		return 1;
	}
	if (consume(STRUCT))
	{
		if (consume(ID))
		{
			return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste  ID dupa struct-------------\n");
	}
	currentToken = startToken;
	return 0;
}

int arrayDecl()
{
	Token *startToken = currentToken;
	if (consume(LBRACKET))
	{
		expr();
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

int typeName()
{
	if (typeBase())
	{
		arrayDecl();
		return 1;
	}
	return 0;
}

int declFuncHeader()
{
	Token *startToken = currentToken;

	if (consume(ID))
	{
		if (consume(LPAR))
		{
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
				if (stmCompound())
				{
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
	if (typeBase())
	{
		consume(MUL);
		if (declFuncHeader())
		{
			return 1;
		}
	}
	if (consume(VOID))
	{
		if (declFuncHeader())
		{
			return 1;
		}
	}
	currentToken = startToken;
	return 0;
}

int funcArg()
{
	if (typeBase())
	{
		if (consume(ID))
		{
			arrayDecl();
			return 1;
		}
		else tkerr(currentToken, "-------------Lipseste ID(sau ID[parametru]) dupa typeBase(int,char,...)-------------\n");
	}
	return 0;

}

int stmCompound()
{
	Token *startToken = currentToken;
	if (consume(LACC))
	{ 
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
			return 1;
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
	if (consume(LPAR))
	{
		if (typeName())
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