#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

// Struktura danych
typedef struct {
    int rozmiar;
    int ilosc_min;
    int liczba_ruchow;
    char** widoczna; // Tablica dynamiczna 
    char** ukryta; // Tablica dynamiczna
} Gra;

char** alokuj_macierz(int rozmiar) {
    // calloc
    char** plansza = (char**)calloc(rozmiar, sizeof(char*));
    if (rozmiar <= 0) 
    {
        return NULL;
    }
    if (plansza == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < rozmiar; i++) 
    {
        plansza[i] = (char*)calloc(rozmiar, sizeof(char));
        if (plansza[i] == NULL) 
        {
            for (int j = 0; j < i; j++)
            {
                free(plansza[j]);
            }
            free(plansza);
            return NULL;
        }
    }
    return plansza;
}

void inicjalizuj_plansze_w_grze(Gra* g) {
    if (g->rozmiar <= 0)
    {
        return;
    }
    if (g->widoczna == NULL)
    {
        g->widoczna = alokuj_macierz(g->rozmiar);
    }
    if (g->ukryta == NULL)
    {
        g->ukryta = alokuj_macierz(g->rozmiar);
    }

    for (int i = 0; i < g->rozmiar; i++) 
    {
        for (int j = 0; j < g->rozmiar; j++) 
        {
            g->widoczna[i][j] = '#';
            g->ukryta[i][j] = '.';
        }
    }
    g->liczba_ruchow = 0;
}

void wyczysc_pamiec_gry(Gra* g) {
    if (g->widoczna != NULL) 
    {
        for (int i = 0; i < g->rozmiar; i++) 
        {
            free(g->widoczna[i]);
        }
        free(g->widoczna);
        g->widoczna = NULL;
    }
    if (g->ukryta != NULL) 
    {
        for (int i = 0; i < g->rozmiar; i++)
        {
            free(g->ukryta[i]);
        }
        free(g->ukryta);
        g->ukryta = NULL;
    }
    g->rozmiar = 0;
    g->liczba_ruchow = 0;
}

