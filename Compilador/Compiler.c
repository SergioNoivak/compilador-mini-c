#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define true  1
#define false 0
typedef enum {INT,MAIN,RETURN,ID,NUM,DESCONHECIDO,ABRE_CHAVES,FECHA_CHAVES,ABRE_PARENTESES,
FECHA_PARENTESES,ESPACO,PONTO_E_VIRGULA,ATRIBUICAO,SOMAR,SUBTRAIR} TIPO;

typedef enum {CHAR,STRING,INTEIRO} TIPO_VARIAVEL;


int c=0,l=1;
typedef struct Token{
    int linha;
    int coluna;
    TIPO tipo;
    union{
        int inteiro;
        char string[100];
        char car;
    }valor;
}Token;


//struct para definir as variaveis declaradas e seus valores
typedef struct {
    TIPO_VARIAVEL tipo;
    char representacao[50];
    union{
        int inteiro;
        char string[100];
        char car;
    }valor;
}Variaveis;
// num_variaveis para controlar o tanto de variveis declaradas
int num_variaveis = 0;
//vetor de variaveis
Variaveis variaveis[50];

int tam = 0;
int tamExpre = 0;
char expressaoText[50];
FILE *f;
int flagFinal = false;
char aux[50];
int rules[] = {0,0,0}; // 0 = declarações iniciadas? 1 = algum return já setado?
Token token;
Token getToken();
void montar_codigo_inicial();
void montar_codigo_retorno();
void montar_codigo_final();
void obter_token();
void programa();
void corpo();
void erro();
void montar_int(char *id);
void declaracoes();
void atribuicao();
void expressao();
Token termo();
void resto();
char *CriaPilha();
void Empilha(char *p,char item);
char Desempilha(char *p);
int Prioridade(char c, char t);
void In2Pos(char expr[]);
void posFixaToAssebly(char text[]);

int main(){
        programa ();
    return 0;
}

void montar_int(char *id){
    //para saber se o cabeçalho de declarações já foi setado        
    fprintf(f, "    subq $8, %%rsp\n");
}

void declaracoes(){
    rules[0] = 1;
    fprintf(f, "    pushq	%%rbp\n");
    fprintf(f, "    movq	%%rsp, %%rbp  \n\n");
}

//função responsavel por atribuir um valor a sua variavel e montar o codigo assembly equivalente
void atribuicao(){
    for(int c=0;c < num_variaveis;c++){
        if(strcmp(aux, variaveis[c].representacao) == 0){
            fprintf(f,"\n    movq -%d(%%rbp), %%rax\n",(num_variaveis+1)*8);  
            fprintf(f,"\n    movq %%rax, -%d(%%rbp)\n",(c+1)*8);  
            fprintf(f, "    addq $8, %%rsp\n");
            break;
        }
    }
}

void expressao(){
    termo();
    resto();
}

Token termo(){
     if(token.tipo == NUM){
        int ax = token.valor.inteiro;
        char c[2];
        c[0] = ax + '0';
        c[1] = '\0';

        strcat(expressaoText,c);
        Token aux = token;
        token = getToken();   
        return aux;     
     }
     if(token.tipo == ID){
        // quando o termo for um id procurar ele na tabela e retornar o seu valor
        int c=0;
        for(c=0;c < num_variaveis;c++){
            if(strcmp(token.valor.string, variaveis[c].representacao) == 0){
                strcat(expressaoText,".");
                strcat(expressaoText,token.valor.string);         
                strcat(expressaoText,",");
                token.valor.inteiro = c+1;
                break;
            }
        }
        Token aux = token;
        token = getToken();    
        return aux;     
     }    
}

