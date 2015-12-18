#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int id = 0;
int main()
{

     char **tab = (char**)malloc(10*sizeof(char*));
     for(int i=0;i<10;i++)
        tab[i] = (char*)malloc(7*sizeof(char));
     int *tab1 = (int*)malloc(10*sizeof(int));
     char tmp[100];
     char onion[200];
     for(int i=0;i<100;i++)
       onion[i] = random();
    /* for(int i=0;i<10;i++)
     {
       sprintf(tmp,"toto%d",i);
       //tab[i] = tmp;
       strcpy(tab[i],tmp);
       tab1[i] = i;
       //printf("%s\n",tab[i]);
     }
     
    for(int i=0;i<10;i++)
    {
      printf("tab[%d] = %s\n",i,tab[i]);
      //printf("tab1[%d] = %d\n",i,tab1[i]);
    }*/
    strcat(onion,"|"); 
    sprintf(tmp,"toto/tata");
    strcat(onion,tmp);
    //printf("Onion = %s\n",onion);
    printf("Onion = %s\n",onion);
    char* tmp1 = strtok(onion,"|");
    char* res1 = strtok(NULL,"|");
    char* res2 = strtok(NULL,"");
    char **mylist = (char**)malloc(10*sizeof(char*));
    
    /*int p = 0;
    while(tmp1!=NULL)
    {
       tmp1 = strtok(NULL,"/");
       printf("tmp1 = %s\n",tmp1);
       mylist[p] = tmp1;
       p++;
    }*/

    /*for(int i=0;i<5;i++)
      printf("mylist[%d] = %s\n",i,mylist[i]);*/
    /*char *essai1 = (char*)malloc(10*sizeof(char));
    char essai2[10];
    printf("essai1 = %ld\n",sizeof(essai1));
    printf("essai2 = %ld\n",sizeof(essai2));*/
    /*printf("tmp1 = %s\n",tmp1);
    printf("res1 = %s\n",res1);
    printf("res2 = %s\n",res2);*/
    int d = ++id;
    d = ++id;
    printf("d = %d\n",d);
    printf("id = %d\n",id);
    return 0;
}
