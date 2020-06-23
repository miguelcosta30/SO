#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>


#define THREADS 100
int name = 0, iname = 0, type = 0, empty = 0, exec = 0, modify = 0, size = 0; //Flags para os parametros da funcao find (0->Nao contem este argumento/1->Contem este argumento);
int args;

typedef struct thread_data{ //Estrutura de tarefas  onde cada tarefa tem o seu path para executar a funcao.
	pthread_t  thread_id; //Id da tarefa
  char *base_path;
}T_DATA;

struct argcmd //Lista ligada de argumentos
{
  char *arg;
  char *argvalue;
  struct argcmd *nextarg;
};

typedef struct strings          //lista de path's
{
  char *name;
  struct strings *next;
 
}STRINGS;

typedef struct threadmatch      //Lista ligada de threads que encontraram path's
{
  pthread_t  thread_id; 
  int matches;
   struct strings *firstString;
  
  struct threadmatch *next;
}THREADMATCHS;





int countargs(char *string);
void *DirAndStacs(void *string);
void *print_msg(void *unused);
int *newIntArray(int M);
void argumnets(char *string);
void insertargs(struct argcmd **head, char *new_arg, char *new_argval);
void printListArgs(struct argcmd *node);
void insertargs_tolist(char *string);
int find_name(struct argcmd *head, char *filename);
int find_iname(struct argcmd *head, char *filename);
char *minuscula(char *string);
int find_bysize(struct argcmd *head, float sizebytes);
int find_typehelper(struct argcmd *head);
int nodecount(struct argcmd *head);
int find_byempty(char *base_path);
void inserthreads(struct threadmatch **head, struct threadmatch *no,long int id,char *path);
void inserthreadspath(struct threadmatch *no,long int id,char *path);
void printListArgshelp();
int compareNameLeft(char *argname, char*filename);
int compareNameRight(char *argname, char*filename);
int find_bynmin(struct argcmd *head, time_t tmin);
struct argcmd *Head = NULL;             
struct threadmatch *HeadThreds=NULL;
pthread_mutex_t lock ;

int main(int argc, char *argv[])
{

  char *cmd = malloc(sizeof(char) * 300);
  char *path = malloc(sizeof(char) * 300);
  char *pathhelp = malloc(sizeof(char) * 300);
  char *specs = malloc(sizeof(char) * 300);
  char *specshelp = malloc(sizeof(char) * 300);

  pthread_t thread_id;
  struct thread_data  mainthread_args;
  
  pthread_mutex_init(&lock,NULL);

  printf("find ");
  fgets(cmd, 300, stdin);
  strtok(cmd,"\n");
  args = countargs(cmd); //Funcao para saber quantos argumentos sao
  argumnets(cmd);
  char *token;
  token = strtok(cmd, " ");
  int count = 1;
  while (token != NULL) //Descodificar a string
  {
    if (count == 1) //A primeira palavra é sempre o caminho
    {
      strcpy(path, token);
    }
    
    else //Junta todos os argumentos numa string de argumentos
    {
      strcat(specs, token);
      //strcat(specs, specshelp);
      strcat(specs, " ");
    }
    count++;
    token = strtok(NULL, " ");
  }

  if (strcmp(path, ".") == 0) //Se for um ponto quer dizer queé o diretorio onde se encontra no momento.
  {
    getcwd(path, 300);
    strcat(path,"/");
  }
   if(strstr(path,"."))     //Se for . + nome (./ProjetoSO2)
   {
    int i;
    int j=1;
    for(i=0;path[i]!='\0';i++)
    {
        pathhelp[i]=path[j];
        j++;
    }
    pathhelp[i]='\0';
    printf("%s\n",pathhelp);
    getcwd(path, 300);
    strcat(path,pathhelp);
   }



  insertargs_tolist(specs);
  mainthread_args.base_path= malloc(sizeof(char)*300);   

  strcpy(mainthread_args.base_path,path);
  pthread_create(&thread_id, NULL, &DirAndStacs, &mainthread_args); //tarefa main é chamada para utilizar esta funcao
  pthread_join(thread_id, NULL);
  printListArgshelp();
 

  return 0;
}

