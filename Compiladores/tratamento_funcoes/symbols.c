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
type_symbol_table_func symbol_table_func;
 

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
 type_symbol_table_entry *sym_declare(char *name, int type, int addr, type_symbol_table_variables *stv) {
	if (stv->n_variables < MAX_SYMBOLS) { //Verifica se eh possivel declarar mais variavel
		int n_var;
        n_var = stv->n_variables;
        stv->variable[n_var].type = type;
        stv->variable[n_var].addr = addr;
        strcpy(stv->variable[n_var].name, name);
        stv->n_variables++;
        n_var = stv->n_variables;
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

    if ( symbol_table_string.n_strings < MAX_SYMBOLS ) {
        int n_str;
        char str_name[MAX_TOKSZ];
        sprintf(str_name, "str%d", str_label_count);
        n_str = symbol_table_string.n_strings;

        strcpy(symbol_table_string.string[n_str].value, s);
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
 * @brief Busca por uma funcao na tabela de simbolos de funcoes (TSF)
 * 
 * @param name nome (id) da funcao
 * @return type_symbol_table_func_entry* entrada da funcao, ou NULL se nao cadastrada
 */
type_symbol_table_func_entry *sym_func_find(char *name) {
    int i;
    for (i = 0; i < symbol_table_func.n_funcs; i++) {
        if (strcmp(symbol_table_func.func[i].name, name) == 0)
            return &(symbol_table_func.func[i]);
    }
    return NULL;
}

/**
 * @brief Insere uma nova funcao na tabela de simbolos de funcoes (TSF). Gera
 * e registra o label da funcao ("_fn_<nome>"), usado como destino do jal. O
 * prefixo '_' evita conflito com os nomes das variaveis, que nao podem
 * comecar com '_'.
 * 
 * @param name nome (id) da funcao
 * @param type tipo declarado da funcao
 * @param params lista de parametros (tipo e nome), na ordem do prototipo
 * @param n_params quantidade de parametros
 * @return type_symbol_table_func_entry* entrada inserida, ou NULL se a TSF estiver cheia
 */
type_symbol_table_func_entry *sym_func_declare(char *name, int type, type_symbol_table_entry params[MAX_PARAMS], int n_params) {
    int i, pos;
    type_symbol_table_func_entry *func;

    if (symbol_table_func.n_funcs >= MAX_FUNCS) {
        printf("[ERRO] Limite de declaracao de funcoes atingido.\n");
        return NULL;
    }
    pos = symbol_table_func.n_funcs;
    func = &(symbol_table_func.func[pos]);
    strcpy(func->name, name);
    snprintf(func->label, sizeof(func->label), "_fn_%s", name);
    func->type = type;
    func->n_params = n_params;
    for (i = 0; i < n_params; i++) {
        strcpy(func->params[i].name, params[i].name);
        func->params[i].type = params[i].type;
        func->params[i].addr = 0;
    }
    func->implemented = false;
    func->called = false;
    symbol_table_func.n_funcs++;
    return func;
}

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
    }
    stv->next = NULL;
}

/**
 * @brief Inicializa tabela de simbolos de strings
 * 
 */
void initSymbolTableString() {
    int i;
    symbol_table_string.n_strings = 0;
    for (i = 0; i < MAX_SYMBOLS; i++) {
        strcpy(symbol_table_string.string[i].name, "");
        strcpy(symbol_table_string.string[i].value, "");
    }
}

/**
 * @brief Inicializa tabela de simbolos de funcoes (TSF)
 * 
 */
void initSymbolTableFunc() {
    int i;
    symbol_table_func.n_funcs = 0;
    for (i = 0; i < MAX_FUNCS; i++) {
        strcpy(symbol_table_func.func[i].name, "");
        strcpy(symbol_table_func.func[i].label, "");
        symbol_table_func.func[i].type = -1;
        symbol_table_func.func[i].n_params = 0;
        symbol_table_func.func[i].implemented = false;
        symbol_table_func.func[i].called = false;
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
    }
}

void printSTString() {
    int i, n_str;
    printf("TABELA DE SIMBOLOS DE STRINGS:\n");
    n_str = symbol_table_string.n_strings;
    printf("Numero de strings = %d\n", n_str);
    for (i = 0; i < n_str; i++) {
        printf("\tstring[%d].name:%s\n", i, symbol_table_string.string[i].name);
        printf("\tstring[%d].value:%s\n", i, symbol_table_string.string[i].value);
    }
}

///Para DEBUG - Nome do tipo, usado na impressao da TSF
static const char *stTypeName(int type) {
    switch (type) {
        case INT:    return "int";
        case FLOAT:  return "float";
        case CHAR:   return "char";
        case STRING: return "string";
        default:     return "?";
    }
}

///Para DEBUG - Impressao da tabela de simbolos de funcoes (TSF)
void printSTFunc() {
    int i, j;
    type_symbol_table_func_entry *func;
    printf("TABELA DE SIMBOLOS DE FUNCOES:\n");
    printf("Numero de funcoes = %d\n", symbol_table_func.n_funcs);
    for (i = 0; i < symbol_table_func.n_funcs; i++) {
        func = &(symbol_table_func.func[i]);
        printf("\tfuncao[%d].name:%s\n", i, func->name);
        printf("\tfuncao[%d].type:%d (%s)\n", i, func->type, stTypeName(func->type));
        printf("\tfuncao[%d].label:%s\n", i, func->label);
        printf("\tfuncao[%d].n_params:%d\n", i, func->n_params);
        for (j = 0; j < func->n_params; j++)
            printf("\t\tparam[%d]: type:%d (%s) name:%s\n", j, func->params[j].type, stTypeName(func->params[j].type), func->params[j].name);
        printf("\tfuncao[%d].implemented:%d\n", i, func->implemented);
        printf("\tfuncao[%d].called:%d\n", i, func->called);
    }
}
