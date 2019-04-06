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

// struktúra, - szomszédsági lista egy pontja
// maga célpont és az élhez tartozó súly tárolása
struct ListaPontja {
	int celpont;
	int suly;
	struct ListaPontja* next;
};
// Struktúra szomszédsági lista

struct Lista {
	struct ListaPontja* head;
};

// Ez a struktúra egy gráfot reprezentál, a gráf egy szomszédsági lista tömb
// A tömb mérete: V, vagyis a gráf csúcsainak száma.
struct Graf {
	int V;
	struct Lista* tomb;
};

//láncolt lista adatszerkezet a megjelnítendõ pontok koordinátái és azonosítóival
struct Pontok {
	int id;
	double x;
	double y;
	struct Pontok* kovetkezo;
};

// lista pontjainak helyfoglalása és a célpont, súly tárolása
struct ListaPontja* ujIlleszkedesiListaPontja(int celpont, int suly)
{
	struct ListaPontja* ujPont =
		(struct ListaPontja*)malloc(sizeof(struct ListaPontja));
	ujPont->celpont = celpont; // célont hozzáadása
	ujPont->suly = suly; // súly hozzáadása az élhez.
	ujPont->next = NULL;
	return ujPont;
}
//V pontú gráfnak helyfoglalás
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
// Új él hozzáadása forras-bõl celpont-ba. Az új csúcs hozzáadódik a szomszédsági listához
void elHozzaad(struct Graf* graf, int forras, int celpont, int suly)
{
	struct ListaPontja* ujPont = ujIlleszkedesiListaPontja(celpont, suly);
	ujPont->next = graf->tomb[forras].head;
	graf->tomb[forras].head = ujPont;
	ujPont = ujIlleszkedesiListaPontja(forras, suly);
	ujPont->next = graf->tomb[celpont].head;
	graf->tomb[celpont].head = ujPont;
}
// segédfüggvény a gráf elemeinek bejárásához, kiiratásához a programban a függvényt nem használom
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
// a szomszédsági mátrixot feltölti pozíció helyesen. A pontok között futó élek súlyával
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
// mátrix kiiratása nxm-es mátrix kiiratására képes a függvény
void mtxKiir(int** tomb, int sor, int oszlop)
{
	int i, j;
	for (i = 0; i < sor; ++i) {
		for (j = 0; j < oszlop; ++j)
			printf("%d\t", tomb[i][j]);
		printf("\n\n");
	}
}
// nxn-es dinamikusan foglalt mátrix memóriáját felszabadító  függvény
void mtxFelszabadit(int** dinMtx, int V)
{
	int i;
	for (i = 0; i < V; i++) {
		free(*(dinMtx + i));
	}
	free(dinMtx);
}
// a prím algoritmusba integrált ujMatrixbaMasol függvény
//V itt a gráf csúcsainak számát jeletni
void ujMatrixbaMasol(int szulo[], int** szomszedsagiMtx, int** minSulyuFeszFaMTX, int V)
{
	int i;
	for (i = 1; i < V; i++) {
		minSulyuFeszFaMTX[i][szulo[i]] = szomszedsagiMtx[i][szulo[i]];
	}
}
// INT_MAX Az integer maximális értéke, ehhez viszonyítva van meghatározva a minimum érték.
// Az algoritmus mindig hozzávesz egy pontot a már meglévõ fához, ez a függvény a bejárható
//élek közül kiválasztja a legkisebbet
// minimumKulcs algoritmus kiválasztja a 0-val kapcsolatban lévõ pontok közül azt, amelyiknek a legkisebb a súlya
// eredményül annak a pontnak a számát adja ahol minimális az él
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
	int *kulcs = (int *)malloc(sizeof(int) * V);// kulcs tartalmazza a minimális súlyokat
	bool *minSulyuFeszFaBeallit = (bool *)malloc(sizeof(bool) * V);
	// inicializálódik a kulcs és a minsulyuFeszFaBeallit tomb
	for (i = 0; i < V; i++){
		kulcs[i] = INT_MAX, minSulyuFeszFaBeallit[i] = false;
	}
	kulcs[0] = 0; // 0 pont lesz az elsõ elem ami a fában benne lesz, kiinduló pont
	szulo[0] = -1;
	// a ciklus V-1 ig megy, mert az élek száma minimális súlyú feszíta esetében
	//mindig a pontok száma-1
	for (szamolo = 0; szamolo < V - 1; szamolo++) {
		// u értéke egyenlõ a meglévõ fához kapcsolódó legkisebb súlyú élhez tartozó ponttal
		u = minimumKulcs(kulcs, minSulyuFeszFaBeallit, V);
		//a tömb u-adik eleme hamisról igazra változik, tehát bekerül a pont a fába
		minSulyuFeszFaBeallit[u] = true;
		// Kulcs értékek frissítése
		for (v = 0; v < V; v++) {
			//minSulyuFeszFaBeallit[v] eleme ha false akkor nincsen benne a fában még.
			//Kulcsérték frissítése, ha a mátrix[u][v] eleme kisebb mint a kulcs[v] eleme
			if (szomszedsagiMtx[u][v] && minSulyuFeszFaBeallit[v] == false && szomszedsagiMtx[u][v] < kulcs[v]){
				szulo[v] = u;
				kulcs[v] = szomszedsagiMtx[u][v];
			}
		}
	}
	// átmásolódik a mátrix egy új mátrixba pozícióhelyesen
	ujMatrixbaMasol(szulo, szomszedsagiMtx, minSulyuFeszFaMTX, V);
	free(szulo);
	free(kulcs);
	free(minSulyuFeszFaBeallit);
}

