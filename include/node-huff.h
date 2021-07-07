#include <string>
using namespace std;
struct node_huff
{
    node_huff *esquerda, *direita;
    long int frequencia;
    unsigned char caracter;
    string bit;
};