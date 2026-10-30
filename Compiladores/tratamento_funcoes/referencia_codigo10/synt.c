/**
 * @file synt.c
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Codificacao do modulo do analisador sintatico
 * @version 0.4
 * @date 2022-02-04
 * 
 */

// Inclusao do cabecalho
#include "synt.h"

// Variaveis globais
type_token *lookahead;
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
extern char output_file_name[MAX_CHAR];
extern FILE *output_file;

/**
 * @brief Verifica se o proximo caracter (a frente) na cadeia eh o esperado
 * 
 * @param token_tag (int) codigo do token a ser verificado 
 * @return int true/false
 */
int match(int token_tag) {
    if ( lookahead->tag == token_tag ) {
        lookahead = getToken(); //Pega o proximo token por meio do lexico
        return true;
    }
    printf("[ERRO] Entrada esperada: %s\n", lookahead->lexema);
    return false;
}

/**
 * @brief Regra de derivacao inicial
 */
void program (void) {
    gen_preambule(); //Temporariamente cria um preambulo adicional que permite o uso das funcoes scanf e printf
    declarations();
    gen_preambule_code(); //Chamada do gerador de codigo para escrita do cabecalho da secao de codigo
    //#begin
    statements();
    //#end
    //#codigo das funcoes
    gen_epilog_code();
    gen_data_section(); //Chamada do gerador de codigo para declaracao de variaveis
}

/**
 * @brief Regra de derivacao para declaracoes
 */
void declarations(void) {
    while ( declaration() ); //Laco para processamento continuo das declaracoes
}

/**
 * @brief Regra de derivacao declaracao
 * @return int true/false
 */
int declaration (void) {
    type_symbol_table_entry *search_symbol;
    type_symbol_table_func_entry *search_func_symbol;
    int ok1, ok2;
    char var_name[MAX_CHAR];
    int var_type;

    //Verifica o tipo da variavel
    var_type = lookahead->tag;
    if ( var_type == INT || var_type == FLOAT || var_type == CHAR || var_type == STRING) { 
        match(var_type);
        strcpy(var_name, lookahead->lexema);
        ok1 = match(ID); //Verifica ID
        
        //Declaracao de VARIAVEL
        if (lookahead->tag == SEMICOLON) { //Verifica se eh declaracao de variavel
            search_symbol = sym_find( var_name, &global_symbol_table_variables ); //Busca na TS de variaveis
            if ( search_symbol != NULL) {
                printf ("[ERRO] Variavel '%s' ja declarada.\n", var_name); 
                return false;
            } else {
                sym_declare( var_name, var_type, 0, &global_symbol_table_variables);
                ok2 = match(SEMICOLON); //Verifica se ; vem a seguir
                return ok1 && ok2;
            }
        } // Declaracao de FUNCAO 
        else if (lookahead->tag == OPEN_PAR) { //Verifica se eh declaracao de funcao
            search_func_symbol = sym_func_find(var_name);
            if (search_func_symbol != NULL) {
                printf("[ERRO] Funcao '%s' ja declarada.\n", var_name);
                return false;
            } else {
                match(OPEN_PAR);
                //Constroi lista de parametros
                type_symbol_table_entry parameters[MAX_PARAMS];
                int n_params;
                n_params = 0;
                char p_name[MAX_CHAR];
                int p_type;
                
                while (lookahead->tag != CLOSE_PAR) {
                    p_type = lookahead->tag;
                    if ( p_type == INT || p_type == FLOAT || p_type == CHAR || p_type == STRING) {
                        match(p_type);
                        strcpy(p_name, lookahead->lexema);
                        match(ID);
                        parameters[n_params].type = p_type;
                        strcpy(parameters[n_params].name, p_name);
                        n_params++;
                        if (lookahead->tag == COMMA) {
                            match(COMMA);
                            if (lookahead->tag != INT || lookahead->tag != FLOAT || lookahead->tag != CHAR || lookahead->tag != STRING) {
                                printf("[ERRO] Erro na declaracao de parametro da funcao. Obrigatorio um tipo de dado apos a virgula.\n");
                                return false;
                            }
                        }
                    } else {
                        printf("[ERRO] Declaracao de parametro incorreta. Esperado um tipo de dado.\n");
                        return false;
                    }
                }

                sym_func_declare(var_name, var_type, parameters, n_params);
                ok2 = match(CLOSE_PAR) && match(SEMICOLON);
                return ok1 && ok2;
            }
        } else { //Caso contrario eh erro
            printf("[ERRO] Era esperado um ';' ou '(' para reconhecer declaracao de variavel, ou de funcao.\n");
            return false;
        }
    } else if (lookahead->tag == ENDTOKEN ||
                lookahead->tag == BEGIN ) {
        //Verifica se fim de arquivo
        return false;         
    } else {
        printf ("[ERRO] Tipo desconhecido: %d %s.\n", lookahead->tag, lookahead->lexema);
        return false; 
    }
}

