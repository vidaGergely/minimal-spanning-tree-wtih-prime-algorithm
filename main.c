#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>
#define PI 3.14159265358979323846
#define SUGAR 400
#define MAXMERET 32
#define MAXBEOLVAS 16
#include "iskola.h"

// strukt�ra, - szomsz�ds�gi lista egy pontja
// maga c�lpont �s az �lhez tartoz� s�ly t�rol�sa
struct ListaPontja {
	int celpont;
	int suly;
	struct ListaPontja* next;
};
// Strukt�ra szomsz�ds�gi lista

struct Lista {
	struct ListaPontja* head;
};

// Ez a strukt�ra egy gr�fot reprezent�l, a gr�f egy szomsz�ds�gi lista t�mb
// A t�mb m�rete: V, vagyis a gr�f cs�csainak sz�ma.
struct Graf {
	int V;
	struct Lista* tomb;
};

//l�ncolt lista adatszerkezet a megjeln�tend� pontok koordin�t�i �s azonos�t�ival
struct Pontok {
	int id;
	double x;
	double y;
	struct Pontok* kovetkezo;
};

// lista pontjainak helyfoglal�sa �s a c�lpont, s�ly t�rol�sa
struct ListaPontja* ujIlleszkedesiListaPontja(int celpont, int suly)
{
	struct ListaPontja* ujPont =
		(struct ListaPontja*)malloc(sizeof(struct ListaPontja));
	ujPont->celpont = celpont; // c�lont hozz�ad�sa
	ujPont->suly = suly; // s�ly hozz�ad�sa az �lhez.
	ujPont->next = NULL;
	return ujPont;
}
//V pont� gr�fnak helyfoglal�s
struct Graf* grafKeszit(int V)
{
	int i;
	struct Graf* graf = (struct Graf*)malloc(sizeof(struct Graf));
	graf->V = V;
	graf->tomb = (struct Lista*)malloc(V * sizeof(struct Lista));
	for (i = 0; i < V; ++i)
		graf->tomb[i].head = NULL;
	return graf;
}
// �j �l hozz�ad�sa forras-b�l celpont-ba. Az �j cs�cs hozz�ad�dik a szomsz�ds�gi list�hoz
void elHozzaad(struct Graf* graf, int forras, int celpont, int suly)
{
	struct ListaPontja* ujPont = ujIlleszkedesiListaPontja(celpont, suly);
	ujPont->next = graf->tomb[forras].head;
	graf->tomb[forras].head = ujPont;
	ujPont = ujIlleszkedesiListaPontja(forras, suly);
	ujPont->next = graf->tomb[celpont].head;
	graf->tomb[celpont].head = ujPont;
}
// seg�df�ggv�ny a gr�f elemeinek bej�r�s�hoz, kiirat�s�hoz a programban a f�ggv�nyt nem haszn�lom
void grafKiir(struct Graf* graf)
{
	int v;
	for (v = 0; v < graf->V; ++v) {
		struct ListaPontja* pontok = graf->tomb[v].head;
		printf("\n Szomszedsagi lista %d. csucsa\n head ", v);
		while (pontok) {
			printf("-> %d\t(Sulya:[%d])\t", pontok->celpont, pontok->suly);
			pontok = pontok->next;
		}
		printf("\n");
	}
}
// a szomsz�ds�gi m�trixot felt�lti poz�ci� helyesen. A pontok k�z�tt fut� �lek s�ly�val
void szomszedsagiMtxFeltolt(int** ptomb, struct Graf* graf)
{
	int i;
	struct ListaPontja* elem;

	for (i = 0; i < graf->V; ++i) {
		elem = graf->tomb[i].head;
		while (elem) {
			ptomb[i][elem->celpont] = elem->suly;
			elem = elem->next;
		}
	}
}
// m�trix kiirat�sa nxm-es m�trix kiirat�s�ra k�pes a f�ggv�ny
void mtxKiir(int** tomb, int sor, int oszlop)
{
	int i, j;
	for (i = 0; i < sor; ++i) {
		for (j = 0; j < oszlop; ++j)
			printf("%d\t", tomb[i][j]);
		printf("\n\n");
	}
}
// nxn-es dinamikusan foglalt m�trix mem�ri�j�t felszabad�t�  f�ggv�ny
void mtxFelszabadit(int** dinMtx, int V)
{
	int i;
	for (i = 0; i < V; i++) {
		free(*(dinMtx + i));
	}
	free(dinMtx);
}
// a pr�m algoritmusba integr�lt ujMatrixbaMasol f�ggv�ny
//V itt a gr�f cs�csainak sz�m�t jeletni
void ujMatrixbaMasol(int szulo[], int** szomszedsagiMtx, int** minSulyuFeszFaMTX, int V)
{
	int i;
	for (i = 1; i < V; i++) {
		minSulyuFeszFaMTX[i][szulo[i]] = szomszedsagiMtx[i][szulo[i]];
	}
}
// INT_MAX Az integer maxim�lis �rt�ke, ehhez viszony�tva van meghat�rozva a minimum �rt�k.
// Az algoritmus mindig hozz�vesz egy pontot a m�r megl�v� f�hoz, ez a f�ggv�ny a bej�rhat�
//�lek k�z�l kiv�lasztja a legkisebbet
// minimumKulcs algoritmus kiv�lasztja a 0-val kapcsolatban l�v� pontok k�z�l azt, amelyiknek a legkisebb a s�lya
// eredm�ny�l annak a pontnak a sz�m�t adja ahol minim�lis az �l
int minimumKulcs(int kulcs[], bool minSulyuFeszFaBeallit[], int V)
{
	int min = INT_MAX, v;
	int minIndex;
	for (v = 0; v < V; v++){
		if (minSulyuFeszFaBeallit[v] == false && kulcs[v] < min){
			min = kulcs[v], minIndex = v;
		}
	}
	return minIndex;
}
void primAlg(int** szomszedsagiMtx, int** minSulyuFeszFaMTX, int V)
{
	int i, szamolo, u, v;
	int *szulo = (int *)malloc(sizeof(int) * V);
	int *kulcs = (int *)malloc(sizeof(int) * V);// kulcs tartalmazza a minim�lis s�lyokat
	bool *minSulyuFeszFaBeallit = (bool *)malloc(sizeof(bool) * V);
	// inicializ�l�dik a kulcs �s a minsulyuFeszFaBeallit tomb
	for (i = 0; i < V; i++){
		kulcs[i] = INT_MAX, minSulyuFeszFaBeallit[i] = false;
	}
	kulcs[0] = 0; // 0 pont lesz az els� elem ami a f�ban benne lesz, kiindul� pont
	szulo[0] = -1;
	// a ciklus V-1 ig megy, mert az �lek sz�ma minim�lis s�ly� fesz�ta eset�ben
	//mindig a pontok sz�ma-1
	for (szamolo = 0; szamolo < V - 1; szamolo++) {
		// u �rt�ke egyenl� a megl�v� f�hoz kapcsol�d� legkisebb s�ly� �lhez tartoz� ponttal
		u = minimumKulcs(kulcs, minSulyuFeszFaBeallit, V);
		//a t�mb u-adik eleme hamisr�l igazra v�ltozik, teh�t beker�l a pont a f�ba
		minSulyuFeszFaBeallit[u] = true;
		// Kulcs �rt�kek friss�t�se
		for (v = 0; v < V; v++) {
			//minSulyuFeszFaBeallit[v] eleme ha false akkor nincsen benne a f�ban m�g.
			//Kulcs�rt�k friss�t�se, ha a m�trix[u][v] eleme kisebb mint a kulcs[v] eleme
			if (szomszedsagiMtx[u][v] && minSulyuFeszFaBeallit[v] == false && szomszedsagiMtx[u][v] < kulcs[v]){
				szulo[v] = u;
				kulcs[v] = szomszedsagiMtx[u][v];
			}
		}
	}
	// �tm�sol�dik a m�trix egy �j m�trixba poz�ci�helyesen
	ujMatrixbaMasol(szulo, szomszedsagiMtx, minSulyuFeszFaMTX, V);
	free(szulo);
	free(kulcs);
	free(minSulyuFeszFaBeallit);
}

