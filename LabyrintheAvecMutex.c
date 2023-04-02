#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LIGNES 15
#define COLONNES 15
#define MUR 0
#define CASE_LIBRE 1
#define ENTREE 2
#define SORTIE 3
#define CASE_VISITEE -1

// Prototypes de fonctions
int** setAllocationCarre();
void initialisationMatrice(int **m);
void lectureFichier(int **m);
void affichageMatrice(int **m);
void rechercheEntree(int **m, int* entreeXY);
void *trouverSortieAvecMutex(void *params);
void desallocationMatrice(int **m);

//Allocation dynamique de la matrice
int** setAllocationCarre(){
    int **m = malloc(LIGNES * sizeof(int*));
    for(int i = 0; i < LIGNES; ++i){
        m[i] = malloc(COLONNES * sizeof(int));
    }
    return m;
}
//Initialisation de la matrice
void initialisationMatrice(int **m){
    for(int i = 0; i < LIGNES; i++){
        for(int j = 0; j < COLONNES; j++){
            m[i][j] = 0;
        }
    }
}
//Affichage de la matrice
void affichageMatrice(int **m){
    for (int i = 0; i < LIGNES; ++i){
        for (int j = 0; j < COLONNES; ++j){
            printf("%4d", m[i][j]);
        }
        printf("\n");
    }
}
//Methode pour lire le fichier qui contient le labyrinthe
//Au lieu d'avoir une matrice qui contient des elements char ils deviennent des entiers.
void lectureFichier(int **m){
    FILE* fichier = NULL;
    char caractere;
    fichier = fopen("fichierLabyrinthe.txt","r");
    if(fichier == NULL){
        printf("Le fichier est encore fermee\n");
    }
    while(! feof(fichier)){
        for (int i = 0; i < LIGNES; i++){
            for (int j = 0; j < COLONNES; j++){
                //Fonction getc pour lire le fichier caratere par caractere.
                caractere = fgetc(fichier);
                if(caractere == '0'){
                    m[i][j] = MUR;
                } else if(caractere == '1'){
                    m[i][j] = CASE_LIBRE;
                } else if(caractere == '2'){
                    m[i][j] = ENTREE;
                } else if(caractere == '3'){
                    m[i][j] = SORTIE;
                }
            }
            fgetc(fichier);
        }
    }
    fclose(fichier);
}
//Desallocation de la matrice.
void desallocationMatrice(int **m){
    for (int i = 0; i < LIGNES; ++i){
        free(m[i]);
    }
    free(m);
}
//Methode pour trouver l entrée du labyrinthe
void rechercheEntree(int **m, int* argentree) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            if (m[i][j] == ENTREE) {
                argentree[0] = i;
                argentree[1] = j;
            }
        }
    }
}
struct Coordonnees{
    int x1;
    int y1;
    int **m;
};

//Initialisation du mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *trouverSortieAvecMutex(void *params) {
    pthread_mutex_lock(&mutex);
    //A l'aide du cast on convertit le pointeur params (qui est en void) en pointeur de type Coordonnees
    struct Coordonnees *coord = (struct Coordonnees*) params;
    int x = coord->x1;
    int y = coord->y1;
    int **m = coord->m;

    // On vérifie si la case actuelle est la sortie du labyrinthe.
    if (m[x][y] == SORTIE) {
        printf("Felicitations, sortie trouvee !!\n");
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
    }

    // On affecte -1 à la case actuelle.
    m[x][y] = CASE_VISITEE;

    //Creation des pthread
    pthread_t threads[4];
    int nbthreads = 0;
    //On vérifie si la case au dessus est disponible.
    if (x > 0 && (m[x-1][y] == CASE_LIBRE || m[x-1][y] == SORTIE) && m[x-1][y] != CASE_VISITEE && m[x-1][y] != MUR) {
        struct Coordonnees ligneAvant = {x-1, y, m};
        pthread_create(&threads[nbthreads], NULL, trouverSortieAvecMutex, &ligneAvant);
        nbthreads++;
    }
    //On vérifie si la case en dessous est disponible.
    if (x < LIGNES-1 && (m[x+1][y] == CASE_LIBRE || m[x+1][y] == SORTIE) && m[x+1][y] != CASE_VISITEE && m[x+1][y] != MUR) {
        struct Coordonnees ligneApres = {x+1, y, m};
        pthread_create(&threads[nbthreads], NULL, trouverSortieAvecMutex, &ligneApres);
        nbthreads++;
    }
    //On vérifie si la case à gauche est disponible.
    if (y > 0 && (m[x][y-1] == CASE_LIBRE || m[x][y-1] == SORTIE) && m[x][y-1] != CASE_VISITEE && m[x][y-1] != MUR) {
        struct Coordonnees colAvant = {x, y-1, m};
        pthread_create(&threads[nbthreads], NULL, trouverSortieAvecMutex, &colAvant);
        nbthreads++;
    }
    //On vérifie si la case à droite est disponible.
    if (y < COLONNES-1 && (m[x][y+1] == CASE_LIBRE || m[x][y+1] == SORTIE) && m[x][y+1] != CASE_VISITEE && m[x][y+1] != MUR) {
        struct Coordonnees colApres = {x, y+1, m};
        pthread_create(&threads[nbthreads], NULL, trouverSortieAvecMutex, &colApres);
        nbthreads++;
    }
    pthread_mutex_unlock(&mutex);
    //On attends la fin des threads
    for (int i = 0; i < nbthreads; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_exit(NULL);
}
int main(){
    int **matrice;
    // Creation d'un thread pour exécuter la fonction trouverSortieMultiThread
    pthread_t thread;

    //Allocation de la matrice
    matrice = setAllocationCarre();

    //Initialisation de la matrice
    initialisationMatrice(matrice);

    //Lecture du fichier où se trouve le labyrinthe
    lectureFichier(matrice);

    //Affichage de la matrice
    affichageMatrice(matrice);

    int argentree[2];
    //Les coordonnées de depart du labyrinthe
    rechercheEntree(matrice, argentree);

    // Creation de la structure Coordonnees avec les coordonnées de départ (case qui contient le numero 2) et la matrice du labyrinthe
    struct Coordonnees arguments = {argentree[0], argentree[1], matrice};

    //Utilisation de la fonction trouverSortie avec un mutex
    pthread_create(&thread,NULL,trouverSortieAvecMutex,&arguments);
    // Attendre la fin de l'exécution du thread
    pthread_join(thread, NULL);

    //Desallocation de la matrice
    desallocationMatrice(matrice);
    return 0;
}





