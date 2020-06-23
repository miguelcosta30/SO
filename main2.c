#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <apue.h>
#define BUF_SIZE 4096

int *mergesort_run(int *a, int n , int lo, int hi);
void merge_array(int *a, int *aux, int lo, int mid, int hi);
void mergesort_recursivo(int *a, int *aux, int lo, int hi);
int  readIntsN(char * filename);
int *readInts(char*filename, int *a);
int uniform(int val_min, int val_max);
void printArray(const int* a, int N);
int * newIntArray(int M); // alloc new int arrayvoid freeIntArray(int * v); // frees new int array
int uniformArray(int * a, int N, int val_min, int val_max); // function to fill an array with integer random uniformly distributed values (returns -1 if error)
int writeInts(char * filename, int * a, int n);

void handler(int signal_number);
int * SwitchArray( int *a,int div, int lo, int hi);
int *SwitchArrayFather(int *a, int *aux, int lo, int hi);
char *vetortoString(int *vetor,char *string,int N);
int *stringToarray(int *a, char *string);

int main(int argc, char *argv[]) {
    int min, max;
    int N;
    int *a;
    int M, i;
    int fds[2];
    
     int n;
   

    

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
    //printArray(a,N);

 
  

    do{
        int *pids;
         pids = newIntArray(M); //Alocação para o array de filhos.
        
        pipe(fds);

        int div = N / M, divCheck= N%M;
        for (i=0; i < M; i++) { 
                                   //CRIAÇÃO DE M FILHOS
        if((pids[i]=fork())==-1){
	    perror("Erro");
        printf("Erro ao criar o filho");
	    exit(1);
        }
    //CODIGO DE TODOS DOS FILHOS
        if(pids[i]==0){
        if(divCheck!=0)                                                                                  //Para uma divisao decimal, o ultimo filho vai ordenar +1 elemento que os outros
        {
            if (i == M-1) {
                    close(fds[0]);
                    
                    int intervalo=N-(div*i);                                                            //Filho feicha o lado de leitura do pipe pois nao vai necessitar dele.
                    int *aux;
                    char *msg;
                    int size;
                    aux = newIntArray(intervalo);
                    aux = SwitchArray(a, intervalo, div * i, N-1);
                    aux = mergesort_run(aux, intervalo,0,intervalo-1);
                    
                    msg = (char*) malloc(((((intervalo+1)*2)*1000)*sizeof(char))+1);  
                    sprintf(msg, "PID:%d*lo:%d;hi:%d*", getpid(), div*i,N-1 );                 
                    char *string;
                    string = (char*) malloc(((((div*2)*1000)*sizeof(char))+1));
                    string = vetortoString(aux,string,intervalo);
                    strcat(msg,string);
                   
                    size=strlen(msg);
                    msg[size+1]='\0'; 
                    writen(fds[1], msg, strlen(msg));
                    
                close(fds[1]);
                exit(EXIT_SUCCESS);

            } else {
                    
                    close(fds[0]);
                    char *msg;
                    int *aux;
                    int size;
                    aux= newIntArray(div);                                                                              //Filho feicha o lado de leitura do pipe pois nao vai necessitar dele.
                    aux =  SwitchArray(a, div, div * i, (div * i) + (div - 1));
                    aux= mergesort_run(aux, div ,0, div-1);
               
                  
                        msg = (char*) malloc((((div*2)+1000)*sizeof(char))+1); 
                        sprintf(msg, "PID:%d*lo:%d;hi:%d*", getpid(), div*i, (div*i)+(div-1));
                        char *string;
                        string = (char*) malloc(((((div*2)*1000)*sizeof(char))+1));
                        string = vetortoString(aux,string,div);
                        
                        strcat(msg,string);
                        size=strlen(msg);
                        msg[size+1]='\0'; 
                        writen(fds[1], msg, strlen(msg));
                    
                   
                    close(fds[1]);
                    exit(EXIT_SUCCESS);
            }
        } 
            else {
                close(fds[0]); 
                char *msg;                                                                 //Filho feicha o lado de leitura do pipe pois nao vai necessitar dele.
                int *aux;   
                int size;                                                             
                aux= newIntArray(div);                                                     //Alocaçao de espaço para o SubArray do Filho
                aux = SwitchArray(a, div, div * i, (div * i) + (div - 1));                 //SubArray do filho corta o array original do pai
                aux=  mergesort_run(aux, div,0, div-1);                                    //Ordenação desse subArray
               
        
                    msg = (char*) malloc((((div*2)*1000)*sizeof(char))+1);                 //Alocação de memoria para a string protocolo final        
                    sprintf(msg, "PID:%d*lo:%d;hi:%d*", getpid(), div*i, (div*i)+(div-1));  //Escreve mensagem na string
                    char *string;                           
                    string = (char*) malloc(((div*2)*1000)*sizeof(char));                   //Alocaçao de espaço para uma string 2x maior que o array (devido as virgulas)
                    string = vetortoString(aux,string,div);                                    //Funcao que converte vetor para string
                                    
                    strcat(msg,string);  
                    size=strlen(msg);
                    msg[size+1]='\0'; 
                    printf("%s\n",msg);                                                                    //Junção da string com a msg de protocolo
                    writen(fds[1], msg, strlen(msg));                                     //Envia essa mensagem pelo pipe
                   
                   
                close(fds[1]);
                
                exit(EXIT_SUCCESS);
               
                    }

        }
        }
    
    close(fds[1]);                                                                             //Pai fecha o lado de escrita
    int pid, lo, hi , sizeArray;                                                                
     char *buf;
    buf=(char*) malloc(BUF_SIZE*sizeof(char)); 
    char *buf2;
    buf2= (char*) malloc(10000000*sizeof(char)); 
    ssize_t bytes;
    int count=0;
    int count1=0;
   int counterFilhos=0;
  
    while((bytes = readn(fds[0], buf, BUF_SIZE))!=0)
{
     strcat(buf2,buf); 
}
    //printf("%s",buf2);
    char *msg, *msgsave;
	msg = strtok_r(buf2,"|",&msgsave);
	while(msg!=NULL && count1<M)
{
 
    printf("\n");
   // writen(1, msg, strlen(msg));
    //printf("\n");
   sscanf(msg, "%*[^0123456789]%d%*[^0123456789]%d%*[^0123456789]%d",&pid, &lo, &hi);
    sizeArray= (hi-lo)+1;
    printf("\tTamanho de array:%d\n",sizeArray);
    char *msg1, *msg1save;
    
     msg1= strtok_r(msg,"*",&msg1save);
    count=0;
		while (msg1!=NULL)
        {
            
            count++;
             if(count==3)
        {
            int *paux;
            paux=newIntArray(sizeArray);
            char *stringVetor;
            stringVetor = (char*) malloc(((sizeArray*1000)*sizeof(char))+1);
            strcpy(stringVetor,msg1);
           // printf("\n%s\n",stringVetor);  
            paux=stringToarray(paux,stringVetor);
            a= SwitchArrayFather(a,paux,lo,hi);
             counterFilhos++;
        }
        msg1=strtok_r(NULL,"*",&msg1save);
        }

		msg = strtok_r(NULL, "|",&msgsave);
        count1++;
}

    

printf("Numero de filhos=%d\n",counterFilhos);

   
   int pid1=-1;
    int status;
    for(i=0; i < M; i++)
    {
     pid1=waitpid(pids[i], &status, 0); //Pai espera pela finalização dos filhos
     }
    
    free(pids);
    M=M/2;
    }while(M!=0);
                            
    printf("Array do pai:\n");
    printArray(a,N);
    writeInts("ArrayPaiTemp.txt",a,N);
    printf("\nTamanho: %d\n",N);
    return 0;
}
int readIntsN(char*filename){

            FILE *fp;
           char *buf;
            buf=(char*) malloc(BUF_SIZE*sizeof(char)); 
            int counter=0;
            char *stringVetor;
            stringVetor = (char*) malloc(1000000*sizeof(char));
            fp = fopen(filename, "r");
            if (fp == NULL){
                printf("Erro ao abrir o ficheiro: %s",filename);
            
            }
            while (fgets(buf, BUF_SIZE , fp) != NULL){
                //printf("%s\n", buf);
                strcat(stringVetor,buf);
                }
            char *token;
           token=strtok(stringVetor,",");
           while(token!=NULL)
           {
               counter++;
               token=strtok(NULL,",");
           }
           
            fclose(fp);
           
           return counter;
        }