/**
 * @brief Regra de derivacao para comandos
 */
void statements (void) {
   while ( statement() );  //processa enquanto houver comandos
}

/**
 * @brief Regra de derivacao que processa os comandos
 * 
 * @return int true/false
 */
int statement (void) {
    char lexeme_of_id[MAX_CHAR];
    type_symbol_table_entry *search_symbol;
    type_symbol_table_string_entry *gen_string;
    int ok1, ok2, type;
    char string_value[MAX_CHAR];

    if (lookahead->tag == READ) {
        match(READ);
        strcpy(lexeme_of_id, lookahead->lexema);
        ok1 = match(ID);
        search_symbol = sym_find(lexeme_of_id, &global_symbol_table_variables);
        if (search_symbol != NULL) {
            type = search_symbol->type;
            gen_read(lexeme_of_id, type);
            ok2 = match(SEMICOLON);
            return ok1 && ok2;
        } else {
            printf("[ERRO] Simbolo desconhecido (Variavel nao declarada): %s\n", lexeme_of_id);
            return false;
        }
    } else if (lookahead->tag == WRITE) {
        match(WRITE);
        if ( lookahead->tag == STRING ) {
            strcpy(string_value, lookahead->lexema);
            //TODO: verificar se o valor da constante ja foi incluida na tabela de simbolos
            gen_string = sym_string_declare(string_value);
            match(STRING);
            if ( gen_string != NULL ) {
                strcpy(lexeme_of_id, gen_string->name);
                gen_write(lexeme_of_id, STRING);
                match(SEMICOLON);
            }
        } else if ( lookahead->tag == ID) { //Verifica se uso de funcao ou variavel
            strcpy(lexeme_of_id, lookahead->lexema);
            match(ID);
            search_symbol = sym_find(lexeme_of_id, &global_symbol_table_variables);
            if (search_symbol != NULL) {
                type = search_symbol->type;
                gen_write(lexeme_of_id, type);
                match(SEMICOLON);
                return true;
            } else {
                printf("[ERRO] Simbolo desconhecido (Variavel nao declarada): %s\n", lexeme_of_id);
                return false;
            }
        }
    } else if (lookahead->tag == IF) {
        char label_else[MAX_CHAR];
        char label_end[MAX_CHAR];
        gen_label_name(label_else);
        gen_label_name(label_end);

        match(IF);
        match(OPEN_PAR);
        B(); //Expressao booleana
        gen_cond_jump(label_else);
        match(CLOSE_PAR);
        
        match(BEGIN);
        statements();
        match(END);

        gen_incond_jump(label_end);
        gen_label(label_else);

        //Verifica se ocorre um ELSE
        if (lookahead->tag == ELSE) {
            match(ELSE);
            match(BEGIN);
            statements();
            match(END);
        }
        gen_label(label_end);
        return true;
    } else if (lookahead->tag == WHILE) {
        char label_begin[MAX_CHAR];
        char label_end[MAX_CHAR];
        gen_label_name(label_begin);
        gen_label_name(label_end);

        match(WHILE);
        match(OPEN_PAR);
        gen_label(label_begin);
        B(); //Expressao booleana
        gen_cond_jump(label_end);
        match(CLOSE_PAR);
        match(BEGIN);
        statements();
        match(END);
        gen_incond_jump(label_begin);
        gen_label(label_end);
        return true;
    } else if ( lookahead->tag == ID) {
        char id_name[MAX_TOKSZ];
        strcpy(id_name, lookahead->lexema);
        match(ID);
        // Verifica se eh uma atribuicao de variavel
        if ( lookahead->tag == ASSIGN) {
            type_symbol_table_entry *variable;
            variable = sym_find(id_name);
            if (variable != NULL) {
                match(ASSIGN);
                if (variable->type == INT || variable->type == FLOAT) {
                    if (lookahead->tag == NUM)
                        return true;
                    else {
                        printf("[ERRO SEMANTICO] atribuicao de valor incompativel, esperado um valor numerico.\n");
                        return false;
                    }
                }
                //TODO: processar STRING e CHAR
            } else {
                printf("[ERRO] Tentativa de atribuir valor a uma variavel nao declarada.\n");
                return false;
            }
        } // Verifica se eh uma chamada de funcao
        else if ( lookahead->tag == OPEN_PAR) {
            type_symbol_table_func_entry *function;
            function = sym_func_find(id_name));
            if (function != NULL) {
                match(OPEN_PAR);
                //Monta lista de parametros
                type_token params[MAX_PARAMS];
                int cont_params;
                cont_params = 0;
        
                while (lookahead->tag != CLOSE_PAR) {
                    if (lookahead->tag == NUM) {
                        params[cont_params].tag = lookahead->tag;
                        params[cont_params].value = atoi(lookahead->lexema);
                        cont_params++; 
                    }    
                }
                //TODO: Verifica se lista de parametros eh compativel com declaracao

            } else {
                printf("[ERRO] Tentativa de chamada de funcao nao declarada,\n");
                return false;
            }
        }
            
    
    } else if ((lookahead->tag == ENDTOKEN) || (lookahead->tag == END)) {
        return false;
    } else {
        printf("[ERRO] Comando desconhecido.\nTag=%d; Lexema=%s\n",lookahead->tag, lookahead->lexema);
        return false;
    }
}

