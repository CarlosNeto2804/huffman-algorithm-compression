#include "../include/compactar.h"

bool node_huffcompare(node_huff a, node_huff b)
{
    return a.frequencia < b.frequencia;
}

int compactar(int argc, char *argv[])
{
    long int number[256];
    long int total_bits = 0;
    int total_letras = 0;

    for (long int *i = number; i < number + 256; i++)
        *i = 0;
    

    string nome_arquivo;
    FILE *arquivo_original, *arquivo_comprimido;

    nome_arquivo = argv[1];
    nome_arquivo += ".freq";

    unsigned char *x_p, x;
    x_p = &x;
    long int tamanho_total = 0, tamanho;
    total_bits += 16 + 9 * (argc - 1);
    for (int current_file = 1; current_file < argc; current_file++)
    {

        for (char *c = argv[current_file]; *c; c++)
            number[(unsigned char)(*c)]++;
        

        tamanho_total += tamanho = tamanho_do_arquivo(argv[current_file]);
        total_bits += 64;

        arquivo_original = fopen(argv[current_file], "rb");
        fread(x_p, 1, 1, arquivo_original);
        for (long int j = 0; j < tamanho; j++)
        { 
            number[x]++;
            fread(x_p, 1, 1, arquivo_original);
        }
        fclose(arquivo_original);
    }

    for (long int *i = number; i < number + 256; i++)
    {
        if (*i)
        {
            total_letras++;
        }
    }

    node_huff array[total_letras * 2 - 1];
    node_huff *node = array;

    for (long int *i = number; i < number + 256; i++)
    {
        if (*i)
        {
            node->direita = NULL;
            node->esquerda = NULL;
            node->frequencia = *i;
            node->caracter = i - number;
            node++;
        }
    }
    sort(array, array + total_letras, node_huffcompare);

    node_huff
        *min1 = array,
        *min2 = array + 1,
        *node_atual = array + total_letras,
        *nao_folha = array + total_letras,
        *folha = array + 2;

    for (int i = 0; i < total_letras - 1; i++)
    {

        node_atual->frequencia = min1->frequencia + min2->frequencia;
        node_atual->esquerda = min1;
        node_atual->direita = min2;
        min1->bit = "1";
        min2->bit = "0";
        node_atual++;

        if (folha >= array + total_letras)
        {
            min1 = nao_folha;
            nao_folha++;
        }
        else
        {
            if (folha->frequencia < nao_folha->frequencia)
            {
                min1 = folha;
                folha++;
            }
            else
            {
                min1 = nao_folha;
                nao_folha++;
            }
        }

        if (folha >= array + total_letras)
        {
            min2 = nao_folha;
            nao_folha++;
        }
        else if (nao_folha >= node_atual)
        {
            min2 = folha;
            folha++;
        }
        else
        {
            if (folha->frequencia < nao_folha->frequencia)
            {
                min2 = folha;
                folha++;
            }
            else
            {
                min2 = nao_folha;
                nao_folha++;
            }
        }
    }

    for (node = array + total_letras * 2 - 2; node > array - 1; node--)
    {
        if (node->esquerda)
        {
            node->esquerda->bit = node->bit + node->esquerda->bit;
        }
        if (node->direita)
        {
            node->direita->bit = node->bit + node->direita->bit;
        }
    }

    arquivo_comprimido = fopen(&nome_arquivo[0], "wb");
    int bit_atual_cont = 0;
    unsigned char byte_atual;

    fwrite(&total_letras, 1, 1, arquivo_comprimido);
    total_bits += 8;

    char *str_pointer;
    unsigned char len, current_caracter;
    string str_arr[256];
    for (node = array; node < array + total_letras; node++)
    {
        str_arr[(node->caracter)] = node->bit; 
        len = node->bit.length();
        current_caracter = node->caracter;

        escrever_uChar(current_caracter, byte_atual, bit_atual_cont, arquivo_comprimido);
        escrever_uChar(len, byte_atual, bit_atual_cont, arquivo_comprimido);
        total_bits += len + 16;


        str_pointer = &node->bit[0];
        while (*str_pointer)
        {
            if (bit_atual_cont == 8)
            {
                fwrite(&byte_atual, 1, 1, arquivo_comprimido);
                bit_atual_cont = 0;
            }
            switch (*str_pointer)
            {
            case '1':
                byte_atual <<= 1;
                byte_atual |= 1;
                bit_atual_cont++;
                break;
            case '0':
                byte_atual <<= 1;
                bit_atual_cont++;
                break;
            default:
                cout << "Deu ruim" << endl;
                fclose(arquivo_comprimido);
                remove(&nome_arquivo[0]);
                return 1;
            }
            str_pointer++;
        }

        total_bits += len * (node->frequencia);
    }

    if (total_bits % 8)
    {
        total_bits = (total_bits / 8 + 1) * 8;
    }


    
    escrever_contagem_arquivo(argc - 1, byte_atual, bit_atual_cont, arquivo_comprimido);


    for (int current_file = 1; current_file < argc; current_file++)
    {

        arquivo_original = fopen(argv[current_file], "rb");
        fseek(arquivo_original, 0, SEEK_END);
        tamanho = ftell(arquivo_original);
        rewind(arquivo_original);

        //-------------writes fifth--------------
        if (bit_atual_cont == 8)
        {
            fwrite(&byte_atual, 1, 1, arquivo_comprimido);
            bit_atual_cont = 0;
        }
        byte_atual <<= 1;
        byte_atual |= 1;
        bit_atual_cont++;
        //---------------------------------------

        escrever_tamaho_arquivo(tamanho, byte_atual, bit_atual_cont, arquivo_comprimido);                             
        escrever_nome_arquivo(argv[current_file], str_arr, byte_atual, bit_atual_cont, arquivo_comprimido);       
        escrever_arquivo(arquivo_original, tamanho, str_arr, byte_atual, bit_atual_cont, arquivo_comprimido);
        fclose(arquivo_original);
    }

    if (bit_atual_cont == 8)
    { // here we are writing the last byte of the file
        fwrite(&byte_atual, 1, 1, arquivo_comprimido);
    }
    else
    {
        byte_atual <<= 8 - bit_atual_cont;
        fwrite(&byte_atual, 1, 1, arquivo_comprimido);
    }

    fclose(arquivo_comprimido);
   

    cout<<"Tamanho Original: "<<tamanho_total * 8<<" bytes"<<endl;
    cout<<"Tamanho compactado:"<<total_bits/8<<" bytes"<<endl;
    cout<<"O arquivo compactado será [%"<< (100*(total_bits/8)) /( tamanho_total * 8)<<"] do original"<<endl;

    cout << "Compactado! " << endl;
}
void escrever_uChar(unsigned char uChar, unsigned char &byte_atual, int bit_atual_cont, FILE *fp_write)
{
    byte_atual <<= 8 - bit_atual_cont;
    byte_atual |= (uChar >> bit_atual_cont);
    fwrite(&byte_atual, 1, 1, fp_write);
    byte_atual = uChar;
}

