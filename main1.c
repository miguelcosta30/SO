#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define Buffer 4900096

int *mergesort_run(int *a, int n , int lo, int hi);
void merge_array(int *a, int *aux, int lo, int mid, int hi);
void mergesort_recursivo(int *a, int *aux, int lo, int hi);
int  readIntsN(char * filename);
int *readInts(char*filename, int *a);
int uniform(int val_min, int val_max);
void printArray(const int* a, int N);
int * newIntArray(int M); // alloc new int arrayvoid freeIntArray(int * v); // frees new int array
int *uniformArray(int * a, int N, int val_min, int val_max); // function to fill an array with integer random uniformly distributed values (returns -1 if error)
int writeInts(char * filename, int * a, int n);
int * SwitchArray( int *a,int div, int lo, int hi);
int printIntervalos(int *a, int lo, int hi);
int mergeArrays(int *a, int *b, int n1, int n2);
void handler(int signal_number);




int main(int argc, char *argv[]) {
    int min, max;
    int N;
    int *a,*aux;
    int M, i;
    int pid;
  

//INSTALACÃO DO SINAL    
struct sigaction sa;
sa.sa_handler = &handler;
sa.sa_flags= SA_RESTART;
sigaction(SIGUSR1, &sa, NULL);
sigaction(SIGUSR2, &sa, NULL); //È SEMPRE IGUAL


  
    printf("Quantos filhos deseja criar para a divisão?\n");
    scanf("%d", &M);

    
   


   
    
    N=readIntsN(argv[1]);
    a = newIntArray(N); //Alocação para array de size N.
    printf("\nArray gerado:");
    a=readInts(argv[1],a);
 
    printf("\n");

printArray(a,N);

    do{
        int *pids;                                                                      //Declaração de um array que contem os pids do filhos.
        pids = newIntArray(M);                                                          //Alocação para o array de filhos.
        int div = N / M, divCheck= N%M;                                                 //Div determina o numero de elementos de cada filho(normalmente), divCheck vê se vai existir um filho que vai ordenar mais que os outros 

        for (i=0; i < M; i++) {   
            sleep(1);                                                      //Ciclo que cria M filhos.
        if((pids[i]=fork())==-1){
	    perror("Erro");
	    exit(1);
        }
    //CODIGO DE TODOS DOS FILHOS
        if(pids[i]==0){
        if(divCheck!=0)                                            
        {   
            if (i == M-1) {                                                             //Para uma divisao decimal, o ultimo filho vai ordenar +1 elemento que os outros
                
                printf("Filho(PID:%d) ordenou de a[%d] ate a[%d]->", getpid(), div * i, N-1);
                int intervalo=N-(div*i);                                                            
                int *aux; 
                int pid=getpid();   
                aux = newIntArray(intervalo);
                aux = SwitchArray(a, intervalo, div * i, N-1);
                aux = mergesort_run(aux, intervalo,0,intervalo-1);
                printArray(aux,intervalo);
                char *string1,*string2;
                string1=(char*) malloc((20*sizeof(char))+1);
                sprintf(string1,"Filho");
                string2=(char*) malloc((20*sizeof(char))+1); 
                sprintf(string2,"%d.txt",pid);
                strcat(string1,string2);
               // writeInts(string1, aux, intervalo);
                kill(getppid(),SIGUSR1);                                                //Envio de um sinal SigUsr1 ao seu pai .
                printf("\n");
                exit(EXIT_SUCCESS);

            }
            else
            {
                printf("Filho(PID:%d) ordenou de a[%d] ate a[%d]->",getpid(), (div * i), ((div * i) + (div - 1)));
                int *aux; 
                 int pid=getpid();                                                               
                aux= newIntArray(div);                                                     //Alocaçao de espaço para o SubArray do Filho
                aux = SwitchArray(a, div, div * i, (div * i) + (div - 1));                 //SubArray do filho c"orta" o array original do pai e coleca no seu propio array
                aux=  mergesort_run(aux, div,0, div-1);                                    //Ordenação desse subArray
                printArray(aux,div);
                char *string1,*string2;
                string1=(char*) malloc((20*sizeof(char))+1);
                sprintf(string1,"Filho");
                string2=(char*) malloc((20*sizeof(char))+1); 
                sprintf(string2,"%d.txt",pid);
                strcat(string1,string2);
                //writeInts(string1, aux, div);
                kill(getppid(),SIGUSR1);
                printf("\n");
                exit(EXIT_SUCCESS);
            }
             
            }
            else {
                                                                                         
                printf("Filho(PID:%d) ordenou de a[%d] ate a[%d]->",getpid(), (div * i), ((div * i) + (div - 1)));
                int *aux;
                int pid=getpid();                                                               
                aux= newIntArray(div);                                                     //Alocaçao de espaço para o SubArray do Filho
                aux = SwitchArray(a, div, div * i, (div * i) + (div - 1));                 //SubArray do filho "corta" o array original do pai e coloca no seu subarray
                aux=  mergesort_run(aux, div,0, div-1);                                    //Ordenação desse subArray
                printArray(aux,div);
                char *string1,*string2;
                string1=(char*) malloc((20*sizeof(char))+1);
                sprintf(string1,"Filho");
                string2=(char*) malloc((20*sizeof(char))+1); 
                sprintf(string2,"%d.txt",pid);
                strcat(string1,string2);
                //writeInts(string1, aux, div);
                kill(getppid(),SIGUSR1);
                printf("\n");
                exit(EXIT_SUCCESS);
              
            }
        }         
 }
    for(int i=0;i<M;i++){ 
	
    int result;
	waitpid(pids[i],&result, 0);  //Pai espera pela finalização de cada filho.
        
        }
    M=M/2;
                       //free para um novo array de filho.
    }while(M!=0);

    //printf("Array do pai\n");
    //printArray(a,N);
    
    return 0;
}


