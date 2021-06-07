#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum {
    INT,
    RETURN,
    MENOS,
    ID,
    NUM,
    NAO_IDENTIFICADO,
    ABRE_CHAVES,
    FECHA_CHAVES,
    FECHA_PARENTESES,
    ESPACO,
    SOMA,
    MAIN,
    PONTO_E_VIRGULA,
    ABRE_PARENTESES,
    IGUALDADE_LOGICA,
    ATRIBUICAO,
} TIPO;

typedef enum {
    CARACTER,
    CADEIA_CARACTER,
    INTEIRO
} VARIAVELTIPO;


int c = 0, l = 1;
typedef struct Token_TED {

    TIPO tipo;
    union {
        int tipo_inteiro;
        char tipo_cadeia_caracter[100];
        char caracter;
    }valor;

    int l;
    int c;
}tk;

#define true  1
#define false 0

typedef struct {
    VARIAVELTIPO tipo;
    union {
        int tipo_inteiro;
        char tipo_cadeia_caracter[100];
        char caracter;
    }value;
    char representacao[50];
}Variaveis;

Variaveis array_vars[50];
int quantidade_de_valores_na_pilha = 0;


int tamanhoExpressao = 0;
int tamanho = 0;
char expressaoTextoUnitario[50];
FILE* arq;
int isFinal = false;
char variavel_auxiliar[50];
int regras[] = { 0,0,0 }; 
tk tk_lido;
tk lerCaracter();
//funcoes criadas em sala
void montar_codigo_inicial();
void montar_codigo_retorno();
void montar_codigo_final();
//funcoes referente a gramatica
void obter_token();
void inicio();
void corpo();
void printarERRO();
void construir_inteiro(char* id);
void declaracoes();
void receber_attr();
void stmt();
tk primary_exp();
void lado_direito();
char* makeStack();
void push(char* p, char item);
char pop(char* p);
int Verificacao_prioridade(char c, char t);
void infixaParaPos(char expr[]);
void montar_assembly_de_posicao(char text[]);

int main() {

    //A funcao main significa a primeira regra da gramatica no caso
    inicio();
    return 0;
}

void construir_inteiro(char* id) {
    fprintf(arq, "    subq $8, %%rsp\n");
}

void declaracoes() {
    regras[0] = 1;
    fprintf(arq, "    pushq	%%rbp\n");
    fprintf(arq, "    movq	%%rsp, %%rbp  \n\n");
}

void receber_attr() {
    for (int c = 0; c < quantidade_de_valores_na_pilha; c++) {
        if (strcmp(variavel_auxiliar, array_vars[c].representacao) == 0) {


            fprintf(arq, "\n    movq -%d(%%rbp), %%rax\n", (quantidade_de_valores_na_pilha + 1) * 8);
            
            //q eh referente ao tamanho em assembly
            fprintf(arq, "\n    movq %%rax, -%d(%%rbp)\n", (c + 1) * 8);
            
            fprintf(arq, "    addq $8, %%rsp\n");
            
            
            break;
        }
    }
}

void stmt() {
    primary_exp();
    lado_direito();
}

tk primary_exp() {
    if (tk_lido.tipo == NUM) {


        int bc = tk_lido.valor.tipo_inteiro;
        char local_char[2];
        local_char[0] = bc + '0';
        local_char[1] = '\0';

        strcat(expressaoTextoUnitario, local_char);
        tk aux = tk_lido;
        tk_lido = lerCaracter();
        return aux;
    }
    if (tk_lido.tipo == ID) {
        int c = 0;
        for (c = 0; c < quantidade_de_valores_na_pilha; c++) {
            if (strcmp(tk_lido.valor.tipo_cadeia_caracter, array_vars[c].representacao) == 0) {
                strcat(expressaoTextoUnitario, ".");
                strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
                strcat(expressaoTextoUnitario, ",");
                tk_lido.valor.tipo_inteiro = c + 1;
                break;
            }
        }
        tk aux = tk_lido;
        tk_lido = lerCaracter();
        return aux;
    }
}