void escrever_contagem_arquivo(int file_count, unsigned char &byte_atual, int bit_atual_cont, FILE *arquivo_comprimido)
{
    unsigned char temp = file_count % 256;
    escrever_uChar(temp, byte_atual, bit_atual_cont, arquivo_comprimido);
    temp = file_count / 256;
    escrever_uChar(temp, byte_atual, bit_atual_cont, arquivo_comprimido);
}

void escrever_tamaho_arquivo(long int tamanho, unsigned char &byte_atual, int bit_atual_cont, FILE *arquivo_comprimido)
{
    for (int i = 0; i < 8; i++)
    {
        escrever_uChar(tamanho % 256, byte_atual, bit_atual_cont, arquivo_comprimido);
        tamanho /= 256;
    }
}

void escrever_nome_arquivo(char *nome_arquivo, string *str_arr, unsigned char &byte_atual, int &bit_atual_cont, FILE *arquivo_comprimido)
{
    escrever_uChar(strlen(nome_arquivo), byte_atual, bit_atual_cont, arquivo_comprimido);
    char *str_pointer;
    for (char *c = nome_arquivo; *c; c++)
    {
        str_pointer = &str_arr[(unsigned char)(*c)][0];
        while (*str_pointer)
        {
            if (bit_atual_cont == 8)
            {
                fwrite(&byte_atual, 1, 1, arquivo_comprimido);
                bit_atual_cont = 0;
            }
            switch (*str_pointer)
            {
            case '1':
                byte_atual <<= 1;
                byte_atual |= 1;
                bit_atual_cont++;
                break;
            case '0':
                byte_atual <<= 1;
                bit_atual_cont++;
                break;
            default:
                cout << "Ocorreu um erro" << endl;
                exit(2);
            }
            str_pointer++;
        }
    }
}

void escrever_arquivo(FILE *arquivo_original, long int tamanho, string *str_arr, unsigned char &byte_atual, int &bit_atual_cont, FILE *arquivo_comprimido)
{
    unsigned char *x_p, x;
    x_p = &x;

    char *str_pointer;
    fread(x_p, 1, 1, arquivo_original);
    for (long int i = 0; i < tamanho; i++)
    {
        str_pointer = &str_arr[x][0];
        while (*str_pointer)
        {
            if (bit_atual_cont == 8)
            {
                fwrite(&byte_atual, 1, 1, arquivo_comprimido);
                bit_atual_cont = 0;
            }
            switch (*str_pointer)
            {
            case '1':
                byte_atual <<= 1;
                byte_atual |= 1;
                bit_atual_cont++;
                break;
            case '0':
                byte_atual <<= 1;
                bit_atual_cont++;
                break;
            default:
                cout << "Deu ruim" << endl;
                exit(2);
            }
            str_pointer++;
        }
        fread(x_p, 1, 1, arquivo_original);
    }
}

long int tamanho_do_arquivo(char *path)
{
    long int tamanho;
    FILE *fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    tamanho = ftell(fp);
    fclose(fp);
    return tamanho;
}
