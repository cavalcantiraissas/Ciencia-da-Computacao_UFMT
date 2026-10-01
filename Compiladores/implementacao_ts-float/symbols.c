/**
 * @file symbols.c
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Modulo do gerador da tabela de simbolos
 * @version 0.1
 * @date 2022-02-04
 */
#include "symbols.h"

//extern int nstringconsts;
//int sympos = 0;
//int nstringconsts = 0;
//int symfuncspos = 0; //Sera usado posteriormente
//int stack_pos = 0;

//Variaveis globais
type_symbol_table_variables global_symbol_table_variables;
type_symbol_table_string symbol_table_string;
//type_symbol_function symfuncs[MAX_FUNCS];
 

/**
 * @brief Busca pelo simbolo na tabela de variaveis
 * 
 * @param char*
 * @param type_symbol_table_variables*
 * @return type_symbol_table_entry* 
 */
type_symbol_table_entry *sym_find(char *s, type_symbol_table_variables *stv) {
	int i;
	type_symbol_table_entry *symbol = NULL;
	for (i = 0; i < stv->n_variables; i++) {
		if (strcmp(stv->variable[i].name, s) == 0) {
			symbol = &(stv->variable[i]);
		}
	}
	return symbol;
}

/**
 * @brief Insere um novo simbolo na tabela de variaveis
 * 
 * @param name 
 * @param type 
 * @param addr 
 * @return type_symbol* 
 */
 type_symbol_table_entry *sym_declare(char *name, int type, int addr, void *value, type_symbol_table_variables *stv) {
	if (stv->n_variables < MAX_SYMBOLS) { //Verifica se eh possivel declarar mais variavel
		int n_var;
        n_var = stv->n_variables;
        stv->variable[n_var].type = type;
        stv->variable[n_var].addr = addr;
        stv->variable[n_var].value = value; //valor generico (NULL se nao informado)
        strcpy(stv->variable[n_var].name, name);
        stv->n_variables++;
        return &(stv->variable[n_var]);
	} else {
        printf("[ERRO] Limite de declaracao (quantidade) de variaveis atingido.\n");
        return NULL;
    }
}

/**
 * @brief Busca pelo simbolo na tabela de STRINGS
 * 
 * @param char*
 * @return type_symbol_table_string_entry* 
 */
type_symbol_table_string_entry *sym_string_find(char *s) {
    int i;
    type_symbol_table_string_entry *str = NULL;
    for (i = 0; i < symbol_table_string.n_strings; i++ ) {
        if ( strcmp(symbol_table_string.string[i].value, s) == 0 ) {
            str = &(symbol_table_string.string[i]);
        }
    }
    return str;
}

/**
 * @brief Insere um novo simbolo na tabela de STRINGS
 * 
 * @param char*
 * @return type_symbol_table_string_entry* 
 */
type_symbol_table_string_entry *sym_string_declare(char *s) {
    static int str_label_count = 0;

    if ( symbol_table_string.n_strings < MAX_STRINGS ) {
        int n_str;
        char str_name[MAX_TOKSZ];
        snprintf(str_name, sizeof(str_name), "str%d", str_label_count);
        n_str = symbol_table_string.n_strings;

        //limita a copia ao tamanho do campo (evita overflow com strings longas)
        strncpy(symbol_table_string.string[n_str].value, s, MAX_STRINGSZ - 1);
        symbol_table_string.string[n_str].value[MAX_STRINGSZ - 1] = '\0';
        strcpy(symbol_table_string.string[n_str].name, str_name);
        str_label_count++;
        symbol_table_string.n_strings++;
        return &(symbol_table_string.string[n_str]) ;
    } else {
        printf("[ERRO] Numero maximo de strings declaradas alcancado!\n");
        return NULL;
    }
}

/**
 * @brief Busca pelo simbolo na tabela (funcoes)
 * 
 * @param s 
 * @return type_symbol_function* 
 */
