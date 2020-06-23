#include <sys/un.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

char *socket_path = "/tmp/socket";

#define BUF_SIZE 4096	

int *mergesort_run(int *a, int n , int lo, int hi);
void merge_array(int *a, int *aux, int lo, int mid, int hi);
void mergesort_recursivo(int *a, int *aux, int lo, int hi);
int  readIntsN(char * filename);
int *readInts(char*filename, int *a);
char *readString(char*filename, char *string);
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

int main(int argc, char **argv)
{
	int count=0;
    int c, uds, bytes;
    			/* buffer for incoming file */
    struct sockaddr_un channel;		/* Unix Domain socket */
    int i=0;

    if (argc != 4) {
        printf("Usage: client file-name\n");
        exit(1);
    }
    
    if ( (uds = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }
    
    memset(&channel, 0, sizeof(channel));
    channel.sun_family= AF_UNIX;
    strncpy(channel.sun_path, socket_path, sizeof(channel.sun_path)-1);

    if (connect(uds, (struct sockaddr*)&channel, sizeof(channel)) == -1) {
        perror("connect error");
        exit(1);
    }

	
	

	int div,lo,hi,N;
 	ssize_t valread;
lo=atoi(argv[2]);
hi=atoi(argv[3]);
div=(hi-lo)+1;


int *a;
N=readIntsN("ArrayPaiTemp.txt");  
a=newIntArray(N);
a=readInts("ArrayPaiTemp.txt",a);
//printArray(a,N);




     		
		
            int *aux; 
            aux= newIntArray(div); 
            aux = SwitchArray(a, div, lo, hi);   
            aux=  mergesort_run(aux, div,0, div-1);
            char *string;                           
            string = (char*) malloc(((div*2)*10000)*sizeof(char));                   //Alocaçao de espaço para uma string 2x maior que o array (devido as virgulas)
            string = vetortoString(aux,string,div);
            char *string2;
            string2=(char*) malloc((((div*2)*10000)*sizeof(char))); 
            sprintf(string2, "lo:%d;hi:%d*", lo, hi);
            strcat(string2,string);
            
            int size=strlen(string2);
            //string2[size+1]='\0';
            strcat(string2,"|");
           
			write(uds , string2 , strlen(string2)+1);  	
           //	printf("%s\n",string2)	;		
         
	exit(0);
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

char *readString(char*filename, char *string){

            FILE *fp;
           char *buf;
            buf=(char*) malloc(BUF_SIZE*sizeof(char)); 
            int counter=0;
            
            fp = fopen(filename, "r");
            if (fp == NULL){
                printf("Erro ao abrir o ficheiro: %s",filename);
            
            }
            while (fgets(buf, BUF_SIZE , fp) != NULL){
                
                strcat(string,buf);
                }
            
           
           
           
            fclose(fp);
           
           return string;
        }
