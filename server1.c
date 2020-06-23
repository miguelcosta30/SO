#include <sys/un.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUF_SIZE 4096                                                       // block transfer size
#define LISTENQ 10                                                        // Size of the listen Queue

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
void writeString(const char *filepath, const char *data);

char *socket_path = "/tmp/socket";                                          // Unix domain socket name

int prepate_socket(struct sockaddr_un channel_srv,char *socket_path) {
    int listenfd;
    
    if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {              // Creating the server socket
        perror("socket error");
        exit(-1);
    }
    unlink(socket_path);

    memset(&channel_srv, 0, sizeof(channel_srv));
    channel_srv.sun_family = AF_UNIX;
    strncpy(channel_srv.sun_path, socket_path, sizeof(channel_srv.sun_path)-1);

    if (bind(listenfd, (struct sockaddr*)&channel_srv, sizeof(channel_srv)) == -1) {      // Binding the server socket to its name
        perror("bind error");
        exit(-1);
    }
    
     if (listen(listenfd, LISTENQ) == -1) {                                  // Configuring the listen queue -Numa papelaria só consegue atender '5' Clientes ao mesmo tempo, mas ao fim do dia pode atender mais
        perror("listen error");
        exit(-1);
    }
    return listenfd;
}

    int main(int argc, char *argv[]) {

        int listenfd, connfd,fd;
       
        
        struct sockaddr_un channel_srv;
        listenfd = prepate_socket(channel_srv,socket_path);
        
    int *a;
    int i;
  
    int N;
    int M;
    printf("Quantos filhos deseja criar para a primeira divisao?\n");
    scanf("%d",&M);
    
    printf("Array gerado:\n");
    N=readIntsN("ArrayPai.txt");
    a = newIntArray(N);                                                 //Alocação para array de size N.
    a=readInts("ArrayPai.txt",a);         
    writeInts("ArrayPaiTemp.txt", a, N);
    printArray(a,N);

    printf("\n");
        
    do{



    pid_t *pids;
    pids = newIntArray(M); //Alocação para o array de filhos.
    int div = N/M, divCheck= N%M;
    printf("\nDIVISAO: %d\n", div);    
       

        for(int i = 0; i < M; i++) {
          
            if((pids[i]=fork())==-1){
	        perror("Erro");
	        exit(1);
        }
            if (pids[i] == 0) {
               
                if(divCheck!=0)
            {
                
                if (i == M-1){
                int lo=div*i;
                int hi = N-1;  
                
                
               char stringlo[10];
               sprintf(stringlo,"%d",lo);
                char stringhi[10];
               sprintf(stringhi,"%d",hi);
              
                char *arg[5] = {"client1", "server1",stringlo,stringhi, NULL}; 
                execv("client1", arg);
                perror("Error");
              
                exit(1);
                
            }
            else
            {
                int lo=div*i;
                int hi=(div*i)+(div-1);
                char stringlo[10];
               sprintf(stringlo,"%d",lo);
                char stringhi[10];
               sprintf(stringhi,"%d",hi);
              
                char *arg[5] = {"client1", "server1",stringlo,stringhi, NULL}; 
                execv("client1", arg);
                perror("Error");
              
                exit(1);
                
            }
            }else
            {
                
                int lo=div*i;
                int hi=(div*i)+(div-1);
                
               char stringlo[10];
               sprintf(stringlo,"%d",lo);
                char stringhi[10];
               sprintf(stringhi,"%d",hi);
              
                char *arg[5] = {"client1", "server1",stringlo,stringhi, NULL}; 
                execv("client1", arg);
                perror("Error");
              
                exit(1);
                
              
            }
  
            }
        }
        ssize_t bytes;
    char *buf;
    buf=(char*) malloc(BUF_SIZE*sizeof(char)); 
    char *buf2;
    buf2= (char*) malloc(10000000*sizeof(char));
    char *bufcopy;
    bufcopy=(char*) malloc(BUF_SIZE*sizeof(char));
      
        for(int i = 0; i < M; i++) {
            if((connfd  = accept (listenfd,NULL,NULL)) == -1) {
                perror("accecpt error");
                continue;
            }
              
         
            while((bytes = read(connfd,buf,BUF_SIZE))!=0) {
                // printf("%s",buf); <-String recebida aos chunks(recebe de forma correta)
               
               strcpy(bufcopy,buf);        
               strcat(buf2,bufcopy); //<-ERRO AQUI
            }
            
              close(connfd);
              
        }
       printf("\n");
       
       
    //printf("%s",buf2); //String que fica com todos os "chunks" do que foi lido pelo socket
      
int lo,hi,count1=0;
int sizeArray;
char *msg, *msgsave;
msg = strtok_r(buf2,"|",&msgsave);
	
    while(msg!=NULL){
       write(1,msg,strlen(msg));
       printf("\n");
        sscanf(msg, "%*[^0123456789]%d%*[^0123456789]%d",&lo, &hi);
        
        sizeArray=(hi-lo)+1;
       
		char *msg1, *msg1save;
     	msg1= strtok_r(msg,"*",&msg1save);
    	int count=0;

		while (msg1!=NULL)
        {
          
             count++;
             if(count==2)
        {
            int *paux;
            paux=newIntArray(sizeArray*10);
            char *stringVetor;
            stringVetor =(char*) malloc(10000000*sizeof(char));
            strcpy(stringVetor,msg1);
           // printf("%s",stringVetor);
            paux=stringToarray(paux,stringVetor);
            a= SwitchArrayFather(a,paux,lo,hi);
            writeInts("ArrayPaiTemp.txt",a,N);
            
        }
       
        msg1=strtok_r(NULL,"*",&msg1save);
        }
        
        msg = strtok_r(NULL, "|",&msgsave);
        }
         
        
M=M/2;
    }while(M!=0);

    printf("\nArray final do pai:\n");

    printArray(a,N); 
    printf("Tamanho:%d\n",N);
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
                  

void writeString(const char *filepath, const char *data)
{
    FILE *fp = fopen(filepath, "w");
    if (fp != NULL)
    {
        fputs(data, fp);
        fclose(fp);
    }
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
