#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX 100
#define MAXDIM 20
//FILE *mat1Txt, *mat2Txt, *ans1Txt, *ans2Txt, *ans3Txt;
int **M1, **M2, **ANS;
struct thread_data{
    int row1;
    int col1;
    int col2;
};

void spaceParser(char *parsed[], char *st){
    int i;
    for(i = 0; i < MAX; i++){
        parsed[i] = strsep(&st, " ");
        if(parsed[i] == NULL)
            break;
    }
}

char * equalParser(char *s){
    strcpy(s, strtok(s, "="));
    return strcpy(s, strtok(NULL, "="));
}

// Thread per matrix
void* perMatrixMult(void *dim){
    struct thread_data *my_data;
    my_data = (struct thread_data *) dim;
    int r1 = my_data->row1;
    int c1 = my_data->col1;
    int c2 = my_data->col2;
    int i, j ,k;

    for (i = 0; i < r1; i++) {
      for (j = 0; j < c2; j++) {
         ANS[i][j] = 0;
         for (k = 0; k < c1; k++)
            ANS[i][j] += M1[i][k] * M2[k][j];
      }
    }
    free(my_data);
}

void perMatrixThreading(int r1, int c1, int c2){
    pthread_t t;

    struct thread_data *tdata = malloc(sizeof(struct thread_data));
    tdata->row1 = r1;
    tdata->col1 = c1;
    tdata->col2 = c2;

    if(pthread_create(&t, NULL, perMatrixMult, (void *)tdata)){
        printf("Failed to create thread\n");
        exit(-1);
    }
    pthread_join(t, NULL);
}

// Thread per row
void* perRowMult(void *dim){
    struct thread_data *my_data;
    my_data = (struct thread_data *) dim;
    int r1 = my_data->row1;
    int c1 = my_data->col1;
    int c2 = my_data->col2;
    int i ,j;
    for (i = 0; i < c2; i++) {
     ANS[r1][i] = 0;
     for (j = 0; j < c1; j++)
        ANS[r1][i] += M1[r1][j] * M2[j][i];
    }
    free(my_data);

}

void perRowThreading(int r1, int c1, int c2){
    pthread_t threads[r1];
    int i;
    for(i = 0; i < r1; i++){
        int *p = malloc(sizeof(int));
        *p = i;
        struct thread_data *tdata = malloc(sizeof(struct thread_data));
        tdata->row1 = *p;
        tdata->col1 = c1;
        tdata->col2 = c2;
        if(pthread_create(&threads[i], NULL, perRowMult, (void *)tdata)){
            printf("Failed to create thread\n");
            exit(-1);
        }
    }

    for(i = 0; i < r1; i++)
        pthread_join(threads[i], NULL);

}

// Thread per element
void* perElemMult(void *dim){
    struct thread_data *my_data;
    my_data = (struct thread_data *) dim;
    int r1 = my_data->row1;
    int c1 = my_data->col1;
    int c2 = my_data->col2;
    int i;
    ANS[r1][c2] = 0;
    for(i = 0; i < c1; i++)
        ANS[r1][c2] += M1[r1][i] * M2[i][c2];
    free(my_data);
}

void perElemThreading(int r1, int c1, int c2){
    pthread_t threads[r1 * c2];

    int i, j;
    for(i = 0; i < r1; i++){
        for(j = 0; j < c2; j++){

            int *p1 = malloc(sizeof(int));
            int *p2 = malloc(sizeof(int));
            *p1 = i;
            *p2 = j;
            struct thread_data *tdata = malloc(sizeof(struct thread_data));
            tdata->row1 = *p1;
            tdata->col1 = c1;
            tdata->col2 = *p2;
            if(pthread_create(&threads[i * c2 + j], NULL, perElemMult, (void *)tdata)){
                printf("Failed to create thread\n");
                exit(-1);
            }
        }
    }

    for(i = 0; i < r1 * c2; i++)
        pthread_join(threads[i], NULL);
}

