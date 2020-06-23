#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int *uniformArray(int * a, int N, int val_min, int val_max);
int uniform(int val_min, int val_max);
int *newIntArray(int  M);
int writeInts(char *filename, int *a, int N);
int main(int argc, char *argv[]){


int *a;
int N,min,max;
        
    printf("Qual o tamanho do array a gerar?\n"); //Tamanho do array a ordenar.
    scanf("%d", &N);
  
  a=newIntArray(N);
    
    
    flag:
    printf("Valor minimo:\n");
    scanf("%d", &min);
    printf("Valor maximo:\n");
    scanf("%d", &max);
    if (max <= min) {
        printf("ERRO|Introduza valores validos\n");
        goto flag;
    }
uniformArray(a, N, min, max);
writeInts("ArrayPai.txt", a, N);


    return 0;
}

int uniform(int  val_min, int val_max) {
    return val_min + rand() % (val_max - val_min + 1);
}
int  *uniformArray(int *a, int N, int val_min, int  val_max) {
    int i;
    for (i = 0; i < N; i++) {
        a[i] = uniform(val_min, val_max);
    }
    return a;
}
int writeInts(char *filename, int *a, int N) {
    FILE *fp;
    int i = 0;
  
    fp = fopen(filename, "w");
    if (fp != NULL) {
        while (i < N){
            if(i==N-1)
            {
                fprintf(fp,"%d", a[i++]);
            }
            else
            {
                fprintf(fp, "%d", a[i++]);
                fprintf(fp, ",");
            }
            
           } //Separação entre virgulas;
        fclose(fp);
    } else
    {
        printf("Erro na abertura do ficheiro");
        return -1; // erro na leitura do ficheiro
    }
    return 0;
}
int *newIntArray(int  M) {
    return (int*) malloc(sizeof(int) * M);
}