// Przeciążenie funkcji
void wyswietl_info(int rozmiar, int ilosc_min) {
    SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("Rozmiar: %dx%d | Miny: %d\n", rozmiar, rozmiar, ilosc_min);
    SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

// Przeciążenie funkcji
void wyswietl_info(int pozostale_miny, double czas_gry, int ruchy) {
    SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf(" Miny: %d | Czas: %.1f s | Ruchy: %d \n", pozostale_miny, czas_gry, ruchy);
    SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

int menu(bool przerwac_gre) {
    int liczba_z_manu = 0;

    // Tablica statyczna dwuwymiarowa
    char opcje_menu[6][50] = {
        "1. Rozpocznij nowa gre",
        "2. Wczytaj zapisana gre",
        "3. Instrukcja gry",
        "4. Zakoncz",
        "5. Zapisz poprzednia gre",
        "6. Kontynuuj poprzednia gre"
    };

    do {
        system("cls");
        SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        printf("============================================\n");
        printf(" ____        _       ____     _____   ____    \n");
        printf("/ ___|      / \\     |  _ \\   | ____| |  _ \\   \n");
        printf("\\___ \\     / _ \\    | |_) |  |  _|   | |_) | \n");
        printf(" ___) |   / ___ \\   |  __/   | |___  |  _ <   \n");
        printf("|____/   /_/   \\_\\  |_|      |_____| |_| \\_\\ \n");
        printf("============================================\n\n");
        SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        for (int i = 0; i < 4; i++)
        {
            printf("%s\n", opcje_menu[i]);
        }
        if (przerwac_gre) 
        {
            printf("%s\n", opcje_menu[4]);
            printf("%s\n", opcje_menu[5]);
        }

        printf("\nWybierz opcje: ");
        if (scanf_s("%d", &liczba_z_manu) != 1)
        {
            system("cls");
            printf("To nie jest liczba.\n");
            while (getchar() != '\n');
            system("pause");
            continue;
        }

        if (przerwac_gre) 
        {
            if (liczba_z_manu >= 1 && liczba_z_manu <= 6)
            {
                break;
            }
        }
        else 
        {
            if (liczba_z_manu >= 1 && liczba_z_manu <= 4)
            {
                break;
            }
        }
    } while (true);
    return liczba_z_manu;
}

float oblicz_procent_odkrycia(Gra* g) {
    int odkryte = 0;
    int bezpieczne_pola = (g->rozmiar * g->rozmiar) - g->ilosc_min;

    for (int i = 0; i < g->rozmiar; i++) 
    {
        for (int j = 0; j < g->rozmiar; j++) 
        {
            if (g->widoczna[i][j] != '#' && g->widoczna[i][j] != 'F' && g->widoczna[i][j] != '*') 
            {
                odkryte++;
            }
        }
    }
    if (bezpieczne_pola == 0)
    {
        return 0.0f;
    }
    return ((float)odkryte / bezpieczne_pola) * 100.0f;
}

void zapisz_gre(Gra* g) {
    char nazwa_pliku[100]; // Tablica statyczna jednowymiarowa
    FILE* f;

    printf("Podaj nazwe pliku: ");
    if (scanf_s("%99s", nazwa_pliku, (unsigned)sizeof(nazwa_pliku)) != 1)
    {
        return;
    }
    if (fopen_s(&f, nazwa_pliku, "w") != 0) 
    {
        printf("Nie mozna otworzyc pliku do zapisu!\n");
        return;
    }

    fprintf(f, "%d %d %d\n", g->rozmiar, g->ilosc_min, g->liczba_ruchow);

    for (int a = 0; a < g->rozmiar; a++) 
    {
        for (int b = 0; b < g->rozmiar; b++)
        {
            fprintf(f, "%c", g->widoczna[a][b]);
        }
        fprintf(f, "\n");
    }

    fprintf(f, "\n");
    for (int a = 0; a < g->rozmiar; a++) 
    {
        for (int b = 0; b < g->rozmiar; b++)
        {
            fprintf(f, "%c", g->ukryta[a][b]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    printf("Gra zostala zapisana.\n");
}

bool wczytaj_gre(Gra* g) {
    char nazwa_pliku[100];
    int r = 0;
    int m = 0;
    int wczytane_ruchy = 0;
    FILE* f;

    printf("Podaj nazwe pliku: ");
    if (scanf_s("%99s", nazwa_pliku, (unsigned)sizeof(nazwa_pliku)) != 1)
    {
        return false;
    }
    if (fopen_s(&f, nazwa_pliku, "r") != 0) 
    {
        printf("Nie mozna otworzyc pliku do odczytu!\n");
        return false;
    }

    if (fscanf_s(f, "%d %d %d", &r, &m, &wczytane_ruchy) != 3) 
    {
        printf("Blad formatu pliku.\n");
        fclose(f);
        return false;
    }

    wyczysc_pamiec_gry(g);
    g->rozmiar = r;
    g->ilosc_min = m;
    g->liczba_ruchow = wczytane_ruchy;

    inicjalizuj_plansze_w_grze(g);

    for (int a = 0; a < g->rozmiar; a++) 
    {
        for (int b = 0; b < g->rozmiar; b++)
        {
            fscanf_s(f, " %c", &g->widoczna[a][b], 1);
        }
    }
    for (int a = 0; a < g->rozmiar; a++) 
    {
        for (int b = 0; b < g->rozmiar; b++)
        {
            fscanf_s(f, " %c", &g->ukryta[a][b], 1);
        }
    }

    fclose(f);
    printf("Gra zostala wczytana. Ruchy: %d\n", g->liczba_ruchow);
    return true;
}

void zasady() {
    system("cls");
    printf("ZASADY GRY\n");
    printf("---------------------------------------------------\n");
    printf("# - pole zakryte | F - flaga | . - puste | * - mina\n\n");
    printf("1. Celem gry jest odkrycie wszystkich bezpiecznych pol.\n");
    printf("2. Odkrycie pola z mina konczy gre przegrana.\n");
    printf("3. Liczba na polu oznacza ile min jest wokol niego.\n");
    printf("\n");
    system("pause");
}

void wyswietl_plansze(int wiersz, int kolumna, Gra* g) {
    char pole;
    if (g->rozmiar == 0)
    {
        return;
    }

    for (int b = 0; b < g->rozmiar * 2 + 1; b++)
    {
        printf("%c", 205);
    }
    printf("\n");
    for (int a = 0; a < g->rozmiar; a++) 
    {
        for (int b = 0; b < g->rozmiar; b++)
        {
            SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            printf("%c", 186);

            if (a == wiersz && b == kolumna)
            {
                SetConsoleTextAttribute(h, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
            }
            else 
            {
                pole = g->widoczna[a][b];
                if (pole == '1') SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                else if (pole == '2') SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                else if (pole == '3') SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY);
                else if (pole == 'F') SetConsoleTextAttribute(h, FOREGROUND_RED);
                else if (pole == '*') SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY);
                else SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            printf("%c", g->widoczna[a][b]);
        }
        SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        printf("%c\n", 186);
    }
    for (int b = 0; b < g->rozmiar * 2 + 1; b++)
    {
        printf("%c", 205);
    }
    printf("\n");
}

int policz_miny(Gra* g, int x, int y) {
    int ilosc = 0;
    for (int a = -1; a <= 1; a++) 
    {
        for (int b = -1; b <= 1; b++) 
        {
            int ny = x + a;
            int nx = y + b;
            if (ny >= 0 && ny < g->rozmiar && nx >= 0 && nx < g->rozmiar && g->ukryta[ny][nx] == '*') 
            {
                ilosc++;
            }
        }
    }
    return ilosc;
}

int policz_flagi(Gra* g) {
    int flagi = 0;
    for (int i = 0; i < g->rozmiar; i++)
    {
        for (int j = 0; j < g->rozmiar; j++)
        {
            if (g->widoczna[i][j] == 'F')
            {
                flagi++;
            }
        }
    }
    return flagi;
}

void odkryj_puste(int wiersz, int kolumna, Gra* g) {
    if (wiersz < 0 || wiersz >= g->rozmiar || kolumna < 0 || kolumna >= g->rozmiar)
    {
        return;
    }
    if (g->widoczna[wiersz][kolumna] != '#' && g->widoczna[wiersz][kolumna] != 'F')
    {
        return;
    }

    g->widoczna[wiersz][kolumna] = g->ukryta[wiersz][kolumna];

    if (g->widoczna[wiersz][kolumna] == '.')
    {
        for (int a = -1; a <= 1; a++)
        {
            for (int b = -1; b <= 1; b++)
            {
                if (a == 0 && b == 0)
                {
                    continue;
                }
                odkryj_puste(wiersz + a, kolumna + b, g);
            }
        }
    }
}

void generuj_plansze(Gra* g, int pierwszy_wiersz, int pierwsza_kolumna) {
    int w, k;
    for (int i = 0; i < g->ilosc_min; i++)
    {
        w = rand() % g->rozmiar;
        k = rand() % g->rozmiar;

        if (g->ukryta[w][k] != '*' && (abs(w - pierwszy_wiersz) >= 1 && abs(k - pierwsza_kolumna) >= 1)) 
        {
            g->ukryta[w][k] = '*';
        }
        else
        {
            i--;
        }
    }

    for (int x = 0; x < g->rozmiar; x++)
    {
        for (int y = 0; y < g->rozmiar; y++)
        {
            if (g->ukryta[x][y] != '*' && policz_miny(g, x, y) > 0)
            {
                g->ukryta[x][y] = '0' + policz_miny(g, x, y);
            }
        }
    }
}

bool czy_wygrana(Gra* g) {
    for (int a = 0; a < g->rozmiar; a++) 
    {
        for (int b = 0; b < g->rozmiar; b++) 
        {
            if (g->ukryta[a][b] != '*' && (g->widoczna[a][b] == '#' || g->widoczna[a][b] == 'F'))
            {
                return false;
            }
        }
    }
    return true;
}

int main() {
    int wiersz = 0, kolumna = 0, poziom_trudnosci = 0;
    int* rekord_wierszu = &wiersz;
    printf("Wartość: %d", rekord_wierszu);
    
    system("pause");
    /*
    bool pierwszy_ruch = true;
    bool wejsc_do_menu = false;
    bool przerwac_gre = false;
    double start_czasu = 0;

    Gra gra = { 0, 0, 0, NULL, NULL };

    srand((unsigned int)time(NULL));

    do {
        wejsc_do_menu = false;
        switch (menu(przerwac_gre)) 
        {
        case 1:
            pierwszy_ruch = true;
            wiersz = 0;
            kolumna = 0;

            do {
                system("cls");
                printf("Wybierz poziom trudnosci:\n1 - 8x8 (10 min)\n2 - 10x10 (15 min)\n3 - 15x15 (30 min)\nPodaj numer: ");
                if (scanf_s("%d", &poziom_trudnosci) != 1) 
                {
                    while (getchar() != '\n'); continue;
                }
                if (poziom_trudnosci >= 1 && poziom_trudnosci <= 3)
                {
                    break;
                }
            } while (true);

            wyczysc_pamiec_gry(&gra);

            if (poziom_trudnosci == 1) 
            { 
                gra.rozmiar = 8; gra.ilosc_min = 10; 
            }
            else if (poziom_trudnosci == 2) 
            {
                gra.rozmiar = 10; gra.ilosc_min = 15; 
            }
            else 
            {
                gra.rozmiar = 15; gra.ilosc_min = 30; 
            }

            inicjalizuj_plansze_w_grze(&gra);
            przerwac_gre = true;
            break;

        case 2:
            system("cls");
            if (wczytaj_gre(&gra)) 
            {
                pierwszy_ruch = false;
                start_czasu = (double)clock();
                przerwac_gre = true;
            }
            else
            {
                system("pause");
                continue;
            }
            break;

        case 3: 
            zasady(); 
            wejsc_do_menu = true;  
            break;
        case 4:
            wyczysc_pamiec_gry(&gra);
            system("pause"); 
            exit(0);
        case 5:
            system("cls");
            if (gra.widoczna != NULL)
            {
                zapisz_gre(&gra);
            }
            else
            {
                printf("Brak gry do zapisu.\n");
            }
            system("pause"); break;
        case 6:
            if (gra.rozmiar == 0)
            {
                continue;
            }
            break;
        default: 
            continue;
        }

        while (!wejsc_do_menu) 
        {
            system("cls");
            if (!pierwszy_ruch)
            {
                wyswietl_info(gra.ilosc_min - policz_flagi(&gra), (double)(clock() - start_czasu) / CLOCKS_PER_SEC, gra.liczba_ruchow);
            }
            else
            {
                wyswietl_info(gra.rozmiar, gra.ilosc_min);
            }
            wyswietl_plansze(wiersz, kolumna, &gra);
            printf("\n[W,S,A,D] - Ruch | [O] - Odkryj | [F] - Flaga | [M] - Menu\n");
            printf("Wybierz akcje: ");

            switch (_getch()) 
            {
            case 'w': 
                if (wiersz > 0)
                {
                    wiersz--; 
                    break;
                }
            case 'a': 
                if (kolumna > 0)
                {
                    kolumna--; 
                    break;
                }
            case 's': 
                if (wiersz < gra.rozmiar - 1)
                {
                    wiersz++;
                    break;
                }
            case 'd': 
                if (kolumna < gra.rozmiar - 1)
                {
                    kolumna++;
                    break;
                }
            case 'o':
                if (pierwszy_ruch) 
                {
                    start_czasu = (double)clock();
                    generuj_plansze(&gra, wiersz, kolumna);
                    pierwszy_ruch = false;
                }

                if (gra.widoczna[wiersz][kolumna] != 'F' && gra.widoczna[wiersz][kolumna] == '#') 
                {
                    gra.liczba_ruchow++;
                    if (gra.ukryta[wiersz][kolumna] == '*')
                    {
                        for (int i = 0; i < gra.rozmiar; i++)
                        {
                            for (int j = 0; j < gra.rozmiar; j++)
                            {
                                if (gra.ukryta[i][j] == '*')
                                {
                                    gra.widoczna[i][j] = '*';
                                }
                            }
                        }
                        system("cls");
                        wyswietl_info(gra.rozmiar, (double)(clock() - start_czasu) / CLOCKS_PER_SEC, gra.liczba_ruchow);
                        wyswietl_plansze(wiersz, kolumna, &gra);
                        printf("\nBUM! Trafiles na mine!\n");
                        printf("Odkryto planszy: %.1f%%\n", oblicz_procent_odkrycia(&gra));
                        system("pause");
                        wyczysc_pamiec_gry(&gra);
                        przerwac_gre = false;
                        wejsc_do_menu = true;
                        break;
                    }
                    odkryj_puste(wiersz, kolumna, &gra);
                }
                break;
            case 'f':
                if (gra.widoczna[wiersz][kolumna] == 'F')
                {
                    gra.widoczna[wiersz][kolumna] = '#';
                }
                else if (gra.widoczna[wiersz][kolumna] == '#')
                {
                    gra.widoczna[wiersz][kolumna] = 'F';
                }
                break;
            case 'm':
                wejsc_do_menu = true;
                przerwac_gre = true;
                break;
            }

            if (wejsc_do_menu)
            {
                break;
            }

            if (czy_wygrana(&gra)) 
            {
                system("cls");
                wyswietl_plansze(wiersz, kolumna, &gra);
                printf("\nGRATULACJE! Wygrales!\n");
                printf("Czas: %.1f s | Ruchy: %d\n", (double)(clock() - start_czasu) / CLOCKS_PER_SEC, gra.liczba_ruchow);
                system("pause");
                wyczysc_pamiec_gry(&gra);
                przerwac_gre = false;
                wejsc_do_menu = true;
            }
        }
    } while (true);
    */
    return 0;
}