void writeAns(char *fileName, int rows, int cols, int whichFIle){ //whichFile = 0 -> per_matrix, whichFile = 1 ->p per_row, whichFile = 2 -> per_element.
    FILE *fp;
    int i,j;
    fp = fopen(fileName, "w");
    switch(whichFIle){
        case 0:
            fprintf(fp,"Method: A thread per matrix\nrow=%d col=%d\n", rows, cols);
            break;
        case 1:
            fprintf(fp, "Method: A thread per row\nrow=%d col=%d\n", rows, cols);
            break;
        case 2:
            fprintf(fp,"Method: A thread per element\nrow=%d col=%d\n", rows, cols);
            break;
    }
    for(i = 0; i < rows; i++){
        for(j = 0; j < cols; j++)
            fprintf(fp, "%d ", ANS[i][j]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

int main(){
    struct timeval stop, start;
    FILE *fp;
    char mat1Name[MAX] = "a.txt", mat2Name[MAX] = "b.txt";
    char perMatrix[MAX] = "c_per_matrix.txt", perRow[MAX] = "c_per_row.txt", perElem[MAX] = "c_per_element.txt";
    char inputNames[MAX] = "";
    char *names[3];
    char rows[7] = "", cols[7] ="";
    int r1, c1, r2, c2, i, j;

    // Read names of text files if exist
    scanf("%[^\n]s", inputNames);
    spaceParser(names, inputNames);

    // checks if input names are provided
    if(strcmp("", inputNames) != 0){
        strcpy(mat1Name, names[0]);
        strcat(mat1Name,".txt");

        strcpy(mat2Name, names[1]);
        strcat(mat2Name,".txt");

        strcpy(perMatrix, names[2]);
        strcat(perMatrix, "_per_matrix.txt");

        strcpy(perRow, names[2]);
        strcat(perRow, "_per_row.txt");

        strcpy(perElem, names[2]);
        strcat(perElem, "_per_element.txt");
    }

    // Read matrix 1

    fp = fopen(mat1Name, "r");
    if(fp == NULL){
        printf("File not found\n");
        exit(-1);
    }
    fscanf(fp, "%s %s", rows, cols);
    r1 = atoi(equalParser(rows));
    c1 = atoi(equalParser(cols));
    M1 = (int**) malloc(r1 * sizeof(int*));
    for (i = 0; i < r1; i++)
        M1[i] = (int*) malloc(c1 * sizeof(int));
    for(i = 0; i < r1; i++){
        for(j = 0; j < c1; j++)
            fscanf(fp, "%d", &M1[i][j]);
    }
    fclose(fp);

    // Read matrix 2
    fp = fopen(mat2Name, "r");
    if(fp == NULL){
        printf("File not found\n");
        exit(-1);
    }
    fscanf(fp, "%s %s", rows, cols);
    r2 = atoi(equalParser(rows));
    c2 = atoi(equalParser(cols));
    M2 = (int**) malloc(r2 * sizeof(int*));
    for (i = 0; i < r2; i++)
        M2[i] = (int*) malloc(c2 * sizeof(int));
    for(i = 0; i < r2; i++){
        for(j = 0; j < c2; j++)
            fscanf(fp, "%d", &M2[i][j]);
    }
    fclose(fp);

    // Invalid multiplication
    if(c1 != r2){
        printf("ERROR");
        exit(1);
    }

    ANS = (int**) malloc(r1 * sizeof(int*));
    for (i = 0; i < r1; i++)
        ANS[i] = (int*) malloc(c2 * sizeof(int));

    gettimeofday(&start, NULL); //start checking time
    perMatrixThreading(r1, c1, c2);
    gettimeofday(&stop, NULL); //end checking time
    printf("Thread per matrix\n");
    printf("Threads created = 1\n");
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    writeAns(perMatrix, r1, c2, 0);

    gettimeofday(&start, NULL); //start checking time
    perRowThreading(r1, c1, c2);
    gettimeofday(&stop, NULL); //end checking time
    printf("Thread per row\n");
    printf("Threads created = %d\n", r1);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    writeAns(perRow, r1, c2, 1);

    gettimeofday(&start, NULL); //start checking time
    perElemThreading(r1, c1, c2);
    gettimeofday(&stop, NULL); //end checking time
    printf("Thread per element\n");
    printf("Threads created = %d\n", r1 * c2);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    writeAns(perElem, r1, c2, 2);

    free(M1);
    free(M2);
    free(ANS);
    return 0;
}