void resto(){
    
    if(token.tipo == SOMAR){
        strcat(expressaoText,token.valor.string);
        token = getToken(); 

        if(token.tipo == ABRE_PARENTESES){
            strcat(expressaoText,token.valor.string);
            //considerando que sempre ao abrir uma chave irá ter uma expressão dentro
            token = getToken(); 
            expressao();     
            if(token.tipo == FECHA_PARENTESES){
                strcat(expressaoText,token.valor.string);        
                token = getToken();            
            }else(FECHA_PARENTESES);
        }else {
            //caso o proximo token não for uma nova expressao será apenas um termo
            termo();                
            expressao();
        }
        
    } else if(token.tipo == SUBTRAIR){
        strcat(expressaoText,token.valor.string);
        token = getToken();         
         if(token.tipo == ABRE_PARENTESES){
            strcat(expressaoText,token.valor.string);
            //considerando que sempre ao abrir uma chave irá ter uma expressão dentro
            token = getToken(); 
            expressao();     
            if(token.tipo == FECHA_PARENTESES){
                strcat(expressaoText,token.valor.string);
                token = getToken();             
            }else(FECHA_PARENTESES);
        }else {
            //caso o proximo token não for uma nova expressao será apenas um termo
            termo();
            expressao();
        }   
    }
    else if(token.tipo == ABRE_PARENTESES){
        strcat(expressaoText,token.valor.string);
        token = getToken(); 
        //considerando que sempre ao abrir uma chave irá ter uma expressão dentro
        expressao();     
        if(token.tipo == FECHA_PARENTESES){
            strcat(expressaoText,token.valor.string);
            token = getToken();             
            resto();        
        }else(FECHA_PARENTESES);
    }
}

void programa () {
    token = getToken();
        if(token.tipo == INT){
            token = getToken();
            if(token.tipo == MAIN){
                token = getToken();
                if(token.tipo == ABRE_PARENTESES){
                    token = getToken();                    
                    if(token.tipo == FECHA_PARENTESES){
                        token = getToken();
                        if(token.tipo == ABRE_CHAVES){
                         montar_codigo_inicial(); 
                         token = getToken();
                          corpo();                         
                        if (token.tipo == FECHA_CHAVES) {
                             token = getToken();
                            montar_codigo_final();
                        }
                        else erro(FECHA_CHAVES);
                    } else erro(ABRE_CHAVES);
                } else erro(FECHA_PARENTESES);
            } else erro(ABRE_PARENTESES);
        } else erro(MAIN);
    } else erro(INT);
}

void corpo() {
    expressaoText[0] = '\0';
    tamExpre = 0;
    //identifica uma atribuição a uma variavel
    if (token.tipo == ID) {
        //guardo a varivel para utilizar depois
        strcpy(aux,token.valor.string);
        token = getToken();
        if (token.tipo == ATRIBUICAO) {
            token = getToken();
            //chamo a função espressão para pegar o que vem a frente
            //não importa se seja apenas um numero ou de fato uma espressão
            //chamo a função atribuição para gerar o assembly equivalente
            expressao();
            //função para gerar o codigo equivalente da expressao
            In2Pos(expressaoText);
            atribuicao();    
            if (token.tipo == PONTO_E_VIRGULA) {
                token = getToken();
                //chamar corpo para prosseguir o fluxo de compilação
                corpo();
                } else erro(PONTO_E_VIRGULA);
        } else return;   
    }

    if(token.tipo == INT){
        token = getToken();
         if (token.tipo == ID) {
             //Declaração de inteiro
             //insiro a nova variavel no vetor de variaveis
            variaveis[num_variaveis].tipo = INT;
            strcpy(variaveis[num_variaveis].representacao,token.valor.string);
            num_variaveis++;
            //reservar espaço dessa variavel
            montar_int(token.valor.string);
             token = getToken();
            if (token.tipo == PONTO_E_VIRGULA) {
             token = getToken();
             corpo();
             } else erro(PONTO_E_VIRGULA);
        } else erro(ID);  
    };
    
    //identificação da função return
    if(token.tipo == RETURN) {
        token = getToken();
        expressao();
        In2Pos(expressaoText);
        montar_codigo_retorno();      
            if (token.tipo == PONTO_E_VIRGULA) {
                token = getToken();
                //flag para saber se um return já foi declarado
                rules[1] = 1;
               corpo();
            }
            else erro(PONTO_E_VIRGULA);
    }else{
        //caso nenhum return declarado apresentar erro
        if(rules[1] == 0)
            erro(RETURN);
     };
}

