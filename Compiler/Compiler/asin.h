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
				tkerr(currentToken, "missing {");
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
	printf("Am intrat pe declVar\n");
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
				printf("Succes declVar\n");
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
	printf("Am intrat pe typebaase\n");
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
	printf("Am intrat pe arraydecl\n");
	Token *startToken = currentToken;
	if (consume(LBRACKET))
	{
		expr();
		if (consume(RBRACKET))
		{
			printf("Succes arrayDecl\n");
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
	if (consume(ID) == 0)
	{
		tkerr(currentToken, "Incomplete DeclFunc declaration, Id expected");
		return 0;
	}
	if (consume(LPAR) == 0)
	{
		tkerr(currentToken, "Incomplete DeclFunc declaration, Lpar expected");
		return 0;
	}
	if (funcArg())
	{
		while (1)
		{
			if (consume(COMMA))
			{
				if (funcArg())
				{
					return 1;
				}
				else
					tkerr(currentToken, "Invalid declaration!\n");
			}
			else
				break;
		}
	}
	if (consume(RPAR) == 0)
	{
		tkerr(currentToken, "Incomplete DeclFunc declaration, RPAR expected");
		return 0;
	}
	if (stmCompound() == 0)
	{
		tkerr(currentToken, "Incomplete DeclFunc declaration, StmCompound expected");
		return 0;
	}
	return 1;
}

int declFunc()
{
	printf("Am intrat pe declFunc!\n");
	if (typeBase())
	{
		consume(MUL);
		if (declFuncHeader())
			return 1;
	}
	if (consume(VOID))
		if (declFuncHeader())
			return 1;
	return 0;

}

int funcArg()
{
	Token *startToken = currentToken;
	if (typeBase())
	{
		if (consume(ID))
		{
			arrayDecl();
			return 1;
		}
		else tkerr(currentToken, "Missing Id\n");
	}
	currentToken = startToken;
	return 0;

}

int stmCompound()
{

	if (!consume(LACC))
		return 0;
	while (1)
	{
		if (declVar())
		{

		}
		else
			if (stm())
			{

			}
			else
				break;
	}
	if (!consume(RACC))
		tkerr(currentToken, "misssing } or syntax error");
	return 1;

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
	{
		return 1;
	}
	return 0;
}

int exprAssign()
{
	Token *startToken = currentToken;
	if (exprUnary())
	{
		if (consume(ASSIGN))
		{
			if (exprAssign())
			{
				return 1;
			}
		}
		else
			tkerr(currentToken, "Missing =\n");
	}
	if (exprOr())
	{
		return 1;
	}
	currentToken = startToken;
	return 0;
}

int exprOr()
{
	if (exprAnd())
	{
		if (exprOr1())
		{
			return 1;
		}
	}
	return 0;
}

int exprOr1()
{
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
	return 1;
}

int exprAnd()
{
	if (exprEq())
	{
		if (exprAnd1())
		{
			return 1;
		}
	}
	return 0;
}

int exprAnd1()
{
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
	return 1;
}


int exprEq()
{
	if (exprRel())
		if (exprEq1())
			return 1;
	return 0;
}

int exprEq1()
{
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
	return 1;

}

int exprRel()
{
	if (exprAdd())
		if (exprRel1())
			return 1;
	return 0;
}

int exprRel1()
{
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
	return 1;
}

int exprAdd()
{
	if (exprMul())
		if (exprAdd1())
			return 1;
	return 0;
}

int exprAdd1()
{
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
	return 1;
}

int exprMul()
{
	if (exprCast())
		if (exprMul1())
			return 1;
	return 0;
}

int exprMul1()
{
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
		if (exprPostfix1())
			return 1;
	return 0;
}

int exprPostfix1()
{
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
	}
	currentToken = startToken;
	return 0;
}

#endif