void ListaFelszabadit(struct Lista lista){
	struct ListaPontja *tmp, *p = lista.head;
	while (p != NULL){ //pont felszabad�t�sa NULL ig
		tmp = p->next;
		free(p);
		p = tmp;
	}
}
// Szomsz�ds�gi lista t�mb felszabad�t�sa
void SzomszedsagiListaFelszabadit(struct Graf* szomsz_lista){
	int i;
	for (i = 0; i < szomsz_lista->V; i++) //felszabad�t minden egyes l�ncolt list�t
		ListaFelszabadit(szomsz_lista->tomb[i]);
	free(szomsz_lista->tomb); //felszabad�tja a l�ncolt lista t�mb�t
	free(szomsz_lista); //felszabad�tja a szomsz�ds�gi list�t
}
// lista elemeinek kiirat�sa ellen�rz�s c�lj�b�l
void listaKiir(struct Pontok** lista)
{
	struct Pontok* mutato = *lista;
	while (mutato != NULL) {
		printf("\n id: [%d] x:[%f] \t y:[%f] \n", mutato->id, mutato->x, mutato->y);
		mutato = mutato->kovetkezo;
	}
}
//l�ncolt lista v�g�re f�zi az azonos�t�t, x �s y koordin�t�t a f�ggv�ny
void listaVegereFuz(struct Pontok** lista, int id, double x, double y)
{
	struct Pontok* jelenlegi = *lista;
	struct Pontok* ujElem;
	ujElem = (struct Pontok*)malloc(sizeof(struct Pontok));
	ujElem->id = id;
	ujElem->x = x;
	ujElem->y = y;
	ujElem->kovetkezo = NULL;
	if (jelenlegi == NULL) {
		*lista = ujElem;
	}
	else {
		while (jelenlegi->kovetkezo != NULL) {
			jelenlegi = jelenlegi->kovetkezo;
		}
		jelenlegi->kovetkezo = ujElem;
	}
}