int *readInts(char*filename,int *a){

           FILE *fp;
           char *buf;
            buf=(char*) malloc(BUF_SIZE*sizeof(char)); 
            int counter=0;
            char *stringVetor;
            int i=0;
            int num;
            stringVetor = (char*) malloc(1000000*sizeof(char));
            fp = fopen(filename, "r");;
            if (fp == NULL){
                printf("Erro ao abrir o ficheiro: %s",filename);
            
            }
            while (fgets(buf, BUF_SIZE , fp) != NULL)
                {
                    strcat(stringVetor,buf);
                }
            char *token;
            token=strtok(stringVetor,",");
           while(token!=NULL)
           {
               num=atoi(token);
               a[i]=num;
               i++;
               token=strtok(NULL,",");
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
int  uniformArray(int *a, int N, int val_min, int  val_max) {
    int i;
    for (i = 0; i < N; i++) {
        a[i] = uniform(val_min, val_max);
    }
    return *a;
}
int writeInts(char *filename, int *a, int N) {
    FILE *fp;
    int i = 0;
    
    fp = fopen(filename, "w");
    if (fp != NULL) {
        while (i < N){
            fprintf(fp, "%d", a[i++]);
            fprintf(fp, ",");} //Separação entre virgulas;
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
int *SwitchArrayFather(int *a, int *aux, int lo, int hi)
{
    int i,j=0;
    for(i=lo;i<=hi;i++)
    {
        a[i]=aux[j];
        j++;
    }
    return a;
}
char *vetortoString(int *vetor,char *string,int N)
{
    char *string1;
    string1= (char*) malloc(4096*sizeof(char)); 
  
    int i;
    int num;
    for( i=0; i<N;i++)
    {
        num=vetor[i];
        sprintf(string1,"%d",num);
       
        if( i==N-1)
        {
            strcat(string,string1);
            strcat(string,"|");
        } else{
            strcat(string,string1);
            strcat(string,",");}

    }
    int Tam =strlen(string);
    string[Tam+1]='\0';
    
    return string;
}
int *stringToarray(int *a, char *string)
{
    int i=0;
    int num;
    char *token;
    token = strtok(string,",");
    while(token!=NULL)
    {
        sscanf(token,"%d", &num);
        a[i]=num;
        i++;
        token=strtok(NULL,",");
    }
    return a;
}