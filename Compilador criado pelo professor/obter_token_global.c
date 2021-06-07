#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define TAM_TAB 8

int conv_car(char car){
	return car - '0';
}

int proxima_coluna(int coluna){
	int qtd_colunas;
	int resto_colunas;
	coluna -= 1;	//descontando a leitura do caractere TAB
	
	qtd_colunas = coluna/TAM_TAB;
	resto_colunas = coluna%TAM_TAB;
	
	if (resto_colunas != 0){
		qtd_colunas = (qtd_colunas+1)*TAM_TAB;
	}
	else{
		qtd_colunas = qtd_colunas*TAM_TAB;
	}
	return qtd_colunas + 1;

//	return (((coluna-1)%TAM_TAB == 0)? ((coluna-1)/TAM_TAB) * TAM_TAB: ((coluna-1)/TAM_TAB + 1) * TAM_TAB)+1;
}

typedef enum {INT, MAIN, RETURN, ID, NUM, DESCONHECIDO, ABRE_CHAVES, FECHA_CHAVES, ABRE_PARENTESES, FECHA_PARENTESES, ESPACO, PONTO_E_VIRGULA} TIPO;

typedef struct Token{
	int linha;
	int coluna;
	TIPO tipo;
	union{
		int inteiro;
		char *string;
		char car;
	}valor;
}Token;

Token token;				// token declarado global
int linha=1, coluna=1;	// linha e coluna declarados global

void obter_token(){
	char car;
	int numero;
	char buffer[100];

	car=getchar();
	coluna++;

	//Preparando a parte padrão para todos os tokens
	token.linha = linha;
	token.coluna = coluna-1;

	if (car == EOF){
		token.tipo = EOF;
		return;
	}

	if (car == ' '){
		token.tipo = ESPACO;
		return;
	}

	if (car == '\n'){
		linha++;
		coluna = 1;
		token.tipo = ESPACO;
		return;
	}
		
	if (car == '\t'){
		//coluna += TAM_TAB;
		coluna = proxima_coluna(coluna);
		token.tipo = ESPACO;
		return;
	}

	// Reconhecendo um abre parenteses
	if (car == '('){
		token.valor.car = car;
		token.tipo = ABRE_PARENTESES;
		return;
	}

	// Reconhecendo um fecha parenteses
	if (car == ')'){
		token.valor.car = car;
		token.tipo = FECHA_PARENTESES;
		return;
	}

	// Reconhecendo um abre chaves
	if (car == '{'){
		token.valor.car = car;
		token.tipo = ABRE_CHAVES;
		return;
	}

	// Reconhecendo um fecha chaves
	if (car == '}'){
		token.valor.car = car;
		token.tipo = FECHA_CHAVES;
		return;
	}

	// Reconhecendo um ponto e vírgula
	if (car == ';'){
		token.valor.car = car;
		token.tipo = PONTO_E_VIRGULA;
		return;
	}

	// Reconhecendo um número inteiro sem sinal
	if (isdigit(car)){
		//printf("DIGITO: %c\n", car);
		numero = conv_car(car);
	
		//Obtendo os próximos digitos
		car=getchar();
		coluna++;
		while(isdigit(car)){
			numero = numero*10 + conv_car(car);

			car=getchar();
			coluna++;
		}

		//Não há mais digitos. Devolvendo o último caractere para o buffer.
		ungetc(car, stdin);
		coluna--;

		//printf ("<número, %i> encontrado(%i, %i)\n", numero, linha, coluna);
		token.tipo = NUM;
		token.valor.inteiro=numero;
		return;
	}

	//Reconhecendo um identificador
	if (isalpha(car) || car == '_'){		
		int pos=0;
		buffer[pos++] = car;
			
		//Obtendo os próximos caracteres do indentificador
		car=getchar();
		coluna++;

		while((isalnum(car) || car == '_')){
			buffer[pos++] = car;
			car=getchar();
			coluna++;
		}
		buffer[pos] = '\0';
		//Identificador finalizado. Devolvendo o último caractere para o buffer.
		ungetc(car, stdin);
		coluna--;
		
		//Reconhecendo a palavra reservada main
		if (!strcmp(buffer, "main")){
			//printf("<main, %s> encontrado(%i, %i)\n", buffer, linha, coluna);
			token.tipo = MAIN;
			token.valor.string=strdup(buffer);
			return;
		}

		//Reconhecendo a palavra reservada int
		if (!strcmp(buffer, "int")){
			//printf("<int, %s> encontrado(%i, %i)\n", buffer, linha, coluna);
			token.tipo = INT;
			token.valor.string=strdup(buffer);
			return;
		}

		//Reconhecendo a palavra reservada return
		if (!strcmp(buffer, "return")){
			//printf("<return, %s> encontrado(%i, %i)\n", buffer, linha, coluna);
			token.tipo = RETURN;
			token.valor.string=strdup(buffer);
			return;
		}

		//printf("<id, %s> encontrado(%i, %i)\n", buffer, linha, coluna);
		token.tipo = ID;
		token.valor.string=strdup(buffer);
		return;
	}
	
	// Caracter não identificado
	//printf("(%i, %i): %c\n", linha, coluna, car);
	token.tipo = DESCONHECIDO;
	token.valor.car=car;
	return;
}

void obter_token_geral(){
	obter_token();
	while(token.tipo == ESPACO){
		obter_token();
	}
}

//############ início da análise sintática

void erro(){
	if (token.tipo == INT){
		printf("erro sintático (%i, %i). Esperava: \"%s\"", token.linha, token.coluna, token.valor.string);
		return;
	}

	printf("erro sintático (%i, %i).\n\n", token.linha, token.coluna);
}

void programa(){
	if (token.tipo == INT){
		printf("int reconhecido.\n");

	}else{
		erro();
	}
}

void parser(){
	obter_token_geral();
	//printf("->%i %s\n\n", token.tipo, token.valor.string);
	programa();
}

int main(){
	parser();	
}