void lado_direito() {

            int i, j, k;
    if (tk_lido.tipo == SOMA) {
        strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
        tk_lido = lerCaracter();

        if (tk_lido.tipo == ABRE_PARENTESES) {
            i = false;
            j = 0;
            k = true;

            strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
            tk_lido = lerCaracter();
            stmt();
            if (tk_lido.tipo == FECHA_PARENTESES) {
                strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
                tk_lido = lerCaracter();
            }
            else(FECHA_PARENTESES);
        }
        else {
            i = 0;
            j = false;
            k = 0;

            primary_exp();
            stmt();
        }

    }
    else if (tk_lido.tipo == MENOS) {
        strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
        tk_lido = lerCaracter();
        if (tk_lido.tipo == ABRE_PARENTESES) {
            strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
            tk_lido = lerCaracter();
            stmt();
            if (tk_lido.tipo == FECHA_PARENTESES) {
                strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
                tk_lido = lerCaracter();
            }
            else(FECHA_PARENTESES);
        }
        else {
            primary_exp();
            stmt();
        }
    }


    else if (tk_lido.tipo == IGUALDADE_LOGICA) {
        strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
        tk_lido = lerCaracter();
        if (tk_lido.tipo == ABRE_PARENTESES) {
            strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
            tk_lido = lerCaracter();
            stmt();
            if (tk_lido.tipo == FECHA_PARENTESES) {
                strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
                tk_lido = lerCaracter();
            }
            else(FECHA_PARENTESES);
        }
        else {
            primary_exp();
            stmt();
        }
    }
    else if (tk_lido.tipo == ABRE_PARENTESES) {
        strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
        tk_lido = lerCaracter();
        stmt();
        if (tk_lido.tipo == FECHA_PARENTESES) {
            strcat(expressaoTextoUnitario, tk_lido.valor.tipo_cadeia_caracter);
            tk_lido = lerCaracter();
            lado_direito();
        }
        else(FECHA_PARENTESES);
    }
}

void inicio() {
    tk_lido = lerCaracter();
    if (tk_lido.tipo == INT) {
        tk_lido = lerCaracter();
        if (tk_lido.tipo == MAIN) {
            tk_lido = lerCaracter();
            if (tk_lido.tipo == ABRE_PARENTESES) {
                tk_lido = lerCaracter();
                if (tk_lido.tipo == FECHA_PARENTESES) {
                    tk_lido = lerCaracter();
                    if (tk_lido.tipo == ABRE_CHAVES) {
                        montar_codigo_inicial();
                        tk_lido = lerCaracter();
                        corpo();
                        if (tk_lido.tipo == FECHA_CHAVES) {
                            tk_lido = lerCaracter();
                            montar_codigo_final();
                        }
                        else printarERRO(FECHA_CHAVES);
                    }
                    else printarERRO(ABRE_CHAVES);
                }
                else printarERRO(FECHA_PARENTESES);
            }
            else printarERRO(ABRE_PARENTESES);
        }
        else printarERRO(MAIN);
    }
    else printarERRO(INT);
}

void corpo() {
    expressaoTextoUnitario[0] = '\0';
    tamanhoExpressao = 0;
    if (tk_lido.tipo == ID) {
        strcpy(variavel_auxiliar, tk_lido.valor.tipo_cadeia_caracter);
        tk_lido = lerCaracter();
        if (tk_lido.tipo == ATRIBUICAO) {
            tk_lido = lerCaracter();

            stmt();
            infixaParaPos(expressaoTextoUnitario);
            receber_attr();
            if (tk_lido.tipo == PONTO_E_VIRGULA) {
                tk_lido = lerCaracter();
                corpo();
            }
            else printarERRO(PONTO_E_VIRGULA);
        }
        else return;
    }

    if (tk_lido.tipo == INT) {
        tk_lido = lerCaracter();
        if (tk_lido.tipo == ID) {

            array_vars[quantidade_de_valores_na_pilha].tipo = INT;
            strcpy(array_vars[quantidade_de_valores_na_pilha].representacao, tk_lido.valor.tipo_cadeia_caracter);
            quantidade_de_valores_na_pilha++;
            construir_inteiro(tk_lido.valor.tipo_cadeia_caracter);
            tk_lido = lerCaracter();
            if (tk_lido.tipo == PONTO_E_VIRGULA) {
                tk_lido = lerCaracter();
                corpo();
            }
            else printarERRO(PONTO_E_VIRGULA);
        }
        else printarERRO(ID);
    };

    if (tk_lido.tipo == RETURN) {
        tk_lido = lerCaracter();
        stmt();
        infixaParaPos(expressaoTextoUnitario);
        montar_codigo_retorno();
        if (tk_lido.tipo == PONTO_E_VIRGULA) {
            tk_lido = lerCaracter();
            regras[1] = 1;
            corpo();
        }
        else printarERRO(PONTO_E_VIRGULA);
    }
    else {
        if (regras[1] == 0)
            printarERRO(RETURN);
    };
}