Token getToken(){
    char car;    
    int numero=0;
    int i=0,tam=0;
    char buffer[100];
    Token token;
    car = getchar();
    c++;
    token.coluna = c;
    token.linha = l;

    if(car == '+'){
        token.tipo = SOMAR;
        strcpy(token.valor.string,"+");  
        return token;
    }
    if(car == '-'){
        token.tipo = SUBTRAIR;
        strcpy(token.valor.string,"-");  
        return token;
    }
    if(car == '='){
        token.tipo = ATRIBUICAO;
        strcpy(token.valor.string,"=");  
        return token;
    }

    if(car == '{'){
        token.tipo = ABRE_CHAVES;
        strcpy(token.valor.string,"{");  
        return token;
    }  
    else if(car  == '}'){
        token.tipo = FECHA_CHAVES;
        strcpy(token.valor.string,"}");
        return token;
    }
    else if(car  == ' '){
        token.tipo = ESPACO;
        strcpy(token.valor.string," ");
        return getToken();
    }
    else if(car  == '('){
        token.tipo = ABRE_PARENTESES;
        strcpy(token.valor.string,"(");
        return token;
    }   
    else if(car  == ')'){
        token.tipo = FECHA_PARENTESES;
        strcpy(token.valor.string,")");
        return token;
    }
    else if(car  == '\n'){
        token.tipo = ESPACO;
        l++;
        c=0;
        return getToken();
    }   
    else if(car  == EOF){
        token.tipo = ESPACO;
        strcpy(token.valor.string,"\\n");
        return token;
    }   
    else if(car  == ';'){
        token.tipo = PONTO_E_VIRGULA;
        strcpy(token.valor.string,";");
        return token;
    }

    if(isalpha(car) || car == '_' ){
        token.coluna = c;
        token.linha = l;
        while(isalnum(car) || car == '_' ){
            buffer[tam] = car;
            tam++;
            c++;
            car = getchar();       
        }
        ungetc(car, stdin);
        buffer[tam] = '\0';

        if(strcmp(buffer,"main") == 0){
            token.tipo = MAIN;
            strcpy(token.valor.string,"main");

            return token;
        }
        else if(strcmp(buffer,"int") == 0 ){
            token.tipo = INT;
            strcpy(token.valor.string,"int");        
            return token;
        }
               
        else if(strcmp(buffer,"return") == 0){ 
            token.tipo = RETURN;
            strcpy(token.valor.string,"return");
            return token;  
        }

        else{
            token.tipo = ID;
            strcpy(token.valor.string,buffer);
            return token;
        }
    }
    if(isdigit(car)){
        numero = numero * 10;
        numero += (car - '0');
        token.coluna = c;
        token.linha = l;
        car = getchar();  
        while(isdigit(car)){
            numero = numero * 10;
            numero += (car - '0');
            c++;
            car = getchar();       
        }
        ungetc(car, stdin);
        token.tipo = NUM;
        token.valor.inteiro = numero;
        return token;
    }       
}


void montar_codigo_inicial() {
    f = fopen("out.s", "w+");
   // fprintf(f, ".text\n");
    fprintf(f, "    .global _start\n\n");
    fprintf(f, "_start:\n\n");
    declaracoes();
}

void montar_codigo_retorno() {
    fprintf(f, "\n    movq  -%d(%%rbp), %%rbx\n", (num_variaveis+1)*8);
    fprintf(f,"\n    addq   $%d, %%rsp\n",(num_variaveis+1)*8);              
    fprintf(f,"    movq   %%rbp, %%rsp\n");              
    fprintf(f,"    popq   %%rbp\n");
    fprintf(f, "\n    movq   $1, %%rax\n");
    fprintf(f, "    int $0x80\n\n");
}

void montar_codigo_final() {
    fclose(f);
    printf("Arquivo \"out.s\" gerado.\n\n");
}

void erro(TIPO type) {
    if (type == INT)
        printf("Erro Sintatico: (%d, %d). Esperado < 'INT' >\n", token.linha, token.coluna);
    else if (type == MAIN)
        printf("Erro Sintatico: (%d, %d). Esperado < 'MAIN' >\n", token.linha, token.coluna);
    else if (type == ABRE_PARENTESES)
        printf("Erro Sintatico: (%d, %d). Esperado < '(' >\n", token.linha, token.coluna);
    else if (type == FECHA_PARENTESES)
        printf("Erro Sintatico: (%d, %d). Esperado < ')' >\n", token.linha, token.coluna);
    else if (type == ABRE_CHAVES)
        printf("Erro Sintatico: (%d, %d). Esperado < '{' >\n", token.linha, token.coluna);
    else if (type == RETURN)
        printf("Erro Sintatico: (%d, %d). Esperado < 'RETURN' >\n", token.linha, token.coluna);
    else if (type == NUM)
        printf("Erro Sintatico: (%d, %d). Esperado < 'NUM' >\n", token.linha, token.coluna);
    else if (type == PONTO_E_VIRGULA)
        printf("Erro Sintatico: (%d, %d). Esperado < ';' >\n", token.linha, token.coluna);
    else if (type == FECHA_CHAVES)
        printf("Erro Sintatico: (%d, %d). Esperado < '}' >\n", token.linha, token.coluna);
    exit(1);
}