// id alapj�n keres�s, a gr�f pontjaihoz tartoz� koordin�t�k megkeres�se
struct Pontok* idKeres(struct Pontok* kereso, int id)
{
	while (kereso != NULL) {
		if (kereso->id == id) {
			return kereso;
		}
		kereso = kereso->kovetkezo;
	}
	return NULL;
}

//radi�n - fok konverter
//a sz�gf�ggv�nyekhez val� �tv�lt�shoz
double radianKonvert(double atvaltando)
{
	atvaltando = atvaltando * PI / 180;
	return atvaltando;
}
// kiszamolja adott sug�rral �s adott oszt�si fokban az adott pozicioj� koorin�t�t
//pl  sug�r fix 400, osztofokban pedig a fok sz�ma pl 360/5 eset�n 72 fok 72. fokra fogja kisz�matni az 1. pontot
// k�vetkez�ben pedig 142 fok-ra sz�m�tja ki ... persze adagolni kell neki a sz�geket. �s megmondja hogy mi lesz a koordin�ta
double xKoordinata(int sugar, double osztoFokban)
{
	double xEredmenye;
	xEredmenye = (cos(radianKonvert(osztoFokban)) * sugar);
	return xEredmenye;
}

double yKoordinata(int sugar, double osztoFokban)
{
	double yEredmenye;
	yEredmenye = (sin(radianKonvert(osztoFokban)) * sugar);
	return yEredmenye;
}
// elkesziti a koorinatakat �s belemasolja egy listaba az elemeket
// listavegere f�z fv a l�ncolt lista v�g�re f�zi az x,y koordin�t�t �s az ID-t is ami itt i v�ltoz�.

void koordinataKeszit(struct Pontok** lista, int sugar, int darab)
{
	int i;
	double osztasDarab = 360 / darab;
	for (i = 0; i < darab; i++) {
		listaVegereFuz(lista, i, xKoordinata(sugar, osztasDarab), yKoordinata(sugar, osztasDarab));
		osztasDarab += 360 / darab;
	}
}


//Fajlkezeles
// a f�jl els� sor�b�l kiolvassa a gr�f pontjainak sz�m�t
int vDarab(FILE* be){
	char *token;
	char buffer[5];
	int darab;
	fgets(buffer, 5, be);
	token = strtok(buffer, "");
	darab = atoi(token);
	return darab;
}
// beolvassa a f�ggv�ny a bemeneti f�jlb�l soronk�nt, "darabol�ssal", tabul�tor elv�laszt�ssal a Szomsz�ds�gi lista t�mbbe az elemeket.
void fajlBeolvas(FILE *be, struct Graf* graf){
	char buffer[MAXBEOLVAS];
	char *token;
	int honnet, hova, suly;
	fgets(buffer, MAXBEOLVAS, be);
	while (!feof(be)) {
		fgets(buffer, MAXBEOLVAS, be);
		token = strtok(buffer, "\t");
		honnet = atoi(token);
		token = strtok(NULL, "\t");
		hova = atoi(token);
		token = strtok(NULL, "\t");
		suly = atoi(token);
		// �lhozz�ad fv. a pontok k�z�tti kapcsolatot reprezent�lja a fv, az �lek s�lyait adja hozz�
		elHozzaad(graf, honnet, hova, suly);
	}
}
// a f�ggv�ny l�trehozza az SVG f�jl TAG-c�mk�it �s felparam�terezi �ket a megfelel� koordin�t�val
// l�trehozza a circle TAG-eket,amik a pontok �s m�trix alapj�n meghat�rozza, hogy melyik k�t pont k�z�tt kell
// l�trehozni line TAG-et, text TAG seg�ts�g�vel pedig feliratozza az �leket, k�r alakzatokat, �gy alak�tva ki a gr�fot.

