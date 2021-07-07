
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "../include/compactar.h"
#include "../include/descompactar.h"
using namespace std;

void renomear_arquivo_saida(){
  FILE *novo, *velho;
  novo=fopen("entrada.defreq","w");
  velho=fopen("entrada" ,"r");
  char letra;
  while( (letra = fgetc(velho)) != EOF )
    fprintf(novo, "%c", letra);
    
  fclose(velho);
  fclose(novo);
}

int main(int argcs, char *argv[])
{
  compactar(argcs, argv);
  string nome_saida_comp = argv[1];
  
  descompactar((char*)"./entrada.freq");

  renomear_arquivo_saida();
  
  return 0;
}

