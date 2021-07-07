
#include "../include/descompactar.h"

const unsigned char check = 0b10000000;

int descompactar(char *argv)
{
    int total_letras = 0;
    FILE *arquivo_compactado;
    arquivo_compactado = fopen(argv, "rb");

    fread(&total_letras, 1, 1, arquivo_compactado);
    if (total_letras == 0)
        total_letras = 256;

    unsigned char byte_atual = 0, letra_atual;
    int bit_atual_cont = 0, len;
    node_res *raiz = new node_res;
    raiz->zero = NULL;
    raiz->um = NULL;

    for (int i = 0; i < total_letras; i++)
    {
        letra_atual = bits_para_numero(byte_atual, bit_atual_cont, arquivo_compactado);
        len = bits_para_numero(byte_atual, bit_atual_cont, arquivo_compactado);
        if (len == 0)
            len = 256;
        bits_para_string(byte_atual, len, bit_atual_cont, arquivo_compactado, raiz, letra_atual);
    }

    int arq_cont;
    arq_cont = bits_para_numero(byte_atual, bit_atual_cont, arquivo_compactado);
    arq_cont += 256 * bits_para_numero(byte_atual, bit_atual_cont, arquivo_compactado);

    for (int current_file = 0; current_file < arq_cont; current_file++)
    {

        byte_atual <<= 1;
        bit_atual_cont--;
        long int tamanho = ler_tamanho_arquivo(byte_atual, bit_atual_cont, arquivo_compactado);

        int tamanho_nome_arquivo = bits_para_numero(byte_atual, bit_atual_cont, arquivo_compactado);
        char novo_arqv[tamanho_nome_arquivo + 4];
        escrever_nome_arquivo(novo_arqv, tamanho_nome_arquivo, byte_atual, bit_atual_cont, raiz, arquivo_compactado);

        parse_arquivo(novo_arqv, tamanho, byte_atual, bit_atual_cont, raiz, arquivo_compactado);
    }

    fclose(arquivo_compactado);
    resetar_arvore(raiz);
    cout << "Descompactado" << endl;
}

void resetar_arvore(node_res *node)
{
    if (node->zero)
        resetar_arvore(node->zero);
    if (node->um)
        resetar_arvore(node->um);
    delete node;
}

void bits_para_string(unsigned char &byte_atual, int n, int &bit_atual_cont, FILE *fp_read, node_res *node, unsigned char uChar)
{
    for (int i = 0; i < n; i++)
    {
        if (bit_atual_cont == 0)
        {
            fread(&byte_atual, 1, 1, fp_read);
            bit_atual_cont = 8;
        }

        switch (byte_atual & check)
        {
        case 0:
            if (!(node->zero))
            {
                node->zero = new node_res;
                node->zero->zero = NULL;
                node->zero->um = NULL;
            }
            node = node->zero;
            break;
        case 128:
            if (!(node->um))
            {
                node->um = new node_res;
                node->um->zero = NULL;
                node->um->um = NULL;
            }
            node = node->um;
            break;
        }
        byte_atual <<= 1;
        bit_atual_cont--;
    }
    node->caractere = uChar;
}

unsigned char bits_para_numero(unsigned char &byte_atual, int bit_atual_cont, FILE *fp_read)
{
    unsigned char val, byte_aux;
    fread(&byte_aux, 1, 1, fp_read);
    val = byte_atual | (byte_aux >> bit_atual_cont);
    byte_atual = byte_aux << (8 - bit_atual_cont);
    return val;
}

long int ler_tamanho_arquivo(unsigned char &byte_atual, int bit_atual_cont, FILE *arquivo_compactado)
{
    long int size = 0;
    {
        long int multiplier = 1;
        for (int i = 0; i < 8; i++)
        {
            size += bits_para_numero(byte_atual, bit_atual_cont, arquivo_compactado) * multiplier;
            multiplier *= 256;
        }
    }
    return size;
}

void escrever_nome_arquivo(char *novo_arqv, int tamanho_nome_arquivo, unsigned char &byte_atual, int &bit_atual_cont, node_res *raiz, FILE *arquivo_compactado)
{
    node_res *node;
    novo_arqv[tamanho_nome_arquivo] = 0;
    for (int i = 0; i < tamanho_nome_arquivo; i++)
    {
        node = raiz;
        while (node->zero || node->um)
        {
            if (bit_atual_cont == 0)
            {
                fread(&byte_atual, 1, 1, arquivo_compactado);
                bit_atual_cont = 8;
            }
            if (byte_atual & check)
            {
                node = node->um;
            }
            else
            {
                node = node->zero;
            }
            byte_atual <<= 1;
            bit_atual_cont--;
        }
        novo_arqv[i] = node->caractere;
    }
}

void parse_arquivo(char *path, long int size, unsigned char &byte_atual, int &bit_atual_cont, node_res *raiz, FILE *arquivo_compactado)
{
    node_res *node;
    FILE *fp_new = fopen(path, "wb");
    for (long int i = 0; i < size; i++)
    {
        node = raiz;
        while (node->zero || node->um)
        {
            if (bit_atual_cont == 0)
            {
                fread(&byte_atual, 1, 1, arquivo_compactado);
                bit_atual_cont = 8;
            }
            if (byte_atual & check)
            {
                node = node->um;
            }
            else
            {
                node = node->zero;
            }
            byte_atual <<= 1;
            bit_atual_cont--;
        }
        fwrite(&(node->caractere), 1, 1, fp_new);
    }
    fclose(fp_new);
}