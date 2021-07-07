#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <dirent.h>

#include "./node-huff.h"


using namespace std;

long int tamanho_do_arquivo(char *);
void escrever_uChar(unsigned char, unsigned char &, int, FILE *);
void escrever_contagem_arquivo(int, unsigned char &, int, FILE *);
void escrever_tamaho_arquivo(long int, unsigned char &, int, FILE *);
void escrever_nome_arquivo(char *, string *, unsigned char &, int &, FILE *);
void escrever_arquivo(FILE *, long int, string *, unsigned char &, int &, FILE *);
int compactar(int, char *[]);