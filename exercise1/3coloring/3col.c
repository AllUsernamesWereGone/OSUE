#include "3col.h"

void createMatrix(int argc, char** argv, Matrix *matrix){

    //Matrix initialisieren
    matrix->vertices_count = 0;
    matrix->edges = argc-1;
    //Adjazenz-Matrix und Array initialisieren
    for(int i = 0; i < MAX_VERTICES; i++){
        matrix->vertices[i] = -1; //array mit -1 füllen
        for(int j = 0; j < MAX_VERTICES; j++){
            if(i == j){
                matrix->admatrix[i][j] = -1;//sollte eig auf 1 sein
            }else{
                matrix->admatrix[i][j] = 0;
            }
        }
    }

    //Kanten einfügen in Matrix
    for(int i = 1; i < argc; i++){
        //Kanten im Format    1-2, 30-2, 30-20, 1-30  a=1 b=2
        char *edge = argv[i];
        char *dash = strchr(edge, '-');

        *dash = '\0';// "-" ersetzen
        
        int u = atoi(edge);//adresse
        int v = atoi(dash + 1);//adresse +1 
        //printf("%d %d\n",u,v);


        matrix->admatrix[u][v]= 1;
        matrix->admatrix[v][u]= 1;

        //nimm größte zahl vom graph +1 = anzahl an knoten
        if(u + 1 > matrix-> vertices_count ) matrix-> vertices_count  = u + 1;
        if(v + 1 > matrix-> vertices_count ) matrix-> vertices_count  = v + 1;
    
        *dash = '-'; //wieder "-" hinzufügen
    }
}


void printMatrix(Matrix *matrix){

    for(int i = 0; i < matrix-> vertices_count ; i++){
        for(int j = 0; j < matrix->vertices_count; j++){
            if(matrix->admatrix[i][j] == 1){
                printf("\033[0;31m");//rote farbe
            }
            printf("[%d]", matrix->admatrix[i][j]);
            printf("\033[0m");
            if(j == matrix->vertices_count -1){
                printf("\n");
            }
        }
    }
        printf("V: %d, E: %d\n", matrix->vertices_count, matrix->edges);
}
void printVertices(Matrix *matrix){
    for(int i = 0; i < matrix->vertices_count; i++){
        if(matrix->vertices[i]==0) printf("\033[0;31m");//rot
        if(matrix->vertices[i]==1) printf("\033[0;33m");//gelb
        if(matrix->vertices[i]==2) printf("\033[0;34m");//blau
        printf("[%d]", matrix->vertices[i]);
    }
    printf("\033[0m");
    printf("\n");
}

void removeAmount(Matrix *matrix, Solution *solution){
    int counter = 0;
    solution->edge_count = counter;
    for(int i = 1; i < matrix->vertices_count; i++){ //i=1 da 1. knoten trivial
        for(int j = 0; j < i; j++){
            if(matrix->admatrix[i][j] == 1){//kante vorhanden
                if(matrix->vertices[i] == matrix->vertices[j]){//gleiche farbe
                    counter++;
                    if(counter > MAX_EDGES){ //too many edges
                        break;
                    }
                }
            }
        }
    }
    solution->edge_count = counter;
}

void findEdges(Matrix *matrix, Solution *solution){

    //solution[MAX_EDGES]
    int counter = 0;

    for(int i = 1; i < matrix->vertices_count; i++){ //i=1 da 1. knoten trivial
        for(int j = 0; j < i; j++){
            if(matrix->admatrix[i][j] == 1){//kante vorhanden
                if(matrix->vertices[i] == matrix->vertices[j]){//gleiche farbe
                    solution->edges[counter].u = j;
                    solution->edges[counter].v = i;
                    counter++;
                    if(counter == solution->edge_count){//bereits max anzahl, unnötig weiter zu suchen
                        break;
                    }
                }
            }
        }
    }
}


void printCurrentSolution(Solution *solution){
    printf("The graph might not be 3-colorable, best solution removes %d edges.\n", solution->edge_count);
/*            The Edges are: ", solution->edge_count);
    for(int i = 0; i < solution->edge_count; i++){
        printf("%d-%d ", solution->edges[i].u, solution->edges[i].v);
    }
    printf("\n");*/
}