int countargs(char *string)
{
  int i = 0;
  int args = 0;

  while (string[i] != '\0')
  {
    if (string[i] == ' ')
    {

      args++;
    }
    i++;
  }

  args++;
  return args;
}

void *DirAndStacs(void *string)
{
  int counter=1;
  int i=0;
  int isempty = 0;
  struct thread_data *my_data;
  my_data= (struct thread_data *) string;

  
  
  DIR *dir;
  struct dirent *entry;
  pthread_t *thread_id=malloc(THREADS*sizeof(long)); //Aloca espaço para N threads
  struct stat file_stat;
  char *path = malloc(sizeof(char) * 300);




  if ((dir = opendir(my_data->base_path)) == NULL)
    perror("Error");
  else
  {
    
    while ((entry = readdir(dir)) != NULL)
    {
        int flag=0;


      sprintf(path, "%s%s",my_data->base_path, entry->d_name);

      if (stat(path, &file_stat) == 0)
        
      {
        if (S_ISDIR(file_stat.st_mode) && (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)) //Apenas diretorios
        {
          if (name == 1)
          {
            if (find_name(Head, entry->d_name) == 1)
            {
              flag++;    
            }
          }

          if (iname == 1)
          {
            if (find_iname(Head, entry->d_name) == 1)
            {
              flag++;
            }
          }
           if (type == 1)
          {
            if(find_typehelper(Head)==1)
            {
            flag ++;
            }
          }
           if (size == 1)
          {
            if(find_bysize(Head, file_stat.st_size) == 1)
            {
            
            flag++;
            }
          }
           if (modify == 1)
          {
            
            if(find_bynmin(Head, file_stat.st_mtime)==1)
            {
              flag++;
            }
            
          }
           if (exec == 1)
          {
            if((file_stat.st_mode & S_IXUSR) && (file_stat.st_mode & S_IXGRP) && (file_stat.st_mode & S_IXOTH))
            {
              flag++;
            }
            
          }
          if( empty==1)
          {
            if(find_byempty(path)==1)
                {
                  flag ++;
                }
          }

          if(flag==nodecount(Head)) //Caso o diretorio respeite todas especificaçóes entao foi encontrado.
          {
           pthread_mutex_lock(&lock);
            long int id=pthread_self();
            inserthreads(&HeadThreds, HeadThreds,id,path);
             pthread_mutex_unlock(&lock);
           
          }

          T_DATA thread_data[100];   
          strcat(path,"/");
          thread_data[i].base_path= malloc(sizeof(char)*300);   
          strcpy(thread_data[i].base_path,path);
          pthread_create(&thread_id[i], NULL, DirAndStacs,&thread_data[i]); //
          i++;
        }
        
        
        if ((strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)&& !S_ISDIR(file_stat.st_mode))  //Apenas para ficheiros
        {

          if (name == 1)
          {

            if (find_name(Head, entry->d_name) == 1)
            {
              flag++;    
            }
          }

          if (iname == 1)
          {
            if (find_iname(Head, entry->d_name) == 1)
            {
              flag++;
            }
          }
          if( empty==1)
          {
                if(find_byempty(path)==1)
                {
                  flag ++;
                }
          }
          if (type == 1)
          {
             if(find_typehelper(Head)==2)
            {
              flag ++;
            }
          }
           if (modify == 1)
          {
            if(find_bynmin(Head, file_stat.st_mtime)==1)
            {
              flag++;
            }
          }
          if (exec == 1)
          {
            if((file_stat.st_mode & S_IXUSR) && (file_stat.st_mode & S_IXGRP) && (file_stat.st_mode & S_IXOTH))
            {
              flag++;
            }
          }
          if (size == 1)
          {
            if(find_bysize(Head, file_stat.st_size) == 1)
            {
            flag++;
            }
          }
          if(flag==nodecount(Head)) //Caso o ficheiro respeite todas especificaçóes entao foi encontrado.
          {
             pthread_mutex_lock(&lock);
           long int id=pthread_self();
          inserthreads(&HeadThreds, HeadThreds,id,path);
           pthread_mutex_unlock(&lock);
          }
         // goto end;
        }
      }
    
    //end:
      strcpy(path, "");
   
    }
    closedir(dir);

    for (int j=0;j<i;j++){
    pthread_join(thread_id[j], NULL); }
    
  }

  return 0;
}

