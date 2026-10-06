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
int compile_error = false; //indica se algum erro foi encontrado na compilacao
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
extern char output_file_name[FILENAME_MAX];
extern FILE *output_file;

/**
 * @brief Verifica se o proximo caracter (a frente) na cadeia eh o esperado
 *
 * @param token_tag (int) codigo do token a ser verificado
 * @return int true/false
 */
int match(int token_tag) {
    if ( lookahead->tag == token_tag ) {
        free(lookahead); //token ja consumido
        lookahead = getToken(); //Pega o proximo token por meio do lexico
        return true;
    }
    if (lookahead->tag == ENDTOKEN)
        printf("[ERRO] Fim de arquivo inesperado.");
    else
        printf("[ERRO] Entrada inesperada: '%s'.", lookahead->lexema);
    if (token_tag > 0 && token_tag < 256)
        printf(" Esperado: '%c'.", token_tag);
    printf("\n");
    compile_error = true;
    return false;
}

/**
 * @brief Nome do tipo de uma variavel, para mensagens de erro
 *
 * @param type (int) INT, FLOAT, CHAR ou STRING
 * @return const char*
 */
static const char *typeName(int type) {
    switch (type) {
        case INT:    return "int";
        case FLOAT:  return "float";
        case CHAR:   return "char";
        case STRING: return "string";
        default:     return "desconhecido";
    }
}

/**
 * @brief Verifica se a constante pode ser atribuida a variavel. Regras:
 *        int    <- constante inteira (dentro do intervalo de int)
 *        float  <- constante float ou inteira (convertida para float)
 *        char   <- constante char
 *        string <- constante string
 *
 * @param var variavel (ja declarada) que recebe o valor
 * @param const_tag tag do token da constante
 * @param const_lexeme lexema da constante
 * @return int true/false
 */
static int checkAssignCompatible(type_symbol_table_entry *var, int const_tag, char *const_lexeme) {
    const char *const_type;
    int ok;

    switch (const_tag) {
        case NUM:          const_type = "int";    break;
        case FLOAT_NUM:    const_type = "float";  break;
        case CHAR_CONST:   const_type = "char";   break;
        case STRING_CONST: const_type = "string"; break;
        case ERROR:
            return false; //erro lexico ja informado pelo analisador lexico
        case ENDTOKEN:
            printf("[ERRO] Fim de arquivo inesperado na atribuicao a '%s'.\n", var->name);
            return false;
        default:
            printf("[ERRO] Constante esperada na atribuicao a '%s', encontrado '%s'.\n",
                var->name, const_lexeme);
            return false;
    }

    switch (var->type) {
        case INT:    ok = (const_tag == NUM); break;
        case FLOAT:  ok = (const_tag == NUM || const_tag == FLOAT_NUM); break;
        case CHAR:   ok = (const_tag == CHAR_CONST); break;
        case STRING: ok = (const_tag == STRING_CONST); break;
        default:     ok = false; break;
    }
    if (!ok) {
        printf("[ERRO] Tipo incompativel: variavel '%s' (%s) nao pode receber a constante %s (%s).\n",
            var->name, typeName(var->type), const_lexeme, const_type);
        return false;
    }

    // constante inteira grande demais para um int (4 bytes)
    if (var->type == INT && strtoll(const_lexeme, NULL, 10) > INT_MAX) {
        printf("[ERRO] Constante %s fora do intervalo de int (maximo %d).\n", const_lexeme, INT_MAX);
        return false;
    }
    return true;
}

/**
 * @brief Regra de derivacao inicial
 */
