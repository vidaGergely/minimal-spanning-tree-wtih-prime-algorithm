
#include "iskola.h"
int getline(char s[], int lim)
{
	int i, c;
	for (i = 0; i < lim && (c = getchar()) != EOF && c != '\n'; ++i)
		s[i] = c;
	s[i] = '\0';
	while (c != EOF && c != '\n')
		c = getchar();
	return i;
}

int** dinMtxFoglal(int*** ptomb, int szeles, int magas)
{
	int** uj;
	int y;
	uj = (int**)calloc(magas, magas * sizeof(int*));
	for (y = 0; y < magas; ++y) {
		uj[y] = (int*)calloc(szeles, szeles * sizeof(int));
	}
	*ptomb = uj; // *ptomb >> a float** típusú változó!
	return *ptomb;
}