int readIntsN(char*filename){


            FILE *fp;
            char str[Buffer];
            int counter=0;

            fp = fopen(filename, "r");
            if (fp == NULL){
                printf("Erro ao abrir o ficheiro: %s",filename);
            
            }
            while (fgets(str, Buffer, fp) != NULL){
                printf("%s", str);
          char *token;
           token=strtok(str,",");
           while(token!=NULL)
           {
               counter++;
               token=strtok(NULL,",");
           }
           }
            fclose(fp);
           
           return counter;
        }

int *readInts(char*filename,int *a){

            FILE *fp;
            char str[Buffer];
            int num, i=0;

            fp = fopen(filename, "r");
            if (fp == NULL){
                printf("Erro ao abrir o ficheiro: %s",filename);
            
            }
            while (fgets(str, Buffer, fp) != NULL)
                {
            char *token;
            token=strtok(str,",");
           while(token!=NULL)
           {
               num=atoi(token);
               a[i]=num;
               i++;
               token=strtok(NULL,",");
           }

                  }
               
                fclose(fp);
                return a;
           
}



void printArray(const int  *a, int  N) {
    int i;
    for (i = 0; i < N; i++) {
        printf("%d ", *(a+i));
    }
    printf("\n");
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
void merge_array(int*a, int*aux, int  lo, int mid, int hi) {
    int k;
    for (k = lo; k <= hi; k++)
        *(aux+k) = *(a+k);
    int i = lo, j = mid+1;
    for (k = lo; k <= hi; k++) {
        if (i > mid) *(a+k) = *(aux+(j++));
        else if (j > hi) *(a+k) = *(aux+(i++));
        else if (*(aux+j) < *(aux+i)) *(a+k) = *(aux+(j++));
        else *(a+k) = *(aux+(i++));
    }
}

void mergesort_recursivo(int *a, int *aux, int lo, int hi) {
    if (hi <= lo) return;
    int mid = lo + (hi - lo) / 2;
    mergesort_recursivo(a, aux, lo, mid);
    mergesort_recursivo(a, aux, mid+1, hi);
    merge_array(a, aux, lo, mid, hi);
}

int *mergesort_run(int *a, int n , int lo, int hi) {
    int  * aux;
    aux = malloc(sizeof(int) *n);
    mergesort_recursivo(a, aux, lo, hi);
    free(aux);
    return a;
}
void handler(int signal_number)
{
	switch(signal_number)	{//IRA PERGUNTAR O TEMPO PARA FAZER A DIFERENÇA
		case SIGUSR1:
			printf("SIGUSR1\n");
			 
			break;

		case SIGUSR2:
			printf("SIGUSR2\n");
 		
			break;
		

			
		default:
			printf("Signal %d", signal_number);
			break;
}
}
int *SwitchArray(int *a,int div, int lo, int hi)
{
    int *b;
    b= newIntArray(div);
    int i,j=0;

    for(i=lo; i<=hi;i++)
   {
      b[j]=a[i];
       j++;
   }
   return b;
}