tk lerCaracter() {
    char buffer[100];
    int i = 0, tam = 0;
    char caractere;
    tk token_lido;
    int n = 0;
    caractere = getchar();
    c++;
    token_lido.c = c;
    token_lido.l = l;



    if (caractere == '+') {
        token_lido.tipo = SOMA;
        strcpy(token_lido.valor.tipo_cadeia_caracter, "+");
        return token_lido;
    }



    if (caractere == '=') {
        token_lido.tipo = ATRIBUICAO;
        strcpy(token_lido.valor.tipo_cadeia_caracter, "=");
        return token_lido;
    }
    if (caractere == '-') {
        token_lido.tipo = MENOS;
        strcpy(token_lido.valor.tipo_cadeia_caracter, "-");
        return token_lido;
    }
    if (caractere == '{') {
        token_lido.tipo = ABRE_CHAVES;
        strcpy(token_lido.valor.tipo_cadeia_caracter, "{");
        return token_lido;
    }
    else if (caractere == '}') {
        token_lido.tipo = FECHA_CHAVES;
        strcpy(token_lido.valor.tipo_cadeia_caracter, "}");
        return token_lido;
    }
    else if (caractere == ' ') {
        token_lido.tipo = ESPACO;
        strcpy(token_lido.valor.tipo_cadeia_caracter, " ");
        return lerCaracter();
    }
    else if (caractere == '(') {
        token_lido.tipo = ABRE_PARENTESES;
        strcpy(token_lido.valor.tipo_cadeia_caracter, "(");
        return token_lido;
    }
    else if (caractere == ')') {
        token_lido.tipo = FECHA_PARENTESES;
        strcpy(token_lido.valor.tipo_cadeia_caracter, ")");
        return token_lido;
    }
    else if (caractere == '\n') {
        token_lido.tipo = ESPACO;
        l++;
        c = 0;
        return lerCaracter();
    }
    else if (caractere == EOF) {
        token_lido.tipo = ESPACO;
        strcpy(token_lido.valor.tipo_cadeia_caracter, "\\n");
        return token_lido;
    }
    else if (caractere == ';') {
        token_lido.tipo = PONTO_E_VIRGULA;
        strcpy(token_lido.valor.tipo_cadeia_caracter, ";");
        return token_lido;
    }

    if (isalpha(caractere) || caractere == '_') {
        token_lido.c = c;
        token_lido.l = l;
        while (isalnum(caractere) || caractere == '_') {
            buffer[tam] = caractere;
            tam++;
            c++;
            caractere = getchar();
        }
        ungetc(caractere, stdin);
        buffer[tam] = '\0';

        if (strcmp(buffer, "main") == 0) {
            token_lido.tipo = MAIN;
            strcpy(token_lido.valor.tipo_cadeia_caracter, "main");

            return token_lido;
        }
        else if (strcmp(buffer, "int") == 0) {
            token_lido.tipo = INT;
            strcpy(token_lido.valor.tipo_cadeia_caracter, "int");
            return token_lido;
        }

        else if (strcmp(buffer, "return") == 0) {
            token_lido.tipo = RETURN;
            strcpy(token_lido.valor.tipo_cadeia_caracter, "return");
            return token_lido;
        }

        else {
            token_lido.tipo = ID;
            strcpy(token_lido.valor.tipo_cadeia_caracter, buffer);
            return token_lido;
        }
    }
    if (isdigit(caractere)) {
        n = n * 10;
        n += (caractere - '0');
        token_lido.c = c;
        token_lido.l = l;
        caractere = getchar();
        while (isdigit(caractere)) {
            n = n * 10;
            n += (caractere - '0');
            c++;
            caractere = getchar();
        }
        ungetc(caractere, stdin);
        token_lido.tipo = NUM;
        token_lido.valor.tipo_inteiro = n;
        return token_lido;
    }
}