/**
 * @brief Regra de derivação que analiza expressoes booleanas
 *        no padrao 'id op_rel expr'
 * 
 */
////// TODO
int B() {
    int operator;
    char lexema_of_id[MAX_CHAR];
    strcpy(lexema_of_id, lookahead->lexema);
    match(ID);
    //gen_id(lexema_of_id);
    if ( boolOperator(&operator) ) {
        E();
        gen_bool(operator);
        return true;
    } else {
        return false;
    }
}

int boolOperator(int *operator) {
    int lookahead_tag;
    lookahead_tag = lookahead->tag;
    *operator = -1;
    
    if (lookahead_tag == EQUAL ||
        lookahead_tag == NE ||
        lookahead_tag == LT ||
        lookahead_tag == GT ||
        lookahead_tag == LE ||
        lookahead_tag == GE) {
            *operator = lookahead_tag;
            match(lookahead_tag);
            return true;
    } else {
        printf("[ERRO] Operador relacional experado.\n");
        return false;
    }
}

//------
/* Funções que representam a gramatica que reconhece expressoes aritmeticas */
/* Elaborada nas primeiras aulas */
int E() {
    int b1, b2;
    b1 = T();
    if (b1) 
        b2 = ER();
    return b1 && b2;
}

int ER() {
    if (lookahead->tag == '+')  { 
        int b1, b2;
        match('+');
        b1 = T();
        genAdd();
        if (b1) 
            b2 = ER();
        return b1 && b2;
    } else if (lookahead->tag == '-') {
        int b1, b2;
        match('-');
        b1 = T();
        genSub();
        if (b1)
            b2 = ER();
        return b1 && b2;      
    } else if (lookahead -> tag == ')') {
        return true;
    } else if (lookahead -> tag == ENDTOKEN) {
        return true;
    } else if (lookahead -> tag == '*') {
        return true;
    } else if (lookahead -> tag == '/') {
        return true;
    } else {
        return false;
    }
}

int T() {
    int b1, b2;
    b1 = F();
    if (b1)
        b2 = TR();
    return b1 && b2;
}

int TR() {
    if (lookahead->tag == '*') { 
        int b1, b2;
        match('*');
        b1 = F();
        genMult();
        if (b1)
            b2 = TR();
        return b1 && b2;
    } else if (lookahead -> tag == '/') {
        int b1, b2;
        match('/');
        b1 = F();
        genDiv();
        if (b1)
            b2 = TR();
        return b1 && b2;
    } else if (lookahead->tag == ')') {
        return true;
    } else if (lookahead->tag == ENDTOKEN){ //EOF
        return true;
    } else if (lookahead->tag == '+') {
        return true;
    } else if (lookahead->tag == '-') {
        return true;
    } else {
        return false;
    }
}

int F() {
    if (lookahead->tag == '(') {
        int b1,b2;
        match('(');
        b1 = E();
        if (b1)
            b2 = match(')');
        return b1 && b2;
    } else if (lookahead->tag == NUM) {
        int b1;
        char lexema[MAX_TOKEN];
        strcpy(lexema, lookahead->lexema);
        b1 = match(NUM); //substituir 'id' por NUM
        genNum(lexema);
        return b1;
    } else {
        return false;
    }
}
/*fim da importação */
//------


//--------------------- MAIN -----------------------

/**
 * @brief Funcao principal (main) do compilador
 * 
 * @return int 
 */
int main(int argc, char *argv[]) {

    //Inicializa a tabela de simbolo global
    initSymbolTableVariables(&global_symbol_table_variables);
    initSymbolTableString();
    initSymbolTableFunc();

    //Verifica a passagem de parametro
    if (argc != 2) {
        printf("[ERRO]\n\tÉ necessário informar um arquivo de entrada (código) como parâmetro.\n\n");
        exit(EXIT_FAILURE);
    }
    
    initLex(argv[1]); //Carrega codigo
    lookahead = getToken(); //Inicializacao do lookahead

    //Abre o arquivo de saida 
    strcpy(output_file_name, argv[1]);
    strcat(output_file_name, ".asm");
    output_file = fopen(output_file_name, "w+");

    program(); //Chamada da derivacao/funcao inicial da gramatica

    fclose(output_file);
    return 1;
}