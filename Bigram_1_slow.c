#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define HSIZE 1021

typedef struct Node *nodePointer;
typedef struct Node{
    char bigram[2][100];
    int count;
    nodePointer next;
}Node;

void my_strcpy(char *s1,char *s2){strcpy(s1,s2);}
int my_strlen(char *s){return strlen(s);}
int my_strcmp(char*s1,char *s2){return strcmp(s1,s2);}
int my_isalpha(int _c){return isalpha(_c);} //built-in fucntion, to see in profile.

void lowercase(char *s);
void punctuation(char *s);
char **word_extraction(char *filename);
unsigned int hash(char *word_1,char*word_2);
void add_hash_table(Node **hash_table,char *word1,char *word2, int index);
void make_hash_table(Node **hash_table,char **words);
void hash_table_to_array(nodePointer *hash_table, nodePointer *hash_array, int *node_count);
void insertion_sort(nodePointer *hash_array,int node_count);
void print_bigram(nodePointer *hash_array);




int main()
{
    char filename[30]="war_and_peace.txt";
    char **words=word_extraction(filename);

    Node **hash_table=(Node **)malloc(sizeof(Node*)*1021);
    make_hash_table(hash_table,words);

    Node **hash_array=(Node **)malloc(sizeof(Node*)*1000000);
    int node_count=0;
    hash_table_to_array(hash_table,hash_array,&node_count);

    insertion_sort(hash_array,node_count);

    print_bigram(hash_array);

    for (int i = 0; words[i] != NULL; i++) free(words[i]);
    free(words);

    for (int i = 0; i < HSIZE; i++) {
        nodePointer current = hash_table[i];
        while (current) {
            nodePointer temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(hash_table);
    free(hash_array);

    return 0;
}


void lowercase(char *s)
{
    //int n=strlen(s);
    for (int i=0;i<my_strlen(s);i++){
        if (s[i]>='A'&&s[i]<='Z'){
            s[i]+=('a'-'A');
        }
    }
}

void punctuation(char *s)
{
    int i,j=0;
    char temp[my_strlen(s)+1];
    for (i=0;i<my_strlen(s);i++){
        if (my_isalpha(s[i])){
            temp[j]=s[i];
            ++j;
        }
    }
    temp[j]='\0';
    my_strcpy(s,temp);
}

char **word_extraction(char *filename)
{
    FILE *file=fopen(filename,"r");
    if (!file){
        fprintf(stderr,"File open error!\n");
        exit(1);
    }

    char **words=(char**)malloc(sizeof(char*)*1000000); //bigram size=1000000
    if (!words){
        fprintf(stderr,"Memory allocation failed!\n");
        fclose(file);
        exit(1);
    }

    int i=0;
    char word[100]; //word size=100
    while (fscanf(file,"%99s",word)==1){
        lowercase(word);
        punctuation(word);
        if (my_strlen(word)==0){continue;}

        words[i]=(char*)malloc(100*sizeof(char));
        if (!words[i]){
            fprintf(stderr,"Memory allocation failed!\n");
            fclose(file);
            free(words);
            exit(1);
        }
        my_strcpy(words[i],word);
        ++i;
        if(i>=1000000){break;}
    }
    words[i]=NULL;
    fclose(file);
    return words;
}

unsigned int hash(char *word1,char *word2)
{
    unsigned long hval=0;
    for (;*word1!='\0';word1++){
        hval=hval*31+*word1;
    }
    for (;*word2!='\0';word2++){
        hval=hval*31+*word2;
    }


    return hval%HSIZE;
}

void add_hash_table(Node **hash_table,char *word1,char *word2, int index)
{
    nodePointer current=hash_table[index];
    if (!current){
        nodePointer temp=(nodePointer)malloc(sizeof(Node));
        my_strcpy(temp->bigram[0],word1);
        my_strcpy(temp->bigram[1],word2);
        temp->count=1;
        temp->next=NULL;
        hash_table[index]=temp;
        return;
    }
    else{
        while(current!=NULL){
            if(my_strcmp(current->bigram[0],word1)==0 && my_strcmp(current->bigram[1],word2)==0){
                ++current->count;
                return;
            }
            if (current->next==NULL){break;}
            current=current->next;
        }

        nodePointer temp=(nodePointer)malloc(sizeof(Node));
        my_strcpy(temp->bigram[0],word1);
        my_strcpy(temp->bigram[1],word2);
        temp->count=1;
        temp->next=NULL;
        current->next=temp;
    }
}

void make_hash_table(Node **hash_table,char **words)
{
    for (int i=0;words[i+1]!=NULL;i++){
        int index=hash(words[i],words[i+1]);
        add_hash_table(hash_table,words[i],words[i+1],index);
    }
}

void hash_table_to_array(nodePointer *hash_table, nodePointer *hash_array, int *node_count)
{
    int index=0;
    for (int i=0;i<HSIZE;i++){
        nodePointer current=hash_table[i];
        while(current!=NULL){
            hash_array[index++]=current;
            current=current->next;
        }
    }
    *node_count=index;
}

void insertion_sort(nodePointer *hash_array,int node_count)
{
    for (int i=1;i<node_count;i++){
        nodePointer key=hash_array[i];
        int j=i-1;
        while(j>=0 && ((hash_array[j]->count) < (key->count))){
            hash_array[j+1]=hash_array[j];
            --j;
        }
        hash_array[j+1]=key;
    }
}

void print_bigram(nodePointer *hash_array)
{
    printf("20 most frequent bigrams:\n");
    for (int i=0;i<20;i++){
        if (hash_array[i]==NULL){break;}
        printf("%d. %3s %3s / frequency: %d\n", i+1, hash_array[i]->bigram[0],hash_array[i]->bigram[1],
        hash_array[i]->count);
    }
}