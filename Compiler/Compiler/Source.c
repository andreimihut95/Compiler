#include "alex.h"
#include "asin.h"

int main()
{
	int n = 0; char c;
	f = fopen("fisier.txt", "r");
	if (f == NULL)
		err("Eroare la fisier!");
	while ((c = fgetc(f)) != EOF)
		text[n++] = c;
	fclose(f);
	pointer_urmator = text;
	do
		getNextToken();
	while (lastToken->code != END);
	showAtoms();
	currentToken = tokens;
	unit();
	//terminare();
	return 0;
}