char *CriaPilha(){
    char *pilha = (char*)malloc(100 * sizeof(char));
    return pilha;
}

void Empilha(char *p,char item){
    p[tam] = item;
    tam++;
}

char Desempilha(char *p){
    tam--;
    char aux = p[tam];
    return aux;
}

int Prioridade(char c, char t){
  int pc,pt;
 
  if(c == '^')
    pc = 4;
  else if(c == '*' || c == '/')
    pc = 2;
  else if(c == '+' || c == '-')
    pc = 1;
  else if(c == '(')
    pc = 4;
 
  if(t == '^')
    pt = 3;
  else if(t == '*' || t == '/')
    pt = 2;
  else if(t == '+' || t == '-')
    pt = 1;
  else if(t == '(')
    pt = 0;
 
  return (pc > pt);
}

void In2Pos(char expr[]){
  //Pilha p;
  int i = 0;
  char c,t;
  printf("\n%s expr\n",expr);
  char *p = CriaPilha();
  Empilha(p, '(');
  char posFixa[100];
  int tam = 0;
 
  do{
    c = expr[i];
    i++;
    
    if(c == '.'){
        do{
            posFixa[tam++] = c;
            c = expr[i++];
        }while(c != ',');   
        posFixa[tam++] = c;
    }
    
    if(c >= '0' && c <= '9'){
    //   printf("%c", c);
      posFixa[tam++] = c;
    }
    else if(c == '('){
      Empilha(p, '(');
    }
    else if(c == ')' || c == '\0'){
      do{
        t = Desempilha(p);
        if(t != '('){
        //   printf("%c", t);
            posFixa[tam++] = t;          
          }
      }while(t != '(');
    }
    else if(c == '+' || c == '-' || 
            c == '*' || c == '/' ||
            c == '^' ){
      while(1){
        t = Desempilha(p);
        if(Prioridade(c,t)){
          Empilha(p, t);
          Empilha(p, c);
          break;
        }
        else{
          posFixa[tam++] = t;
        //   printf("%c", t);
        }
      }
    }
  }while(c != '\0');
  posFixa[tam] = '\0';
  printf("%s posfixa\n",posFixa);
  //printf("%s\n",posFixa);
  posFixaToAssebly(posFixa);
 // LiberaPilha(p);
}


void posFixaToAssebly(char text[]){
    int len = strlen(text);
    int pilha = 0;
    int c = 0;
    // if(len < 2) return ;
    for(c;c<len;c++){
        char aux = text[c];
        if(aux == '.'){
            char id[50];
            int tam = 0;
            aux = text[++c];
            do{
                id[tam++] = aux;     
                aux = text[++c];
            }while(aux != ',');
            id[tam] = '\0';
            printf("%s id na expressao\n",id);
            pilha++;
            int x=0;
            for(x=0;x < num_variaveis;x++){
                if(strcmp(id, variaveis[x].representacao) == 0){
                    fprintf(f,"\n    subq $8, %%rsp\n");
                    fprintf(f,"    movq -%d(%%rbp), %%rax\n",(x+1)*8);              
                    fprintf(f,"    movq %%rax, -%d(%%rbp)\n",(num_variaveis+pilha)*8);              
                    break;
                }
            }
        }
        if(aux >= '0' && aux <= '9'){
            pilha++;
            fprintf(f,"\n    subq $8, %%rsp\n");
            fprintf(f,"    movq $%c,-%d(%%rbp)\n",aux,(num_variaveis+pilha)*8);
        }else if(aux == '+' || aux == '-'){ 
            fprintf(f,"\n    movq -%d(%%rbp), %%rax\n",(num_variaveis+pilha-1)*8);
            fprintf(f,"    movq -%d(%%rbp), %%rbx\n",(num_variaveis+pilha)*8);
            
            if(aux == '+'){
                fprintf(f,"\n    addq	%%rax, %%rbx\n");
                fprintf(f,"    movq	%%rbx, -%d(%%rbp)\n\n",(num_variaveis+pilha-1)*8);
            }
            if(aux == '-'){
                fprintf(f,"\n    subq	%%rbx, %%rax\n");
                fprintf(f,"    movq	%%rax, -%d(%%rbp)\n\n",(num_variaveis+pilha-1)*8);
            }
            pilha--;
            fprintf(f,"    addq $8, %%rsp\n");

        }
    }   
}