void fajlbaIras(FILE* fajl, struct Pontok** points, int **dinMtx, struct Graf* graf){
	int i, j;
	struct Pontok* tmp = *points;
	struct Pontok* keresTmp1;
	struct Pontok* keresTmp2;

	fprintf(fajl, "<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" "
		"xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"-500 -500 1000 1000\" "
		"xml:space=\"preserve\"> \n");

	for (i = 1; i < graf->V; i++) {
		for (j = 0; j < graf->V; j++) {
			if (dinMtx[i][j] != 0) {
				keresTmp1 = idKeres(tmp, i);
				keresTmp2 = idKeres(tmp, j);
				fprintf(fajl, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(255,0,0);stroke-width:2\" /> \n", keresTmp1->x, keresTmp1->y, keresTmp2->x, keresTmp2->y);
				fprintf(fajl, "<text x=\"%f\" y=\"%f\" fill=\"black\" font-size=\"35\">%d</text>", (keresTmp1->x + keresTmp2->x) / 2, (keresTmp1->y + keresTmp2->y) / 2, dinMtx[i][j]);
			}
		}
	}
	while (tmp != NULL) {
		fprintf(fajl, "<circle cx=\"%f\" cy=\"%f\" r=\"25\" stroke=\"green\" fill=\"pink\">\n", tmp->x, tmp->y);
		fprintf(fajl, "</circle>");
		fprintf(fajl, "<text x=\"%f\" y=\"%f\" fill=\"black\" font-size=\"35\">%d</text>", tmp->x, tmp->y, tmp->id);
		tmp = tmp->kovetkezo;
	}
	fprintf(fajl, "</svg>\n");
}
// felszabad�tja a l�ncolt lista adatszerekezetet, melyben a pontok vannak t�rolva
void lancoltListaFelszabadit(struct Pontok* lista)
{
	struct Pontok* tmp;
	while (lista != NULL)
	{
		tmp = lista;
		lista = lista->kovetkezo;
		free(tmp);
	}
}