void argumnets(char *string)
{
  if (strstr(string, "-name"))
  {
    name = 1;
  }
  if (strstr(string, "-iname"))
  {
    iname = 1;
  }
  if (strstr(string, "-type"))
  {
    type = 1;
  }
  if (strstr(string, "-empty"))
  {
    empty = 1;
  }
  if (strstr(string, "-executable"))
  {
    exec = 1;
  }
  if (strstr(string, "-mmin"))
  {
    modify = 1;
  }
  if (strstr(string, "-size"))
  {
    size = 1;
  }

  printf("%d,%d,%d,%d,%d,%d,%d\n", name, iname, type, empty, exec, modify, size);
}





void insertargs(struct argcmd **head, char *new_arg, char *new_argval)      //Funcao para colocar os valores na linked list
{
  struct argcmd *new_node = (struct argcmd *)malloc(sizeof(struct argcmd));

  new_node->arg = (char *)malloc(sizeof(char) * 300);
  new_node->arg = new_arg;
  new_node->argvalue = (char *)malloc(sizeof(char) * 300);
  new_node->argvalue = new_argval;
  new_node->nextarg = *head;
  *head = new_node;
}

void printListArgs(struct argcmd *node)
{
  while (node != NULL)
  {
    printf("%s %s\n", node->arg, node->argvalue);
    node = node->nextarg;
  }
}

void insertargs_tolist(char *string)                    //Funcao que coloca os valores na lista
{
  char *arg = malloc(sizeof(char) * 30);
  char *token;
  int flag=0;
  int count = 0;
  token = strtok(string, " ");
  while (token != NULL)
  {

    count++;
    if ((count % 2) != 0)     //Se for impar é um argumento
    {

      strcpy(arg, token);

       if(strcmp(arg,"-empty")==0 || strcmp(arg,"-executable")==0)    //Se for o executable e o empty, como apenas têm 1 argumentos, reseta o count
      {
         count=0;     
         char *arg_ = malloc(sizeof(char) * 30);
       char *argval = malloc(sizeof(char) * 30);
      strcpy(arg_, arg); 
      strcpy(argval, "-");
      insertargs(&Head, arg_, argval);
   
      }
      

      token = strtok(NULL, " ");
       
    }
    else                          //Se for par é o valor do argumento 
    { 
      char *arg_ = malloc(sizeof(char) * 30);
      char *argval = malloc(sizeof(char) * 30);
      strcpy(arg_, arg); 
      strcpy(argval, token);
      insertargs(&Head, arg_, argval);
      flag=0;
      token = strtok(NULL, " ");
      }
    
  }
}

int find_name(struct argcmd *head, char *filename)
{

char *left=malloc(sizeof(char)*300);
int count=0;
  struct argcmd *current = head;
   char *arg= malloc(sizeof(char) * 30);
  while (current != NULL)
  {
     if (strcmp(current->arg, "-name") == 0)
    {
      
   if(strstr(current->argvalue,"'*")) //Asterisco lado esquerdo
    {
      strcpy(arg,current->argvalue);
        char *token, *tokensave;
        token=strtok_r(arg,"'",&tokensave);
        {
          count=0;
          while(token!=NULL)
          {
             if( compareNameRight(token,filename)==1)
       {
         return 1;
       }
            token=strtok_r(NULL,"'",&tokensave);
          }
        }
    }
    if(strstr(current->argvalue,"*'")) //asterisco lado direito
    {
        strcpy(arg,current->argvalue); 
        char *token, *tokensave;
        token=strtok_r(arg,"'",&tokensave);
        {
          while(token!=NULL)
          {
            if( compareNameLeft(token,filename)==1)
       {
         return 1;
       }
            token=strtok_r(NULL,"'",&tokensave);
          }
        }
    }
    if (strcmp(current->argvalue, filename) == 0)     //Se nao tiver asterisco compara o nome todo
    {
      return 1;
    }
    }

    current = current->nextarg;
  }
  return 0;
}

