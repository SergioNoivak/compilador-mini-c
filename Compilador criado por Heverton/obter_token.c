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

Token obter_token(int *linha, int *coluna){
	char car;
	int numero;
	char buffer[100];

	car=getchar();
	(*coluna)++;

	//Preparando a parte padrão para todos os tokens
	Token token;
	token.linha = *linha;
	token.coluna = *coluna-1;

	if (car == EOF){
		token.tipo = EOF;
		return token;
	}

	if (car == ' '){
		token.tipo = ESPACO;
		return token;
	}

	if (car == '\n'){
		(*linha)++;
		*coluna = 1;
		token.tipo = ESPACO;
		return token;
	}
		
	if (car == '\t'){
		//coluna += TAM_TAB;
		*coluna = proxima_coluna(*coluna);
		token.tipo = ESPACO;
		return token;
	}

	// Reconhecendo um abre parenteses
	if (car == '('){
		token.valor.car = car;
		token.tipo = ABRE_PARENTESES;
		return token;
	}

	// Reconhecendo um fecha parenteses
	if (car == ')'){
		token.valor.car = car;
		token.tipo = FECHA_PARENTESES;
		return token;
	}

	// Reconhecendo um abre chaves
	if (car == '{'){
		token.valor.car = car;
		token.tipo = ABRE_CHAVES;
		return token;
	}

	// Reconhecendo um fecha chaves
	if (car == '}'){
		token.valor.car = car;
		token.tipo = FECHA_CHAVES;
		return token;
	}

	// Reconhecendo um ponto e vírgula
	if (car == ';'){
		token.valor.car = car;
		token.tipo = PONTO_E_VIRGULA;
		return token;
	}

	// Reconhecendo um número inteiro sem sinal
	if (isdigit(car)){
		//printf("DIGITO: %c\n", car);
		numero = conv_car(car);
	
		//Obtendo os próximos digitos
		car=getchar();
		(*coluna)++;
		while(isdigit(car)){
			numero = numero*10 + conv_car(car);

			car=getchar();
			(*coluna)++;
		}

		//Não há mais digitos. Devolvendo o último caractere para o buffer.
		ungetc(car, stdin);
		(*coluna)--;

		//printf ("<número, %i> encontrado(%i, %i)\n", numero, linha, coluna);
		token.tipo = NUM;
		token.valor.inteiro=numero;
		return token;
	}

	//Reconhecendo um identificador
	if (isalpha(car) || car == '_'){		
		int pos=0;
		buffer[pos++] = car;
			
		//Obtendo os próximos caracteres do indentificador
		car=getchar();
		(*coluna)++;

		while((isalnum(car) || car == '_')){
			buffer[pos++] = car;
			car=getchar();
			(*coluna)++;
		}
		buffer[pos] = '\0';
		//Identificador finalizado. Devolvendo o último caractere para o buffer.
		ungetc(car, stdin);
		(*coluna)--;
		
		//Reconhecendo a palavra reservada main
		if (!strcmp(buffer, "main")){
			//printf("<main, %s> encontrado(%i, %i)\n", buffer, linha, coluna);
			token.tipo = MAIN;
			token.valor.string=strdup(buffer);
			return token;
		}

		//Reconhecendo a palavra reservada int
		if (!strcmp(buffer, "int")){
			//printf("<int, %s> encontrado(%i, %i)\n", buffer, linha, coluna);
			token.tipo = INT;
			token.valor.string=strdup(buffer);
			return token;
		}

		//Reconhecendo a palavra reservada return
		if (!strcmp(buffer, "return")){
			//printf("<return, %s> encontrado(%i, %i)\n", buffer, linha, coluna);
			token.tipo = RETURN;
			token.valor.string=strdup(buffer);
			return token;
		}

		//printf("<id, %s> encontrado(%i, %i)\n", buffer, linha, coluna);
		token.tipo = ID;
		token.valor.string=strdup(buffer);
		return token;
	}
	
	// Caracter não identificado
	//printf("(%i, %i): %c\n", linha, coluna, car);
	token.tipo = DESCONHECIDO;
	token.valor.car=car;
	return token;
}

int main(){
	int linha=1, coluna=1;
	Token token;
	for (token = obter_token(&linha, &coluna); token.tipo != EOF; token = obter_token(&linha, &coluna)){

		if (token.tipo == ESPACO)
			continue;

		printf("(%-2i, %-2i): ", token.linha, token.coluna);
		switch (token.tipo){
			//case ESPACO:
			//	continue;
			case NUM:
				printf("<NUM, %i> ", token.valor.inteiro);
				break;
			case MAIN:
				printf("<MAIN, %s> ", token.valor.string);
				break;
			case INT:
				printf("<INT, %s> ", token.valor.string);
				break;
			case RETURN:
				printf("<RETURN, %s> ", token.valor.string);
				break;
			case ID:
				printf("<ID, %s> ", token.valor.string);
				break;
			case ABRE_PARENTESES:
				printf("<ABRE_PARENTESES, %c> ", token.valor.car);
				break;
			case FECHA_PARENTESES:
				printf("<FECHA_PARENTESES, %c> ", token.valor.car);
				break;
			case ABRE_CHAVES:
				printf("<ABRE_CHAVES, %c> ", token.valor.car);
				break;
			case FECHA_CHAVES:
				printf("<FECHA_CHAVES, %c> ", token.valor.car);
				break;
			case PONTO_E_VIRGULA:
				printf("<PONTO_E_VIRGULA, %c> ", token.valor.car);
				break;
			default :
				printf("<DESCONHECIDO, %c> ", token.valor.car);
		}
		printf("\n");

	}
}
