#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h> 

typedef struct Produkty {
	int id;
	char nazov[20];
	char vyrobca[15];
	int sklad;
	int cena;
} PRODUKT;

typedef struct Zakaznik {
	char meno[15];
	char priezvisko[15];
	int rozpocet;
	PRODUKT kosik[50];
} ZAKAZNIK;

PRODUKT* nacitaj_subor(char* nazov_suboru) {		//procedura nacitania udajov zo suboru
	FILE* zoznam;
	zoznam = fopen("produkty.txt", "r");
	int pocet,i;

	if (zoznam == NULL) {		//test existencie suboru
		puts("Subor sa neda otvorit.");
	}

	fscanf(zoznam, "%d" ,&pocet);
	
	PRODUKT *produkty;
	produkty = (PRODUKT*)malloc(sizeof(PRODUKT) * (pocet+1));			//dynamicka alokacia pola produktov   ....vytvoreny jeden sample produkt, kde si ulozim pocet produktov
	for (i = 1; i <= pocet; i++) {
		fscanf(zoznam, "%d", &produkty[i].id);
		fscanf(zoznam, "%s", &produkty[i].nazov);
		fscanf(zoznam, "%s", &produkty[i].vyrobca);
		fscanf(zoznam, "%d", &produkty[i].sklad);
		fscanf(zoznam, "%d", &produkty[i].cena);
	}

	produkty[0].id = pocet;		//pocitadlo mnozstva produktov
	fclose(zoznam);
	return(produkty);
} 

void predaj(ZAKAZNIK* osoba, PRODUKT* produkty);

void spoznaj_zakaznika(ZAKAZNIK *osoba, PRODUKT *produkty) {
	char temp_rozpocet[15] = "";
	char pyt[10]="";		//premenna na kontrolu zberu informacii
	int i, dlzka;
	short int chyba = 1;

	puts("Vitajte v obchode.\nTento program o vas zbiera informacie, prajete si pokracovat?\n 1 = ano \t 0 = nie");
	scanf("%s", pyt);
	if (atoi(pyt) == 0) {
		printf("KONIEC\n");
		exit(1);
	}
	else if ( atoi(pyt) == 1) {
		puts("");
	}
	else {
		printf("Nevyjadrenie nesuhlasu povazujeme za suhlas :)\n");
	}
	

	for (i = 1; i <= produkty[0].id; i++) {
		osoba->kosik[i].id = i;
		strcpy(osoba->kosik[i].nazov, produkty[i].nazov);
		strcpy(osoba->kosik[i].vyrobca, produkty[i].vyrobca);
		osoba->kosik[i].sklad = 0;							//vyprazdnenie kosika
		osoba->kosik[i].cena = produkty[i].cena;
	}

	puts("Zadajte vase meno: ");
	scanf("%s", osoba->meno);

	puts("Zadajte vase priezvisko: ");
	scanf("%s", osoba->priezvisko); 

	puts("Zadajte vas rozpocet v celych cislach: ");

	while (chyba == 1) {
		
		scanf("%s", temp_rozpocet);
		dlzka = strlen(temp_rozpocet);
		chyba = 0;

		for (i = 0; i < dlzka; i++) {
			if (isdigit(temp_rozpocet[i]) == 0) chyba=1;
		}

		if (chyba == 1) puts("Neplatny vstup. Zadajte rozpocet v celych cislach.");
	}
	
	osoba->rozpocet = atoi(temp_rozpocet);
	strcpy(pyt, "0");
	
}

void blocek(ZAKAZNIK* osoba, PRODUKT* produkty, int platba) {
	int i;
	FILE* blok;
	blok = fopen("blocek.txt", "w");
	
	if (blok == NULL) {		//kontrola vytvorenia suboru
		puts("Chyba ukladania blocka.");
		exit(1);
	}

	fprintf(blok, "meno: %s \npriezvisko: %s \n\n", osoba->meno, osoba->priezvisko);
	fprintf(blok, "Nakupene produkty: \nid       nazov \t    vyrobca \t zasoba \t cena\n");

	for (i = 1; i <= produkty[0].id; i++) {
		if (osoba->kosik[i].sklad != 0) {
			fprintf(blok ,"%2d  %10s \t %10s \t     %2d \t %4d \n", produkty[i].id, produkty[i].nazov, produkty[i].vyrobca, produkty[i].sklad, produkty[i].cena);
		}
	}
	
	
	fprintf(blok ,"\nhodnota nakupu: %d \ncena bez DPH: %.2f ", platba, (float)platba * 0.8);
	fclose(blok);


}

void vyber_tovaru(ZAKAZNIK* osoba, PRODUKT* produkty, int vysledky[]) {
	int id,i;
	char id_input[20];
	char kusy_input[20];
	int kusy = -1;
	while (0 == 0) {
		puts("Zadajte id ziadaneho tovaru alebo 0 pre hlavnu ponuku:");
		scanf("%s", id_input);		//pre kontrolu zleho inputu
		id = atoi(id_input);		//premeni input na cislo

		if (id == 0) {
			predaj(osoba, produkty);
		}
		else {
			for (i = 1; i <= vysledky[0]; i++) {		//prehladava id najdenych produktov
				if (id == vysledky[i]) {

					if (produkty[id].sklad == 0) {				//kontrola, ci nie je vypredane
						puts("Tovar je vypredany. Navrat do hlavnej ponuky.");
						predaj(osoba, produkty);
					}

					while (kusy < 0) {		//opyta pocet kusov na kupenie
						puts("Zadajte ziadany pocet kusov. Pre navrat do hlavnej ponuky napiste 0.:");
						scanf("%s", kusy_input);
						kusy = atoi(kusy_input);

						if (kusy == 0) {
							predaj(osoba, produkty);
						}

						if (produkty[id].sklad < kusy) {	//kontrola, ci je na sklade dost kusov
							kusy = -1;
							printf("Nedostatok produktov na sklade, zostalo uz len %dks.\n", produkty[id].sklad);
						}

						if ((osoba->kosik[id].cena * kusy) > osoba->rozpocet) {			//kontrola rozpoctu
							printf("Nedostatocny rozpocet, zostava vam uz len %d. Pre navrat do hlavnej ponuky napiste 0.\n", osoba->rozpocet);
							kusy = -1;
						}

					}

					osoba->rozpocet -= osoba->kosik[id].cena * kusy;
					(produkty[id]).sklad -= kusy;
					(osoba->kosik[id]).sklad += kusy;
					printf("Uspesne kupenych %dks produktu %s od vyrobcu %s.\n", kusy, (produkty[id]).nazov, (produkty[id]).vyrobca);
					kusy = -1;
				}
			}
		}
	}

}