int compareNameLeft(char *argname, char*filename)
{
   //printf("%s|%s\n",argname,filename);
int i;
int count=0;
for(i=0; i<strlen(argname)-1;i++)
{
  if(filename[i]==argname[i])
  {
    count ++;
  }
}
if(count==strlen(argname)-1)
{
  return 1;
}
return 0;
}

int compareNameRight(char *argname, char*filename)
{
//printf("%s|%s\n",argname,filename);
int j=strlen(filename)-1;
int i;
int count=0;
for(i=strlen(argname)-1; i>0;i--)
{
 
    if(filename[j]==argname[i])
  {
   
    count ++;
  }
  j--;
  
  
}
if(count==strlen(argname)-1)
{
  return 1;
}
return 0;
}

int find_iname(struct argcmd *head, char *filename)
{

  struct argcmd *current = head;

  while (current != NULL)
  {

    if (strcmp(current->arg, "-iname") == 0)
    {

      char *argval = malloc(sizeof(char) * 30);
      argval = current->argvalue;
      argval = minuscula(argval);
      filename = minuscula(filename);

      if (strcmp(argval, filename) == 0)      //Compara ambas as strings minusculas 
      {

        return 1;
      }
    }
    current = current->nextarg;
  }
  return 0;
}

char *minuscula(char *string) //Funcao que transforma qlq elemento de string para minusculo
{
  int i;
  char *string1 = malloc(sizeof(char) * 300);
  char c, result;
  for (i = 0; string[i] != '\0'; i++)
  {

    c = string[i];
    result = tolower(c);
    strncat(string1, &result, 1);
  }
  //printf("%s\n",string);
  return string1;
}

int find_bynmin(struct argcmd *head, time_t tmin)
{

time_t t= time(NULL);
struct tm tm=*localtime(&t); //Tempo atual
                                   
time_t tfile=tmin;
tm=*localtime(&tfile);     //Tempo da modificaçao do ficheiro
                      
double seconds= difftime(t,tmin);   //Quantidade de segundos de diferença entre o tempo atual e o de modificaçao
double minutes= seconds/60.0;       //Transformar em minutos

struct argcmd *current = head;

  while (current != NULL)
  {
    if (strcmp(current->arg, "-mmin") == 0)
    {
     double minutos;
      sscanf(current->argvalue, "%*[^0123456789]%lf",&minutos); //Numero de minutos passados pelo utilizador
     
      if(minutes<minutos)
      {
        
        return 1;
      }
    }
    current = current->nextarg;
  }

return 0;

}

int find_bysize(struct argcmd *head, float sizebytes)
{
float sizemb=sizebytes/1000000; //Tamanho do ficheiro em mega bytes

struct argcmd *current = head;
float sizebyuser;
  
  while (current != NULL)
  {

    if (strcmp(current->arg, "-size") == 0)
    {
      sscanf(current->argvalue, "%*[^0123456789]%f",&sizebyuser); //Numero de butes passados pelo utilizador
      
          if(strstr(current->argvalue,"+"))     //Se tiver um +
          {
            
                  if(sizemb>sizebyuser)
                  {
                    return 1;
                  }
          }
          if(strstr(current->argvalue,"-"))       //Se tiver um -
          {
              if(sizemb<sizebyuser)
                  {
                    return 1;
                  }
          }
    }
    current = current->nextarg;
  }
  return 0;
}




