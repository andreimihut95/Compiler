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
#endif