void montar_codigo_inicial() {
    arq = fopen("out.s", "w+");
    fprintf(arq, "    .global _start\n\n");
    fprintf(arq, "_start:\n\n");
    declaracoes();
}

void montar_codigo_retorno() {
    fprintf(arq, "\n    movq  -%d(%%rbp), %%rbx\n", (quantidade_de_valores_na_pilha + 1) * 8);
    fprintf(arq, "\n    addq   $%d, %%rsp\n", (quantidade_de_valores_na_pilha + 1) * 8);
    fprintf(arq, "    movq   %%rbp, %%rsp\n");
    fprintf(arq, "    popq   %%rbp\n");
    fprintf(arq, "\n    movq   $1, %%rax\n");
    fprintf(arq, "    int $0x80\n\n");
}

void montar_codigo_final() {
    fclose(arq);
    printf("Arquivo \"out.s\" gerado.\n\n");
}

void printarERRO(TIPO tipo_do_erro) {
        
    //CAda codigo de erro eh conferido nessa funcao 
    //    tratamento basico de erros

    if (tipo_do_erro == INT) {

        printf("ERRO DE COMPILACAO, TOKEN INVALIDO: (%d, %d). FALTOU :'INT' >\n", tk_lido.l, tk_lido.c);
    }

    else if (tipo_do_erro == ABRE_PARENTESES) {

        printf("ERRO DE COMPILACAO, TOKEN INVALIDO: (%d, %d). FALTOU :'(' >\n", tk_lido.l, tk_lido.c);
    }
    else if (tipo_do_erro == MAIN) {

        printf("ERRO DE COMPILACAO, TOKEN INVALIDO: (%d, %d). FALTOU :'MAIN' >\n", tk_lido.l, tk_lido.c);
    }
  
    else if (tipo_do_erro == ABRE_CHAVES) {

        printf("ERRO DE COMPILACAO, TOKEN INVALIDO: (%d, %d). FALTOU :'{' >\n", tk_lido.l, tk_lido.c);
    }
    else if (tipo_do_erro == NUM) {

        printf("ERRO DE COMPILACAO, TOKEN INVALIDO: (%d, %d). FALTOU :'NUM' >\n", tk_lido.l, tk_lido.c);
    }
    else if (tipo_do_erro == RETURN) {

        printf("ERRO DE COMPILACAO, TOKEN INVALIDO: (%d, %d). FALTOU :'RETURN' >\n", tk_lido.l, tk_lido.c);
    }
 
  
    else if (tipo_do_erro == FECHA_CHAVES) {

        printf("ERRO DE COMPILACAO, TOKEN INVALIDO: (%d, %d). FALTOU :'}' >\n", tk_lido.l, tk_lido.c);
    }
    else if (tipo_do_erro == PONTO_E_VIRGULA) {

        printf("ERRO DE COMPILACAO, TOKEN INVALIDO: (%d, %d). FALTOU :';' >\n", tk_lido.l, tk_lido.c);
    }
    else if (tipo_do_erro == FECHA_PARENTESES) {

        printf("ERRO DE COMPILACAO, TOKEN INVALIDO: (%d, %d). FALTOU :')' >\n", tk_lido.l, tk_lido.c);
    }
    exit(1);
}


//Funcoes da pilha

char* makeStack() {
    char* pilha = (char*)malloc(100 * sizeof(char));
    return pilha;
}

void push(char* p, char item) {
    p[tamanho] = item;
    tamanho++;
}

char pop(char* p) {
    tamanho--;
    char aux = p[tamanho];
    return aux;
}

int Verificacao_prioridade(char char_lido, char t) {
    int x;
    int y;

    if (char_lido == '^') {

        x = 4;
    }
    else if (char_lido == '*' || char_lido == '/')
    {
        x = 2;
    }
    else if (char_lido == '+' || char_lido == '-')
    {
        x = 1;
    }
    else if (char_lido == '(')
    {
        x = 4;
    }

    if (t == '^')
    {
        y = 3;
    }
    else if (t == '*' || t == '/')
    {
        y = 2;
    }
    else if (t == '+' || t == '-')
    {
        y = 1;
    }
    else if (t == '(')
    {
        y = 0;
    }

    return (x > y);
}