void predaj_nazov(ZAKAZNIK *osoba, PRODUKT *produkty) {	

	int nproduktov = produkty[0].id;
	char nproduktu[20];
	int i;
	int vysledky[20];
	vysledky[0] = 0;			//deklaracia pocitadla spravnych vysledkov

	puts("Zadajte nazov hladaneho produktu: ");
	scanf("%s", nproduktu);
	printf("id       nazov \t    vyrobca \t zasoba \t cena\n");
																							
	for (i = 0; i <= nproduktov ; i++) {
		
		if (strcmp(nproduktu, produkty[i].nazov) == 0) {							//porovnanie hladaneho a vysledkov
			printf("%2d  %10s \t %10s \t     %2d \t %4d \n", produkty[i].id, produkty[i].nazov, produkty[i].vyrobca, produkty[i].sklad, produkty[i].cena);
			vysledky[0]++;										//pocet spravnych vysledkov sa zvacsi o 1
			vysledky[vysledky[0]] = i;							//na dane miesto sa zapise id najdeneho produktu
		}
	}

	if (vysledky[0] == 0) {
		puts("Hladany produkt nebol najdeny. Navrat do hlavnej ponuky.");
		predaj(osoba, produkty);
	}

	vyber_tovaru(osoba, produkty, vysledky);

	predaj(osoba, produkty);
}

void predaj_vyrobca(ZAKAZNIK* osoba, PRODUKT* produkty) {

	int nproduktov = produkty[0].id;
	char nvyrobcu[20];
	int i;
	int id = 0;
	int kusy = 0;
	int vysledky[20];
	vysledky[0] = 0;			//deklaracia pocitadla spravnych vysledkov

	puts("Zadajte nazov hladaneho vyrobcu: ");
	scanf("%s", nvyrobcu);
	printf("id       nazov \t    vyrobca \t zasoba \t cena\n");

	for (i = 0; i <= nproduktov; i++) {

		if (strcmp(nvyrobcu, produkty[i].vyrobca) == 0) {							//porovnanie hladaneho a vysledkov
			printf("%2d  %10s \t %10s \t     %2d \t %4d \n", produkty[i].id, produkty[i].nazov, produkty[i].vyrobca, produkty[i].sklad, produkty[i].cena);
			vysledky[0]++;										//pocet spravnych vysledkov sa zvacsi o 1
			vysledky[vysledky[0]] = i;							//na dane miesto sa zapise id najdeneho produktu
		}
	}

	if (vysledky[0] == 0) {
		puts("Hladany vyrobca nebol najdeny. Navrat do hlavnej ponuky.");
		predaj(osoba, produkty);
	}

	vyber_tovaru(osoba, produkty, vysledky);

	predaj(osoba, produkty);
}

void predaj(ZAKAZNIK* osoba, PRODUKT* produkty) {
	char volba_input[20];
	int volba = 0;
	puts("");
	puts("################################################################");
	printf("Vazeny/a %s %s, vitajte v obchode. Vas zvysny rozpocet je: %d \n", osoba->meno, osoba->priezvisko, osoba->rozpocet);
	printf("Ponuka sluzieb: \n");
	puts("  1. Vyhladavanie produktov podla nazvu \n  2. Vyhladavanie produktov podla vyrobcu \n  3. Ukoncit nakup a opustit obchod");
	
	if (osoba->rozpocet == 0) {				//kontrola vycerpania rozpoctu
		volba = '3';
		puts("Rozpocet bol vycerpany.");
	}

	while (volba < 1 || volba > 3) {		//kontrola vstupu
		printf("Zadajte vasu volbu: ");
		scanf("%s", volba_input);
		volba = atoi(volba_input);
	}
	
	if (volba == 1) {
		predaj_nazov(osoba, produkty);
	}
	else if(volba == 2) {
		predaj_vyrobca(osoba, produkty);
	}
	else if(volba == 3){
		int zaplatit = 0;
		int i;
		for (i = 1; i <= 13 ; i++){
			zaplatit += (osoba->kosik[i].cena * osoba->kosik[i].sklad);
		}
		printf("Dakujeme za nakup. Nakupili ste produkty v hodnote %d.\n", zaplatit);
		blocek(osoba, produkty, zaplatit);
		free(produkty);
		exit(0);
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx//

int main() {
	ZAKAZNIK osoba;
	ZAKAZNIK *posoba = &osoba;

	PRODUKT* produkty = nacitaj_subor("produkty.txt");
	spoznaj_zakaznika(posoba, produkty);
	puts("Aspon sem?");
	
	predaj(posoba, produkty);
}