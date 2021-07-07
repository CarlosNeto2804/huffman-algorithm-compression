#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;


struct node_res{
    node_res *zero,*um;
    unsigned char caractere;
};


long int ler_tamanho_arquivo(unsigned char&,int,FILE*);
void escrever_nome_arquivo(char*,int,unsigned char&,int&,node_res*,FILE*);
void parse_arquivo(char*,long int,unsigned char&,int&,node_res*,FILE*);
unsigned char bits_para_numero(unsigned char&,int,FILE*);
void bits_para_string(unsigned char&,int,int&,FILE*,node_res*,unsigned char);
void resetar_arvore(node_res*);

int descompactar(char*);