void infixaParaPos(char expr[]) {
    //Pilha p;
    char caracter_local, controle;
    int k = 0;
    printf("\n%s expr\n", expr);
    char* p = makeStack();
    push(p, '(');
    int tam = 0;
    char posicao_posfixa[100];

    do {
        caracter_local = expr[k];
        k++;

        if (caracter_local == '.') {
            do {
                posicao_posfixa[tam++] = caracter_local;
                caracter_local = expr[k++];
            } while (caracter_local != ',');
            posicao_posfixa[tam++] = caracter_local;
        }

        if (caracter_local >= '0' && caracter_local <= '9') {
            posicao_posfixa[tam++] = caracter_local;
        }
        else if (caracter_local == '(') {
            push(p, '(');
        }
        else if (caracter_local == ')' || caracter_local == '\0') {
            do {
                controle = pop(p);
                if (controle != '(') {
                    posicao_posfixa[tam++] = controle;
                }
            } while (controle != '(');
        }
        else if (caracter_local == '+' || caracter_local == '-' ||
            caracter_local == '*' || caracter_local == '/' ||
            caracter_local == '^') {
            while (1) {
                controle = pop(p);
                if (Verificacao_prioridade(caracter_local, controle)) {
                    push(p, controle);
                    push(p, caracter_local);
                    break;
                }
                else {
                    posicao_posfixa[tam++] = controle;
                }
            }
        }
    } while (caracter_local != '\0');
    posicao_posfixa[tam] = '\0';
    printf("%s posfixa\n", posicao_posfixa);
    montar_assembly_de_posicao(posicao_posfixa);
}


void montar_assembly_de_posicao(char text[]) {


    int char_local = 0;
    int pilha = 0;
    //esta em string.h
    int tamanho_text = strlen(text);
    for (char_local; char_local < tamanho_text; char_local++) {
        char variavel_aux_char_local = text[char_local];
        if (variavel_aux_char_local == '.') {
            char identificador[50];
            int tamanho_local = 0;
            variavel_aux_char_local = text[++char_local];
            do {

                identificador[tamanho_local++] = variavel_aux_char_local;
                variavel_aux_char_local = text[++char_local];
            
            } while (variavel_aux_char_local != ',');
            identificador[tamanho_local] = '\0';
            pilha++;
            printf("%s identificador da expressao\n", identificador);
            int x = 0;
            for (x = 0; x < quantidade_de_valores_na_pilha; x++) {
                if (strcmp(identificador, array_vars[x].representacao) == 0) {
                    //q eh o tamanho em asm
                    fprintf(arq, "\n    subq $8, %%rsp\n");
                    fprintf(arq, "    movq -%d(%%rbp), %%rax\n", (x + 1) * 8);
                    fprintf(arq, "    movq %%rax, -%d(%%rbp)\n", (quantidade_de_valores_na_pilha + pilha) * 8);
                    break;
                }
            }
        }
        if (variavel_aux_char_local >= '0' && variavel_aux_char_local <= '9') {
            pilha++;
            fprintf(arq, "\n    subq $8, %%rsp\n");
            //são oito bytes de um int
            fprintf(arq, "    movq $%c,-%d(%%rbp)\n", variavel_aux_char_local, (quantidade_de_valores_na_pilha + pilha) * 8);
        }

        else if (variavel_aux_char_local == '+' || variavel_aux_char_local == '-') {
            fprintf(arq, "\n    movq -%d(%%rbp), %%rax\n", (quantidade_de_valores_na_pilha + pilha - 1) * 8);
            fprintf(arq, "    movq -%d(%%rbp), %%rbx\n", (quantidade_de_valores_na_pilha + pilha) * 8);

            if (variavel_aux_char_local == '+') {
                fprintf(arq, "\n    addq	%%rax, %%rbx\n");
                fprintf(arq, "    movq	%%rbx, -%d(%%rbp)\n\n", (quantidade_de_valores_na_pilha + pilha - 1) * 8);
            }
            if (variavel_aux_char_local == '-') {
                fprintf(arq, "\n    subq	%%rbx, %%rax\n");
                fprintf(arq, "    movq	%%rax, -%d(%%rbp)\n\n", (quantidade_de_valores_na_pilha + pilha - 1) * 8);
            }
            pilha--;

            //Aqui ocorre a adicao
            fprintf(arq, "    addq $8, %%rsp\n");

        }
    }
}