int find_typehelper(struct argcmd *head)
{

  struct argcmd *current = head;

  while (current != NULL)
  {

    if (strcmp(current->arg, "-type") == 0)
    {

      if(strcmp(current->argvalue, "d") == 0) //-type f -> retorna 1;
      {
          return 1;
      }

      if (strcmp(current->argvalue,"f") == 0) // -type d -> retorna 2;
      {

        return 2;
      }
    }
    current = current->nextarg;
  }
  return 0;
}

int find_byempty(char *base_path)
{
  
   DIR *dir;
  struct dirent *entry;
  struct stat file_stat;
  char *path = malloc(sizeof(char) * 300);
  int count=0;
  if ((dir = opendir(base_path)) == NULL)
    return 0;
  else
  {
  
    while ((entry = readdir(dir)) != NULL)
    {
    

      sprintf(path, "%s%s", base_path, entry->d_name);

      if (stat(path, &file_stat) == 0)
      {
          continue;
      }
      else
      {
         count ++;
      }
      

      strcpy(path, "");
    }

    closedir(dir);
  }
  if(count==2) //Se count == 2 (por causa do . e  ..) significa que o diretorio está vazio.
  {
   
    return 1;
  }
  
  return 0;
}

int nodecount(struct argcmd *head) //funcao para contar as especificações
{
int count=0;

struct argcmd *current = head;

  while (current != NULL)
  {
//printf("%s->%s\n",current->arg, current->argvalue);
  count ++;
  current = current->nextarg;
  }

return count;
}


void inserthreads(struct threadmatch **head, struct threadmatch *no,long int id,char *path)
{
  //printf("%s\n",path);
   struct threadmatch *node=no;
   if(node==NULL)                                                                                       //Se a lista tiver vazio, coloca a primeira thread
   {
     struct threadmatch *new_node = (struct threadmatch *)malloc(sizeof(struct threadmatch));

  new_node->thread_id=id;
  new_node->matches=1;
  new_node->next = *head;
  *head = new_node;
  inserthreadspath(HeadThreds,id,path);     

   }
    else                                                                    //Se nao tiver vazia
    { 
      while(node!=NULL )  
   {
     if(node->thread_id==id)                                                  //Percorre a lista para ver se aquela thread ja existe lá
     {
    node->matches=(node->matches)+1;                                          //Se existir adiciona +1 ocorrencia
   inserthreadspath(HeadThreds,id,path);                  
     return;
     }
     node=node->next;
   }
   if(node==NULL)
   {
     
  struct threadmatch *new_node = (struct threadmatch *)malloc(sizeof(struct threadmatch));    //Se nao existir é colocada uma nova thread à cabeça.
  new_node->thread_id=id;
  new_node->matches=1;
  new_node->next = *head;
  *head = new_node;
   inserthreadspath(HeadThreds,id,path);
 
     
   }
    }
   }
   
   void inserthreadspath(struct threadmatch *no,long int id,char *path)
   {
    struct strings *new_string = (struct strings *)malloc(sizeof(struct strings));
   
  while(no!=NULL)
    {
      if(no->thread_id==id){  //Encontra a thread desejada

    new_string->name= malloc(300*sizeof(char)); //Aloca espaço para uma string
    strcpy(new_string->name,path);              //Coloca a string na lista.
    
    new_string->next=no->firstString;
   
    no->firstString=new_string;

    }
    no=no->next; //proxima tarefa
    }
    
   }
   
void printListArgshelp()
{
  int i=0;
  struct threadmatch *node=HeadThreds;
  while (node != NULL)
  {
    
    int counter=0;
     printf("TID:%ld\nMatchs:%d\n", node->thread_id, node->matches);    //Imprime id e counter
    
    struct strings *node_string = node->firstString;        //Apontador para a lista ligada dentro da lista
    
for(int i=0; i<node->matches;i++)           //Percorre todos os caminhos encontrados pela thread
{
printf("%s\n",node_string->name);         //Imprime
node_string=node_string->next;
}
      node = node->next;
      printf("\n");
    }
    }