int main()
{

	FILE *be;
	int V, ok = 0;
	// gr�f mutat�  - elk�sz�l a V pont� gr�f  - szomsz�ds�gi lista, ami egy gr�fot reprezent�l
	struct Graf* graf;
	// mtx ami a list�b�l ker�l kialak�t�sra
	int** szomszedsagiMatrix;
	// prim algoritmus lefuttat�sa ut�n a szomsz�ds�gi m�trix, vagyis a min sulyu fesz fa sulyai poziciohelyesen itt talalhatoak.
	int** minSulyuFeszitoFaMatrix;
	// megjelen�t�shez haszn�lt l�ncolt lista: ID-t 0-t�l V-1-ig, 2 darab double-t tartalmaz x, �s y koordin�t�t k�r alakban t�rt�nik
	// a pontok elrendez�se
	struct Pontok* koordinataPontok = NULL;
	//getline f�ggv�ny ideiglenes t�rol� t�mbje
	char beker[MAXMERET + 1];
	// a m�dos�tott f�jlnevet tartalmaz� t�mb
	char ideiglenesBeker[MAXMERET + 1];
	printf("Adja meg a fajl nevet KITERJESZTESSEL, a fajl tartalmazza:\n"
		"- a fajl elso soraban a graf pontjainak szamat\n"
		" - tobbi soraban pedig a pontok szamat, ami kozott van kapcsolatot es a kapcsolat sulyat tabulatorokkal "
		"elvalasztva!\n\n");
	int hossz = getline(beker, MAXMERET);
	if (hossz > 0) {
		if (hossz < MAXBEOLVAS){
			// V kiolvasasa a fajl elso sorabol, csak szam lehet V = pontok szamaval
			be = fopen(beker, "r");
			if (be){
				printf("A fajl beolvasasa sikeres volt !\n");
				// sikeres f�jlbeolvas�s eset�n V, mint glob�lis v�ltoz� �rt�kad�sa itt t�rt�nik meg
				// A vDarab f�ggv�ny a beolvasott f�jlb�l, ha az l�tezik az els� sor�ban tal�lhat� sz�mot kiolvassa, sz�mm� alak�tja
				// �s �rt�k�l adja V-nek
				V = vDarab(be);
				//grafkesztit f�gv�ny lefoglalja a mem�ri�t V pont� gr�fnak, a strukt�ra, amiben a gr�f pontjai megtal�lhat�ak
				graf = grafKeszit(V);
				// fajlbeolvas f�ggveny a fajl tartalmat beolvassa az els� sor kiv�tel�vel, beolvas�s az utols� sorig megy
				// a f�jl tartalma rendre: honn�t, hova, mennyivel. A gr�f kapcsolatait jellemzi a beolvas�s soronk�nt val�sul meg
				// ezzel egyidej�leg a sorokb�l kiolvasott �rt�ket az elhozzaad f�ggv�ny fogja a gr�fba r�gz�teni.
				fajlBeolvas(be, graf);
				fclose(be);
			}
			else{
				printf("A program leall !\n");

				perror("Hiba oka");
				getchar();
				exit(0);
			}
			while (ok == 0){
				printf("Kerem adjon meg egy kimeneti fajlnevet, amiben a grafok abrazolva lesznek! (KITERJESZTES NELKUL!)\n\n");
				hossz = getline(beker, MAXBEOLVAS);
				if (hossz != 0){
					// megadott f�jln�vhez a kiterjeszt�s hozz�ad�sa �s az _eredet, _modositott szavak hozz�ad�sa
					strcpy(ideiglenesBeker, beker);
					strcat(beker, "_eredeti.svg");
					strcat(ideiglenesBeker, "_modositott.svg");
					// szomszedsagi mtx elemeinek felt�lt�se �s memoria foglalasa
					szomszedsagiMtxFeltolt(dinMtxFoglal(&szomszedsagiMatrix, V, V), graf);
					// pr�m algoritmus futtat�sa a szomsz�ds�gi m�trixon �s a v�geredm�ny elk�sz�t�se dinamikusan foglalt m�trixba
					// a v�geredm�ny m�r a minim�lis s�ly� fesz�t�fa s�lyainak sz�m�t fogja tartalmazni.
					// egy nem nulla s�ly� �l poz�ci�ja a m�trixban azt is jelenti, hogy melyik k�t pont k�z�tt megy az �l
					primAlg(szomszedsagiMatrix, dinMtxFoglal(&minSulyuFeszitoFaMatrix, V, V), V);
					//  koordinata keszit fv elk�sz�ti �s l�ncolt list�ba teszi R sugar� �s n pontb�l �ll� gr�f koordin�t�it
					koordinataKeszit(&koordinataPontok, SUGAR, V);
					// kimeneti f�jlok l�trehoz�sa a megadott f�jln�vvel
					// fajlbaIras f�ggv�ny v�r egy FILE t�pus� pointer-t, egy strukt�r�t, egy m�trixot �s a gr�f strukt�r�j�t.
					be = fopen(beker, "w+");
					if (be){
						fajlbaIras(be, &koordinataPontok, szomszedsagiMatrix, graf);
						fclose(be);
					}
					else{
						printf("A program leall !\n");
						perror("Hiba oka");
						exit(0);
					}
					be = fopen(ideiglenesBeker, "w+");
					if (be){
						fajlbaIras(be, &koordinataPontok, minSulyuFeszitoFaMatrix, graf);
						fclose(be);
					}
					else{
						printf("A program leall !\n");
						perror("Hiba oka");
						exit(0);
					}
					ok = 1;
				}
				else{
					printf("Nem adott meg fajlnevet! \n");
				}
			}
		}
		else {
			printf("A fajl neve nem lehet hosszabb %d karakternel!\n A program kilep!\n", MAXBEOLVAS);
			getchar();
		}
	}
	else {
		printf("A program leall, mert nem adott meg bemeneti fajlnevet ! \n");
		getchar();
		exit(0);
	}
	// mem�ria felszabad�t�sa
	mtxFelszabadit(szomszedsagiMatrix, V);
	mtxFelszabadit(minSulyuFeszitoFaMatrix, V);
	lancoltListaFelszabadit(koordinataPontok);
	SzomszedsagiListaFelszabadit(graf);
	printf("A program sikeresen lefutott ! A grafok a megadott fajlneven elmentesre kerultek !\n");
	getchar();
	return 0;
}
