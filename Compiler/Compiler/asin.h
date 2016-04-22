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
		return 1;
	tkerr(currentToken, "Syntax error \n");
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
					if (declVar()) {}
					else break;
				}
				if (consume(RACC)) {
					if (consume(SEMICOLON))
					{
						return 1;
					}
					else
						tkerr(currentToken, "missing ;\n");
				}
				else
					tkerr(currentToken, "missing }\n");
			}
			else
			{
				currentToken = startToken;
				return 0;
			}
		}
		else
			tkerr(currentToken, "missing ID");
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
						tkerr(currentToken, "Missing ID after ,\n");
				}
				else break;
			}
			if (consume(SEMICOLON))
			{
				return 1;
			}
			else
				tkerr(currentToken, "Missing ;\n");
		}
		else
			tkerr(currentToken, "Missing ID\n");
	}
	currentToken = startToken;
	return 0;
}

int typeBase()
{

	Token *startToken = currentToken;
	if (consume(INT))
	{
		return 1;
	}
	if (consume(DOUBLE))
	{
		return 1;
	}
	if (consume(CHAR))
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
			tkerr(currentToken, "ID expected\n");
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
			tkerr(currentToken, "Missing ] after expression\n");
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
							tkerr(currentToken, "Invalid declaration!\n");
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
					tkerr(currentToken, "Missing stmCompound\n");
			}
			else
				tkerr(currentToken, "Missing )\n");
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
		else tkerr(currentToken, "Missing Id\n");
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
			tkerr(currentToken, "Missing }");
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
								tkerr(currentToken, "Missing stm after else!\n");
						}
						return 1;
					}
				}
				else
					tkerr(currentToken, "Missing ) after expression\n");
			}
			else
				tkerr(currentToken, "Missing expression\n");
		}
		else
			tkerr(currentToken, "Missing lpar");

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
				}
				else
					tkerr(currentToken, "Mising ) after expression\n");
			}
			else
				tkerr(currentToken, "Missing expression after (\n");
		}
		else
			tkerr(currentToken, "Missing (\n");
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
							tkerr(currentToken, "Missing stm\n");
					}
					else
						tkerr(currentToken, "Missing ) after expression\n");
				}
				else
					tkerr(currentToken, "Missing ; after expression\n");
			}
			else
				tkerr(currentToken, "Missing ; after expression\n");
		}
		else
			tkerr(currentToken, "Missing ( after expression\n");
	}

	if (consume(BREAK))
	{
		if (consume(SEMICOLON))
		{
			return 1;
		}
		else
			tkerr(currentToken, "Missing ; after break\n");
	}

	if (consume(RETURN))
	{
		expr();
		if (consume(SEMICOLON))
		{
			return 1;
		}
		else
			tkerr(currentToken, "Missing ; after return\n");
	}

	if (expr())
	{
		if (consume(SEMICOLON))
		{
			return 1;
		}
		else
			tkerr(currentToken, "Missing ; after expression\n");
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
			else
				tkerr(currentToken, "Missing expOr1\n");
		}
		else
			tkerr(currentToken, "Missing exprand\n");
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
			else
				tkerr(currentToken, "Missing exprAnd1\n");
		}
		else
			tkerr(currentToken, "Missing exprEq\n");
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
			else
				tkerr(currentToken, "Missing exprEq1\n");
		}
		else tkerr(currentToken, "Incomplete Equal expression");
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
			else
				tkerr(currentToken, "Missing exprRelPrim\n");
		}
		else
			tkerr(currentToken, "Incomplete Rel expression");
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
			else
				tkerr(currentToken, "Missing exprAdd1\n");
		}
		else tkerr(currentToken, "Incomplete Add expression\n");
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
			tkerr(currentToken, "Missing exprMul1\n");
		}
		else tkerr(currentToken, "Incomplete Mul expression");
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
					tkerr(currentToken, "Missing exprCast\n");
			}
			else
				tkerr(currentToken, "Missing )\n");
		}
		else
			tkerr(currentToken, "Missing typeName\n");
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
			tkerr(currentToken, "Missing exprUnary\n");
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
				else
					tkerr(currentToken, "Missing exprPostfix1\n");
			}
			else
				tkerr(currentToken, "Missing )\n");
		}
		else
			tkerr(currentToken, "Missing expr\n");
	}
	if (consume(DOT))
	{
		if (consume(ID))
		{
			if (exprPostfix1())
			{
				return 1;
			}
			else
				tkerr(currentToken, "Missing exprPostfix1\n");
		}
		else
			tkerr(currentToken, "Missing ID\n");
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
							tkerr(currentToken, "Missing expr after , \n");
					}
					else break;
				}

			}
			if (consume(RPAR))
			{
				return 1;
			}
			else
				tkerr(currentToken, "Missing RPAR\n");
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
				tkerr(currentToken, "RPAR is expected");
		}
		tkerr(currentToken, "Expression is missing\n");
	}
	currentToken = startToken;
	return 0;
}

#endif