void ListaFelszabadit(struct Lista lista){
	struct ListaPontja *tmp, *p = lista.head;
	while (p != NULL){ //pont felszabadítása NULL ig
		tmp = p->next;
		free(p);
		p = tmp;
	}
}
// Szomszédsági lista tömb felszabadítása
void SzomszedsagiListaFelszabadit(struct Graf* szomsz_lista){
	int i;
	for (i = 0; i < szomsz_lista->V; i++) //felszabadít minden egyes láncolt listát
		ListaFelszabadit(szomsz_lista->tomb[i]);
	free(szomsz_lista->tomb); //felszabadítja a láncolt lista tömböt
	free(szomsz_lista); //felszabadítja a szomszédsági listát
}
// lista elemeinek kiiratása ellenõrzés céljából
void listaKiir(struct Pontok** lista)
{
	struct Pontok* mutato = *lista;
	while (mutato != NULL) {
		printf("\n id: [%d] x:[%f] \t y:[%f] \n", mutato->id, mutato->x, mutato->y);
		mutato = mutato->kovetkezo;
	}
}
//láncolt lista végére fûzi az azonosítót, x és y koordinátát a függvény
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

// id alapján keresés, a gráf pontjaihoz tartozó koordináták megkeresése
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

//radián - fok konverter
//a szögfüggvényekhez való átváltáshoz
double radianKonvert(double atvaltando)
{
	atvaltando = atvaltando * PI / 180;
	return atvaltando;
}
// kiszamolja adott sugárral és adott osztási fokban az adott poziciojú koorinátát
//pl  sugár fix 400, osztofokban pedig a fok száma pl 360/5 esetén 72 fok 72. fokra fogja kiszámatni az 1. pontot
// következõben pedig 142 fok-ra számítja ki ... persze adagolni kell neki a szögeket. és megmondja hogy mi lesz a koordináta
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
// elkesziti a koorinatakat és belemasolja egy listaba az elemeket
// listavegere füz fv a láncolt lista végére fûzi az x,y koordinátát és az ID-t is ami itt i változó.

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
// a fájl elsõ sorából kiolvassa a gráf pontjainak számát
int vDarab(FILE* be){
	char *token;
	char buffer[5];
	int darab;
	fgets(buffer, 5, be);
	token = strtok(buffer, "");
	darab = atoi(token);
	return darab;
}
// beolvassa a függvény a bemeneti fájlból soronként, "darabolással", tabulátor elválasztással a Szomszédsági lista tömbbe az elemeket.
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
		// élhozzáad fv. a pontok közötti kapcsolatot reprezentálja a fv, az élek súlyait adja hozzá
		elHozzaad(graf, honnet, hova, suly);
	}
}
// a függvény létrehozza az SVG fájl TAG-címkéit és felparaméterezi õket a megfelelõ koordinátával
// létrehozza a circle TAG-eket,amik a pontok és mátrix alapján meghatározza, hogy melyik két pont között kell
// létrehozni line TAG-et, text TAG segítségével pedig feliratozza az éleket, kör alakzatokat, így alakítva ki a gráfot.

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
// felszabadítja a láncolt lista adatszerekezetet, melyben a pontok vannak tárolva
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
	// gráf mutató  - elkészül a V pontú gráf  - szomszédsági lista, ami egy gráfot reprezentál
	struct Graf* graf;
	// mtx ami a listából kerül kialakításra
	int** szomszedsagiMatrix;
	// prim algoritmus lefuttatása után a szomszédsági mátrix, vagyis a min sulyu fesz fa sulyai poziciohelyesen itt talalhatoak.
	int** minSulyuFeszitoFaMatrix;
	// megjelenítéshez használt láncolt lista: ID-t 0-tól V-1-ig, 2 darab double-t tartalmaz x, és y koordinátát kör alakban történik
	// a pontok elrendezése
	struct Pontok* koordinataPontok = NULL;
	//getline függvény ideiglenes tároló tömbje
	char beker[MAXMERET + 1];
	// a módosított fájlnevet tartalmazó tömb
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
				// sikeres fájlbeolvasás esetén V, mint globális változó értékadása itt történik meg
				// A vDarab függvény a beolvasott fájlból, ha az létezik az elsõ sorában található számot kiolvassa, számmá alakítja
				// és értékül adja V-nek
				V = vDarab(be);
				//grafkesztit fügvény lefoglalja a memóriát V pontú gráfnak, a struktúra, amiben a gráf pontjai megtalálhatóak
				graf = grafKeszit(V);
				// fajlbeolvas függveny a fajl tartalmat beolvassa az elsõ sor kivételével, beolvasás az utolsó sorig megy
				// a fájl tartalma rendre: honnét, hova, mennyivel. A gráf kapcsolatait jellemzi a beolvasás soronként valósul meg
				// ezzel egyidejûleg a sorokból kiolvasott értéket az elhozzaad függvény fogja a gráfba rögzíteni.
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
					// megadott fájlnévhez a kiterjesztés hozzáadása és az _eredet, _modositott szavak hozzáadása
					strcpy(ideiglenesBeker, beker);
					strcat(beker, "_eredeti.svg");
					strcat(ideiglenesBeker, "_modositott.svg");
					// szomszedsagi mtx elemeinek feltöltése és memoria foglalasa
					szomszedsagiMtxFeltolt(dinMtxFoglal(&szomszedsagiMatrix, V, V), graf);
					// prím algoritmus futtatása a szomszédsági mátrixon és a végeredmény elkészítése dinamikusan foglalt mátrixba
					// a végeredmény már a minimális súlyú feszítõfa súlyainak számát fogja tartalmazni.
					// egy nem nulla súlyú él pozíciója a mátrixban azt is jelenti, hogy melyik két pont között megy az él
					primAlg(szomszedsagiMatrix, dinMtxFoglal(&minSulyuFeszitoFaMatrix, V, V), V);
					//  koordinata keszit fv elkészíti és láncolt listába teszi R sugarú és n pontból álló gráf koordinátáit
					koordinataKeszit(&koordinataPontok, SUGAR, V);
					// kimeneti fájlok létrehozása a megadott fájlnévvel
					// fajlbaIras függvény vár egy FILE típusú pointer-t, egy struktúrát, egy mátrixot és a gráf struktúráját.
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
	// memória felszabadítása
	mtxFelszabadit(szomszedsagiMatrix, V);
	mtxFelszabadit(minSulyuFeszitoFaMatrix, V);
	lancoltListaFelszabadit(koordinataPontok);
	SzomszedsagiListaFelszabadit(graf);
	printf("A program sikeresen lefutott ! A grafok a megadott fajlneven elmentesre kerultek !\n");
	getchar();
	return 0;
}