/*
type_symbol_function *sym_func_find(char *s){
    int i;
	type_symbol_function *symbol = NULL;
	for (i = 0; i < symfuncspos; i++) {
		if (strcmp(symfuncs[i].name, s) == 0) {
			symbol = &symfuncs[i];
		}
	}
	return symbol;
}
*/

/**
 * @brief Insere um novo simbolo na tabela (funcoes)
 * 
 * @param name 
 * @param type 
 * @param params 
 * @param nparams 
 * @return type_symbol_function* 
 */
/*
type_symbol_function *sym_func_declare(char *name, char type, type_symbol params[MAX_PARAMS],int nparams){
    int i;
    strncpy (symfuncs[symfuncspos].name, name, MAX_TOKSZ);
    symfuncs[symfuncspos].type = type;
    symfuncs[symfuncspos].nparams = nparams;
    for (i = 0; i < nparams; i++) {    
        strncpy(symfuncs[symfuncspos].params[i].name,params[i].name,MAX_TOKSZ);
        symfuncs[symfuncspos].params[i].type = params[i].type;
    }
    symfuncspos ++;
    if (symfuncspos > MAX_FUNCS) {
		printf("[ERRO] Limite de declaracao de funcoes atingido.");
    }
    return &symfuncs[symfuncspos -1];
}
*/

/**
 * @brief Inicializa tabela de simbolos de variaveis globais
 * 
 * @param stv 
 */
void initSymbolTableVariables(type_symbol_table_variables *stv) {
    int i;
    stv->n_variables = 0;
    for (i = 0; i < MAX_SYMBOLS; i++) {
        strcpy(stv->variable[i].name, "");
        stv->variable[i].type = -1;
        stv->variable[i].addr = 0;
        stv->variable[i].value = NULL;
    }
    stv->next = NULL;
}

/**
 * @brief Inicializa tabela de simbolos de strings
 * 
 */
void initSymbolTableString(void) {
    int i;
    symbol_table_string.n_strings = 0;
    for (i = 0; i < MAX_STRINGS; i++) { //apenas MAX_STRINGS posicoes sao usadas
        strcpy(symbol_table_string.string[i].name, "");
        strcpy(symbol_table_string.string[i].value, "");
    }
}

///Para DEBUG
void printSTVariables(type_symbol_table_variables *stv) {
    int i, n_var;
    printf("TABELA DE SIMBOLOS DE VARIAVEIS:\n");
    n_var = stv->n_variables;
    printf("Numero de variaveis = %d\n", n_var);
    for (i = 0; i < n_var; i++) {
        printf("\tvariavel[%d].name:%s\n", i, stv->variable[i].name);
        printf("\tvariavel[%d].type:%d\n", i, stv->variable[i].type);

        //O campo 'value' eh generico (void*); convertemos de acordo com 'type'
        if ( stv->variable[i].value == NULL ) {
            printf("\tvariavel[%d].value: (nao inicializada)\n", i);
        } else {
            switch ( stv->variable[i].type ) {
                case INT:
                    printf("\tvariavel[%d].value:%d\n", i, *((int *) stv->variable[i].value));
                    break;
                case FLOAT:
                    printf("\tvariavel[%d].value:%f\n", i, *((float *) stv->variable[i].value));
                    break;
                default:
                    printf("\tvariavel[%d].value: (tipo sem impressao definida)\n", i);
                    break;
            }
        }
    }
}

void printSTString(void) {
    int i, n_str;
    printf("TABELA DE SIMBOLOS DE STRINGS:\n");
    n_str = symbol_table_string.n_strings;
    printf("Numero de strings = %d\n", n_str);
    for (i = 0; i < n_str; i++) {
        printf("\tstring[%d].name:%s\n", i, symbol_table_string.string[i].name);
        printf("\tstring[%d].value:%s\n", i, symbol_table_string.string[i].value);
    }
}