void program (void) {
    gen_preambule(); //Temporariamente cria um preambulo adicional que permite o uso das funcoes scanf e printf
    declarations();
    if (compile_error) return; //evita erros em cascata nos comandos
    gen_preambule_code(); //Chamada do gerador de codigo para escrita do cabecalho da secao de codigo
    statements();
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
    int ok1, ok2;
    char var_name[MAX_CHAR];
    int var_type;

    //Verifica o tipo da variavel
    var_type = lookahead->tag;
    if ( var_type == INT || var_type == FLOAT || var_type == CHAR || var_type == STRING) {
        match(var_type);
        if (lookahead->tag != ID) {
            if (lookahead->tag != ERROR) //erro lexico ja foi informado
                printf("[ERRO] Identificador esperado apos o tipo '%s', encontrado '%s'.\n",
                    typeName(var_type), lookahead->lexema);
            compile_error = true;
            return false;
        }
        strcpy(var_name, lookahead->lexema);
        search_symbol = sym_find( var_name, &global_symbol_table_variables );

        if ( search_symbol != NULL) {
            printf ("[ERRO] Variavel '%s' ja declarada.\n", var_name);
            compile_error = true;
            return false;
        } else {
            sym_declare( var_name, var_type, 0, &global_symbol_table_variables);
            ok1 = match(ID); //Verifica se identificador vem a seguir
            ok2 = ok1 && match(SEMICOLON); //Verifica se ; vem a seguir
            return ok1 && ok2;
        }
    } else if (lookahead->tag == ENDTOKEN ||
                lookahead->tag == ID ||
                lookahead->tag == READ ||
                lookahead->tag == WRITE) {
        //Verifica se fim de arquivo ou inicio dos comandos
        return false;
    } else {
        printf ("[ERRO] Tipo desconhecido: '%s'.\n", lookahead->lexema);
        compile_error = true;
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
 * @brief Regra de derivacao que processa os comandos. Por enquanto, apenas a
 * atribuicao de constante a variavel: id = constante;
 *
 * @return int true/false
 */
int statement (void) {
    char lexeme_of_id[MAX_CHAR];
    char const_lexeme[MAX_CHAR];
    type_symbol_table_entry *search_symbol;
    type_symbol_table_string_entry *gen_string = NULL;
    int const_tag;

    if (lookahead->tag == ID) {
        //Verifica se a variavel ID esta na TS
        strcpy(lexeme_of_id, lookahead->lexema);
        search_symbol = sym_find(lexeme_of_id, &global_symbol_table_variables);
        if (search_symbol == NULL) {
            printf("[ERRO] Variavel '%s' nao declarada.\n", lexeme_of_id);
            compile_error = true;
            return false;
        }
        match(ID);

        //Confirma se o proximo token eh ASSIGN
        if (!match(ASSIGN))
            return false;

        //Verifica se a constante eh compativel com o tipo da variavel na TS
        const_tag = lookahead->tag;
        strcpy(const_lexeme, lookahead->lexema);
        if (!checkAssignCompatible(search_symbol, const_tag, const_lexeme)) {
            compile_error = true;
            return false;
        }
        match(const_tag);
        if (!match(SEMICOLON))
            return false;

        //Atribui o valor para a variavel na TS
        if (!sym_set_value(search_symbol, const_lexeme)) {
            compile_error = true;
            return false;
        }

        //Constante string vai para a tabela de strings (reaproveita se repetida)
        if (search_symbol->type == STRING) {
            gen_string = sym_string_find(const_lexeme);
            if (gen_string == NULL)
                gen_string = sym_string_declare(const_lexeme);
            if (gen_string == NULL) {
                compile_error = true;
                return false;
            }
        }

        //Gera codigo de atribuicao
        gen_assign(search_symbol, gen_string);
        return true;
    } else if (lookahead->tag == ENDTOKEN) {
        return false;
    } else {
        printf("[ERRO] Comando desconhecido: '%s'.\n", lookahead->lexema);
        compile_error = true;
        return false;
    }
}




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

    //Verifica a passagem de parametro
    if (argc != 2) {
        printf("[ERRO]\n\tÉ necessário informar um arquivo de entrada (código) como parâmetro.\n\n");
        exit(EXIT_FAILURE);
    }

    initLex(argv[1]); //Carrega codigo
    lookahead = getToken(); //Inicializacao do lookahead

    //Abre o arquivo de saida
    snprintf(output_file_name, sizeof(output_file_name), "%s.asm", argv[1]);
    output_file = fopen(output_file_name, "w+");
    if (output_file == NULL) {
        printf("[ERRO]\n\tNao foi possivel criar o arquivo de saida: %s\n\n", output_file_name);
        exit(EXIT_FAILURE);
    }

    program(); //Chamada da derivacao/funcao inicial da gramatica

    fclose(output_file);
    free(lookahead);

    printSTVariables(&global_symbol_table_variables);
    printSTString();

    if (compile_error) {
        remove(output_file_name); //nao deixa um .asm incompleto
        printf("\nFALHA NA COMPILACAO DO PROGRAMA\n");
        return EXIT_FAILURE;
    }
    printf("\nCodigo gerado em: %s\n", output_file_name);
    return EXIT_SUCCESS;
}
