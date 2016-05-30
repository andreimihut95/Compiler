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
	RetVal rv;
	if (consume(LBRACKET))
	{
		if (expr(&rv))
		{
			if (!rv.isCtVal)tkerr(currentToken, "the array size is not a constant");
			if (rv.type.typeBase != TB_INT)tkerr(currentToken, "the array size is not an integer");
			ret->nElements = rv.ctVal.i;
		}
		else
			ret->nElements = 0;
		if (consume(RBRACKET))
		{
			//printf("%d\n", ret->nElements);
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
	int n = symbols.end-symbols.begin;//aici s-a schimbat ca ii ceva eroare de la prof in laborator
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
			deleteSymbolsAfter(&symbols, symbols.begin+n-1);//redimensionare
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
	RetVal rv, rv1, rv2, rv3;
	if (stmCompound())
	{
		return 1;
	}
	if (consume(IF))
	{
		if (consume(LPAR))
		{
			if (expr(&rv))
			{
				if (rv.type.typeBase == TB_STRUCT)
					tkerr(currentToken, "a structure cannot be logically tested");
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
			if (expr(&rv))
			{
				if (rv.type.typeBase == TB_STRUCT)
					tkerr(currentToken, "a structure cannot be logically tested");
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
			expr(&rv1);
			if (consume(SEMICOLON))
			{
				expr(&rv2);
				if (rv2.type.typeBase == TB_STRUCT)
					tkerr(currentToken, "a structure cannot be logically tested");
				if (consume(SEMICOLON))
				{
					expr(&rv3);
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
		expr(&rv);
		if (currentFunction->type.typeBase == TB_VOID)
			tkerr(currentToken, "a void function cannot return a value");
		cast(&currentFunction->type.typeBase, &rv.type);
		if (consume(SEMICOLON))
		{
			return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste ;(SEMICOLON) dupa expresie sau return\n");
	}

	if (expr(&rv))
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


int expr(RetVal *rv)
{
	if (exprAssign(rv))
		return 1;
	return 0;
}

int exprAssign(RetVal *rv)
{
	Token *startToken = currentToken;
	RetVal  rve;
	if (exprUnary(rv))
	{
		if (consume(ASSIGN))
		{
			if (exprAssign(&rve))
			{
				if (!rv->isLVal)tkerr(currentToken, "cannot assign to a non-lval");
				if (rv->type.nElements > -1 || rve.type.nElements > -1)
					tkerr(currentToken, "the arrays cannot be assigned");
				cast(&rv->type, &rve.type);
				rv->isCtVal = rv->isLVal = 0;
				return 1;
			}
			else
				tkerr(currentToken, "Lipseste expresie dupa =");
		}
		//tkerr(currentToken, "Lipseste expresie dupa =");
		
	}
	currentToken = startToken;
	if (exprOr(rv))
		return 1;
	return 0;
}
int exprOr(RetVal *rv)
{
	if (exprAnd(rv))
	{
		exprOr1(rv);
		return 1;
	}
	return 0;
}

int exprOr1(RetVal *rv)
{
	Token *startToken = currentToken;
	RetVal rve;
	if (consume(OR))
	{
		if (exprAnd(&rve))
		{
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(currentToken, "a structure cannot be logically tested");
			rv->type = createType(TB_INT, -1);
			rv->isCtVal = rv->isLVal = 0;
			if (exprOr1(rv))
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

int exprAnd(RetVal *rv)
{
	if (exprEq(rv))
	{
		exprAnd1(rv);
		return 1;
	}
	return 0;
}

int exprAnd1(RetVal *rv)
{
	Token *startToken = currentToken;
	RetVal rve;
	if (consume(AND))
	{
		if (exprEq(&rve))
		{
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(currentToken, "a structure cannot be logically tested");
			rv->type = createType(TB_INT, -1);
			rv->isCtVal = rv->isLVal = 0;
			if (exprAnd1(rv))
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


int exprEq(RetVal *rv)
{
	if (exprRel(rv))
	{
		exprEq1(rv);
		return 1;
	}
	return 0;
}

int exprEq1(RetVal *rv)
{
	Token *startToken = currentToken;
	Token *tkop;
	RetVal rve;
	if (consume(EQUAL) || consume(NOTEQ))
	{
		tkop = consumedToken;
		if (exprRel(&rve))
		{
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(currentToken, "a structure cannot be compared");
			rv->type = createType(TB_INT, -1);
			rv->isCtVal = rv->isLVal = 0;
			if (exprEq1(rv))
				return 1;
		}
		else tkerr(currentToken, "-------------Lipseste exprREL dupa ==(EQUAL) sau !=(NOTEQ)-------------\n");
	}
	currentToken = startToken;
	return 1;

}

int exprRel(RetVal *rv)
{
	if (exprAdd(rv))
	{
		exprRel1(rv);
		return 1;
	}
	return 0;
}

int exprRel1(RetVal *rv)
{
	Token *startToken = currentToken;
	Token *tkop;
	RetVal rve;
	if (consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
	{
		tkop = consumedToken;
		if (exprAdd(&rve))
		{
			if (rv->type.nElements>-1 || rve.type.nElements>-1)
				tkerr(currentToken, "an array cannot be compared");
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(currentToken, "a structure cannot be compared");
			rv->type = createType(TB_INT, -1);
			rv->isCtVal = rv->isLVal = 0;
			if (exprRel1(rv))
				return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste exprADD dupa LESS(<), LESSEQ(<=), GREATER(>) sau GREATEREQ(>=)-------------\n");
	}
	currentToken = startToken;
	return 1;
}

int exprAdd(RetVal *rv)
{
	if (exprMul(rv))
	{
		exprAdd1(rv);
		return 1;
	}
	return 0;
}

int exprAdd1(RetVal *rv)
{
	Token *startToken = currentToken;
	Token *tkop;
	RetVal rve;
	if (consume(ADD) || consume(SUB))
	{
		tkop = consumedToken;
		if (exprMul(&rve))
		{
			if (rv->type.nElements>-1 || rve.type.nElements>-1)
				tkerr(currentToken, "an array cannot be added or subtracted");
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(currentToken, "a structure cannot be added or subtracted");
			rv->type = getArithType(&rv->type, &rve.type);
			rv->isCtVal = rv->isLVal = 0;
			if (exprAdd1(rv))
				return 1;
		}
		else tkerr(currentToken, "-------------Lipseste exprMUL dupa +(ADD) sau -(SUB)-------------\n");
	}
	currentToken = startToken;
	return 1;
}

int exprMul(RetVal *rv)
{
	if (exprCast(rv))
	{
		exprMul1(rv);
		return 1;
	}
	return 0;
}

int exprMul1(RetVal *rv)
{
	Token *startToken = currentToken;
	Token *tkop;
	RetVal rve;
	if (consume(MUL) || consume(DIV))
	{
		tkop = consumedToken;
		if (exprCast(&rve))
		{
			if (rv->type.nElements>-1 || rve.type.nElements>-1)
				tkerr(currentToken, "an array cannot be multiplied or divided");
			if (rv->type.typeBase == TB_STRUCT || rve.type.typeBase == TB_STRUCT)
				tkerr(currentToken, "a structure cannot be multiplied or divided");
			rv->type = getArithType(&rv->type, &rve.type);
			rv->isCtVal = rv->isLVal = 0;
			if (exprMul1(rv))
				return 1;
		}
		else 
			tkerr(currentToken, "-------------Lipseste exprCAST dupa *(MUL) sau /(DIV)-------------\n");
	}
	currentToken = startToken;
	return 1;
}

int exprCast(RetVal *rv)
{
	Token *startToken = currentToken;
	RetVal rve;
	Type t;
	if (consume(LPAR))
	{
		if (typeName(&t))
		{
			if (consume(RPAR))
			{
				if (exprCast(&rve))
				{
					cast(&t, &rve.type);
					rv->type = t;
					rv->isCtVal = rv->isLVal = 0;
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
	currentToken = startToken;
	if (exprUnary(rv))
		return 1;
	return 0;
}

int exprUnary(RetVal *rv)
{
	Token *tkop;
	Token *startToken = currentToken;
	if (consume(NOT) || consume(SUB))
	{
		tkop = consumedToken;
		if (exprUnary(rv))
		{
			if (tkop->code == SUB) {
				if (rv->type.nElements >= 0)tkerr(currentToken, "unary '-' cannot be applied to an array");
				if (rv->type.typeBase == TB_STRUCT)
					tkerr(currentToken, "unary '-' cannot be applied to a struct");
			}
			else {  // NOT
				if (rv->type.typeBase == TB_STRUCT)tkerr(currentToken, "'!' cannot be applied to a struct");
				rv->type = createType(TB_INT, -1);
			}
			rv->isCtVal = rv->isLVal = 0;
			return 1;
		}
		else
			tkerr(currentToken, "-------------Lipseste exprUNARY dupa !(NOT) sau -(SUBSTRACT)-------------\n");
	}
	if (exprPostfix(rv))
		return 1;
	currentToken = startToken;
	return 0;
}

int exprPostfix(RetVal *rv)
{
	if (exprPrimary(rv))
	{
		exprPostfix1(rv);
		return 1;
	}
	return 0;
}

int exprPostfix1(RetVal *rv)
{
	RetVal rve;
	Token *startToken = currentToken;
	Token *tkName;
	Type typeInt;
	if (consume(LBRACKET))
	{
		if (expr(&rve))
		{
			if (rv->type.nElements<0)	tkerr(currentToken, "only an array can be indexed");
		    typeInt = createType(TB_INT, -1);
			cast(&typeInt, &rve.type);
			rv->type = rv->type;
			rv->type.nElements = -1;
			rv->isLVal = 1;
			rv->isCtVal = 0;
			if (consume(RBRACKET))
			{
				if (exprPostfix1(rv))
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
			tkName = consumedToken;
			Symbol      *sStruct = rv->type.s;
			Symbol      *sMember = findSymbol(&sStruct->members, tkName->text);
			if (!sMember)
				tkerr(currentToken, "struct %s does not have a member %s", sStruct->name, tkName->text);
			rv->type = sMember->type;
			rv->isLVal = 1;
			rv->isCtVal = 0;
			if (exprPostfix1(rv))
			{
				return 1;
			}
		}
		else
			tkerr(currentToken, "-------------Lipseste ID dupa .(DOT)-------------\n");
	}
	return 1;
}

int exprPrimary(RetVal *rv)
{
	Token *startToken = currentToken;
	Token *tkName;
	RetVal arg;
	if (consume(ID))
	{
		tkName = consumedToken;
		Symbol *s = findSymbol(&symbols, tkName->text);
		if (!s)tkerr(currentToken, "undefined symbol %s", tkName->text);
		rv->type = s->type;
		rv->isCtVal = 0;
		rv->isLVal = 1;
		if (consume(LPAR))
		{
			Symbol **crtDefArg = s->args.begin;
			if (s->cls != CLS_FUNC&&s->cls != CLS_EXTFUNC)
				tkerr(currentToken, "call of the non-function %s", tkName->text);
			if (expr(&arg))
			{
				if (crtDefArg == s->args.end)tkerr(currentToken, "too many arguments in call");
				cast(&(*crtDefArg)->type, &(&arg)->type);
				crtDefArg++;
				while (1)
				{
					if (consume(COMMA))
					{
						if (expr(&arg))
						{
							if (crtDefArg == s->args.end)tkerr(currentToken, "too many arguments in call");
							cast(&(*crtDefArg)->type, &(&arg)->type);
							crtDefArg++;
						}
						else
							tkerr(currentToken, "-------------Lipseste expresie dupa ,(COMMA)-------------\n");
					}
					else break;
				}

			}
			if (consume(RPAR))
			{
				if (crtDefArg != s->args.end)tkerr(currentToken, "too few arguments in call");
				rv->type = s->type;
				rv->isCtVal = rv->isLVal = 0;
				return 1;
			}
			else
			{
				if (s->cls == CLS_FUNC || s->cls == CLS_EXTFUNC)
					tkerr(currentToken, "missing call for function %s", tkName->text);
				tkerr(currentToken, "-------------Lipseste )(RPAR) dupa expresie-------------\n");
			}
		}
		return 1;
	}
	Token *tki, *tkr, *tkc, *tks;
	if (consume(CT_INT))
	{
		tki = consumedToken;
		rv->type = createType(TB_INT, -1); rv->ctVal.i = tki->i;
		rv->isCtVal = 1; rv->isLVal = 0;
		return 1;
	}
	else if (consume(CT_REAL))
	{
		tkr = consumedToken;
		rv->type = createType(TB_DOUBLE, -1); rv->ctVal.d = tkr->r;
		rv->isCtVal = 1; rv->isLVal = 0;
		return 1;
	}
	else if (consume(CT_CHAR))
	{
		tkc = consumedToken;
		rv->type = createType(TB_CHAR, -1); rv->ctVal.i = tkc->i;
		rv->isCtVal = 1; rv->isLVal = 0;
		return 1;
	}
	else if (consume(CT_STRING))
	{
		tks = consumedToken;
		rv->type = createType(TB_CHAR, 0); rv->ctVal.str = tks->text;
		rv->isCtVal = 1; rv->isLVal = 0;
		return 1;
	}
	if (consume(LPAR))
	{
		if (expr(rv))
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