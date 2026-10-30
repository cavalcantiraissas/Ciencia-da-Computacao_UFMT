/**
 * @file synt.c
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Codificacao do modulo do analisador sintatico
 * @version 0.5
 * @date 2022-02-04
 *
 */

// Inclusao do cabecalho
#include "synt.h"
#include <stdarg.h>

// Variaveis globais
type_token *lookahead;
int n_errors = 0; //Quantidade de erros encontrados na compilacao
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
extern char output_file_name[FILENAME_MAX];
extern FILE *output_file;

/**
 * @brief Emite uma mensagem de erro e contabiliza o erro
 *
 * @param fmt formato no padrao printf (sem o '\n' final)
 */
void error(const char *fmt, ...) {
    va_list args;
    printf("[ERRO] ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    n_errors++;
}

/**
 * @brief Devolve uma descricao legivel do token, usada nas mensagens de erro
 *
 * @param token_tag (int) codigo do token
 * @return const char* descricao
 */
const char *tagName(int token_tag) {
    static char name[16];
    switch (token_tag) {
        case ID:        return "identificador";
        case NUM:       return "numero";
        case BEGIN:     return "'{' (ou 'begin')";
        case END:       return "'}' (ou 'end')";
        case ENDTOKEN:  return "fim de arquivo";
        default:
            if (token_tag > 0 && token_tag < 256) {
                sprintf(name, "'%c'", token_tag);
                return name;
            }
            sprintf(name, "token %d", token_tag);
            return name;
    }
}

/**
 * @brief Devolve o lexema do token atual (lookahead), usado nas mensagens de erro
 *
 * @return const char* lexema ou "fim de arquivo"
 */
const char *currentLexeme(void) {
    if (lookahead->tag == ENDTOKEN)
        return "fim de arquivo";
    return lookahead->lexema;
}

/**
 * @brief Verifica se o proximo caracter (a frente) na cadeia eh o esperado
 *
 * @param token_tag (int) codigo do token a ser verificado
 * @return int true/false
 */
int match(int token_tag) {
    if ( lookahead->tag == token_tag ) {
        free(lookahead);
        lookahead = getToken(); //Pega o proximo token por meio do lexico
        return true;
    }
    error("Entrada inesperada: '%s'. Esperado: %s.", currentLexeme(), tagName(token_tag));
    return false;
}

/**
 * @brief Regra de derivacao inicial
 * @return int true/false
 */
int program (void) {
    gen_preambule(); //Temporariamente cria um preambulo adicional que permite o uso das funcoes scanf e printf
    if ( !declarations() )
        return false;
    gen_preambule_code(); //Chamada do gerador de codigo para escrita do cabecalho da secao de codigo
    if ( !statements() )
        return false;
    //Os comandos so podem terminar no fim do arquivo (um '}' aqui nao tem '{' correspondente)
    if ( lookahead->tag != ENDTOKEN ) {
        error("Fim de escopo '%s' sem inicio de escopo correspondente.", currentLexeme());
        return false;
    }
    gen_epilog_code();
    gen_runtime(); //rotinas de apoio: conversao texto/numero na entrada e saida
    gen_data_section(); //Chamada do gerador de codigo para declaracao de variaveis
    return true;
}

/**
 * @brief Regra de derivacao para declaracoes
 * @return int true se todas as declaracoes estao corretas
 */
int declarations(void) {
    int errors_before = n_errors;
    while ( declaration() ); //Laco para processamento continuo das declaracoes
    return n_errors == errors_before;
}

/**
 * @brief Regra de derivacao declaracao
 * @return int true se reconheceu uma declaracao; false no fim das declaracoes ou em erro
 */
int declaration (void) {
    type_symbol_table_entry *search_symbol;
    char var_name[MAX_CHAR];
    int var_type;

    //Verifica o tipo da variavel
    var_type = lookahead->tag;
    if ( var_type == INT || var_type == FLOAT || var_type == CHAR || var_type == STRING) {
        match(var_type);
        if ( lookahead->tag != ID ) {
            error("Identificador esperado apos o tipo, encontrado '%s'.", currentLexeme());
            return false;
        }
        strcpy(var_name, lookahead->lexema);
        search_symbol = sym_find( var_name, &global_symbol_table_variables );

        if ( search_symbol != NULL) {
            error("Variavel '%s' ja declarada.", var_name);
            return false;
        }
        sym_declare( var_name, var_type, 0, &global_symbol_table_variables);
        match(ID);
        return match(SEMICOLON); //Verifica se ; vem a seguir
    }
    //Qualquer outro token (if, while, read, write, id, fim de arquivo, ...)
    //encerra as declaracoes; ele sera tratado pela regra 'statements'
    return false;
}

/**
 * @brief Regra de derivacao para comandos
 * @return int true se os comandos terminaram sem erro (no fim de escopo ou de arquivo)
 */
int statements (void) {
    int errors_before = n_errors;
    while ( statement() );  //processa enquanto houver comandos
    return n_errors == errors_before;
}

/**
 * @brief Regra de derivacao que processa os comandos
 *
 * @return int true se reconheceu um comando; false no fim de escopo ('}'),
 *         no fim de arquivo ou em erro
 */
int statement (void) {
    char lexeme_of_id[MAX_CHAR];
    type_symbol_table_entry *search_symbol;
    type_symbol_table_string_entry *gen_string;
    int type;
    char string_value[MAX_CHAR];

    if (lookahead->tag == READ) {
        match(READ);
        strcpy(lexeme_of_id, lookahead->lexema);
        if ( !match(ID) )
            return false;
        search_symbol = sym_find(lexeme_of_id, &global_symbol_table_variables);
        if (search_symbol == NULL) {
            error("Simbolo desconhecido (Variavel nao declarada): %s", lexeme_of_id);
            return false;
        }
        type = search_symbol->type;
        gen_read(lexeme_of_id, type);
        return match(SEMICOLON);
    } else if (lookahead->tag == WRITE) {
        match(WRITE);
        if ( lookahead->tag == STRING_CONST ) {
            strcpy(string_value, lookahead->lexema);
            //Reaproveita a constante caso ela ja esteja na tabela de simbolos
            gen_string = sym_string_find(string_value);
            if ( gen_string == NULL )
                gen_string = sym_string_declare(string_value);
            if ( gen_string == NULL ) {
                n_errors++; //mensagem ja emitida por sym_string_declare
                return false;
            }
            match(STRING_CONST);
            strcpy(lexeme_of_id, gen_string->name);
            gen_write(lexeme_of_id, STRING);
            return match(SEMICOLON);
        } else if ( lookahead->tag == ID) {
            strcpy(lexeme_of_id, lookahead->lexema);
            match(ID);
            search_symbol = sym_find(lexeme_of_id, &global_symbol_table_variables);
            if (search_symbol == NULL) {
                error("Simbolo desconhecido (Variavel nao declarada): %s", lexeme_of_id);
                return false;
            }
            type = search_symbol->type;
            gen_write(lexeme_of_id, type);
            return match(SEMICOLON);
        } else {
            error("Comando write espera uma variavel ou uma constante string, encontrado '%s'.", currentLexeme());
            return false;
        }
    } else if (lookahead->tag == IF) {
        return ifStmt();
    } else if (lookahead->tag == WHILE) {
        return whileStmt();
    } else if (lookahead->tag == ID) {
        return assignStmt();
    } else if ( (lookahead->tag == ENDTOKEN) || (lookahead->tag == END) ) {
        //Fim de arquivo ou fim de escopo ('}' / 'end'): nao ha mais comandos
        return false;
    } else {
        error("Comando desconhecido: '%s'.", currentLexeme());
        return false;
    }
}

/**
 * @brief Regra de derivacao de um bloco de comandos: '{' statements '}'
 * (ou begin statements end)
 *
 * @return int true/false
 */
int block(void) {
    if ( !match(BEGIN) )
        return false;
    if ( !statements() )
        return false;
    return match(END);
}

/**
 * @brief Regra de derivacao do comando IF:
 *        if '(' B ')' block [ else block ]
 *
 * @return int true se todas as derivacoes internas foram reconhecidas
 */
int ifStmt(void) {
    char label_else[MAX_CHAR];
    char label_end[MAX_CHAR];
    gen_label_name(label_else);
    gen_label_name(label_end);

    match(IF);
    if ( !match(OPEN_PAR) )
        return false;
    if ( !B() ) { //Expressao booleana
        error("Expressao booleana invalida no comando if.");
        return false;
    }
    gen_cond_jump(label_else);
    if ( !match(CLOSE_PAR) )
        return false;
    if ( !block() )
        return false;

    //Verifica se ocorre um ELSE
    if (lookahead->tag == ELSE) {
        gen_incond_jump(label_end);
        gen_label(label_else);
        match(ELSE);
        if ( !block() )
            return false;
        gen_label(label_end);
    } else {
        gen_label(label_else);
    }
    return true;
}

/**
 * @brief Regra de derivacao do comando WHILE:
 *        while '(' B ')' block
 *
 * @return int true se todas as derivacoes internas foram reconhecidas
 */
int whileStmt(void) {
    char label_begin[MAX_CHAR];
    char label_end[MAX_CHAR];
    gen_label_name(label_begin);
    gen_label_name(label_end);

    match(WHILE);
    gen_label(label_begin); //o teste eh reavaliado a cada iteracao
    if ( !match(OPEN_PAR) )
        return false;
    if ( !B() ) { //Expressao booleana
        error("Expressao booleana invalida no comando while.");
        return false;
    }
    gen_cond_jump(label_end);
    if ( !match(CLOSE_PAR) )
        return false;
    if ( !block() )
        return false;
    gen_incond_jump(label_begin);
    gen_label(label_end);
    return true;
}

/**
 * @brief Regra de derivacao da atribuicao: id '=' valor ';'
 *        O valor deve corresponder ao tipo da variavel:
 *          int    -> expressao inteira (int/char)
 *          float  -> expressao inteira ou float (inteiro eh convertido)
 *          char   -> expressao inteira (constante char, variavel char, ...)
 *          string -> constante string ou outra variavel string
 * 
 * @return int true/false
 */
int assignStmt(void) {
    char var_name[MAX_CHAR];
    type_symbol_table_entry *var;
    int kind;

    strcpy(var_name, lookahead->lexema);
    var = sym_find(var_name, &global_symbol_table_variables);
    if ( var == NULL ) {
        error("Variavel '%s' nao declarada.", var_name);
        return false;
    }
    match(ID);
    if ( !match(ASSIGN) )
        return false;

    if ( var->type == STRING ) {
        if ( !stringValue(var_name) )
            return false;
        return match(SEMICOLON);
    }

    if ( lookahead->tag == STRING_CONST ) {
        error("Tipo incompativel: variavel '%s' (%s) nao pode receber a constante string %s.",
            var_name, typeName(var->type), lookahead->lexema);
        return false;
    }
    kind = E();
    if ( !kind )
        return false;
    if ( kind == EXPR_BOOL ) {
        error("Tipo incompativel: variavel '%s' (%s) deve receber uma expressao aritmetica, nao uma comparacao.",
            var_name, typeName(var->type));
        return false;
    }
    switch (var->type) {
        case INT:
        case CHAR:
            if ( kind == EXPR_FLOAT ) {
                error("Tipo incompativel: variavel '%s' (%s) nao pode receber uma expressao float (perda da parte fracionaria).",
                    var_name, typeName(var->type));
                return false;
            }
            break;
        case FLOAT:
            if ( kind == EXPR_INT )
                genIntToFloat(0);
            break;
    }
    gen_assign(var_name, var->type);
    return match(SEMICOLON);
}

/**
 * @brief Processa o valor atribuido a uma variavel string: uma constante
 *        string ou outra variavel string
 * 
 * @param var_name nome da variavel string de destino
 * @return int true/false
 */
int stringValue(char *var_name) {
    type_symbol_table_string_entry *str;
    type_symbol_table_entry *source;

    if ( lookahead->tag == STRING_CONST ) {
        str = sym_string_find(lookahead->lexema);
        if ( str == NULL )
            str = sym_string_declare(lookahead->lexema);
        if ( str == NULL ) {
            n_errors++; //mensagem ja emitida por sym_string_declare
            return false;
        }
        gen_assign_string(var_name, str->name);
        return match(STRING_CONST);
    }
    if ( lookahead->tag == ID ) {
        source = sym_find(lookahead->lexema, &global_symbol_table_variables);
        if ( source == NULL ) {
            error("Variavel '%s' nao declarada.", lookahead->lexema);
            return false;
        }
        if ( source->type != STRING ) {
            error("Tipo incompativel: variavel '%s' (string) nao pode receber a variavel '%s' (%s).",
                var_name, source->name, typeName(source->type));
            return false;
        }
        gen_assign_string(var_name, source->name);
        return match(ID);
    }
    error("Tipo incompativel: variavel '%s' (string) deve receber uma constante string ou uma variavel string, encontrado '%s'.",
        var_name, currentLexeme());
    return false;
}

/**
 * @brief Devolve o nome de um tipo de variavel, usado nas mensagens de erro
 */
const char *typeName(int type) {
    switch (type) {
        case INT:    return "int";
        case FLOAT:  return "float";
        case CHAR:   return "char";
        case STRING: return "string";
        default:     return "desconhecido";
    }
}

/**
 * @brief Indica se o tipo de expressao eh numerico (int ou float)
 */
int isNumeric(int kind) {
    return kind == EXPR_INT || kind == EXPR_FLOAT;
}

/**
 * @brief Iguala os tipos dos dois operandos do topo da pilha antes de uma
 *        operacao: se um for int e o outro float, o int eh convertido.
 * 
 * @param left tipo do operando da esquerda (abaixo do topo)
 * @param right tipo do operando da direita (topo da pilha)
 * @return int tipo resultante (EXPR_INT ou EXPR_FLOAT)
 */
int promote(int left, int right) {
    if ( left == right )
        return left;
    if ( left == EXPR_INT )
        genIntToFloat(1); //converte o operando da esquerda
    else
        genIntToFloat(0); //converte o operando da direita
    return EXPR_FLOAT;
}

/**
 * @brief Regra de derivação que analisa expressoes booleanas, no padrao
 *        '[id | exp_aritmetica] op_rel [id | exp_aritmetica]', combinadas
 *        pelos operadores logicos '&&' e '||'
 *
 * @return int true se a expressao eh booleana e bem formada; false caso contrario
 */
int B() {
    int kind;
    kind = BOr();
    if ( !kind )
        return false; //erro especifico ja emitido
    if ( kind != EXPR_BOOL ) {
        error("Expressao booleana esperada: use um operador relacional (<, >, <=, >=, ==, !=), por exemplo 'x > 0'.");
        return false;
    }
    return true;
}

/**
 * @brief BOr -> BAnd BOrR
 * @return int tipo da expressao (EXPR_INT / EXPR_FLOAT / EXPR_BOOL) ou false em erro
 */
int BOr() {
    int kind;
    kind = BAnd();
    if ( !kind )
        return false;
    return BOrR(kind);
}

/**
 * @brief BOrR -> '||' BAnd BOrR | [vazio]
 * @param left tipo do operando a esquerda
 * @return int tipo da expressao ou false em erro
 */
int BOrR(int left) {
    if (lookahead->tag == OR) {
        int right;
        char label_end[MAX_CHAR];
        match(OR);
        if ( left != EXPR_BOOL ) {
            error("O operador '||' exige operandos booleanos (comparacoes).");
            return false;
        }
        gen_logic_begin(OR, label_end); //curto-circuito: pula a direita se a esquerda for verdadeira
        right = BAnd();
        if ( !right )
            return false;
        if ( right != EXPR_BOOL ) {
            error("O operador '||' exige operandos booleanos (comparacoes).");
            return false;
        }
        gen_label(label_end);
        return BOrR(EXPR_BOOL);
    }
    return left;
}

/**
 * @brief BAnd -> BRel BAndR
 * @return int tipo da expressao ou false em erro
 */
int BAnd() {
    int kind;
    kind = BRel();
    if ( !kind )
        return false;
    return BAndR(kind);
}

/**
 * @brief BAndR -> '&&' BRel BAndR | [vazio]
 * @param left tipo do operando a esquerda
 * @return int tipo da expressao ou false em erro
 */
int BAndR(int left) {
    if (lookahead->tag == AND) {
        int right;
        char label_end[MAX_CHAR];
        match(AND);
        if ( left != EXPR_BOOL ) {
            error("O operador '&&' exige operandos booleanos (comparacoes).");
            return false;
        }
        gen_logic_begin(AND, label_end); //curto-circuito: pula a direita se a esquerda for falsa
        right = BRel();
        if ( !right )
            return false;
        if ( right != EXPR_BOOL ) {
            error("O operador '&&' exige operandos booleanos (comparacoes).");
            return false;
        }
        gen_label(label_end);
        return BAndR(EXPR_BOOL);
    }
    return left;
}

/**
 * @brief BRel -> E [ rel_operator E ]
 *        Cada lado pode ser um id, um numero ou uma expressao aritmetica
 *        (int ou float; se os tipos forem diferentes, o int eh convertido).
 * @return int tipo da expressao ou false em erro
 */
int BRel() {
    int left, right, operator;
    char operator_lexeme[MAX_CHAR];

    left = E();
    if ( !left )
        return false;
    if ( !isRelOperator(lookahead->tag) )
        return left; //sem operador relacional: o tipo eh o da expressao

    strcpy(operator_lexeme, lookahead->lexema);
    boolOperator(&operator);
    right = E();
    if ( !right )
        return false;
    if ( !isNumeric(left) || !isNumeric(right) ) {
        error("O operador '%s' exige operandos aritmeticos (id ou expressao aritmetica).", operator_lexeme);
        return false;
    }
    gen_bool(operator, promote(left, right));
    return EXPR_BOOL;
}

int isRelOperator(int tag) {
    return tag == EQUAL || tag == NE || tag == LT || tag == GT || tag == LE || tag == GE;
}

int boolOperator(int *operator) {
    int lookahead_tag;
    lookahead_tag = lookahead->tag;
    *operator = -1;

    if ( isRelOperator(lookahead_tag) ) {
        *operator = lookahead_tag;
        match(lookahead_tag);
        return true;
    } else {
        error("Operador relacional esperado, encontrado '%s'.", currentLexeme());
        return false;
    }
}

//------
/* Funções que representam a gramatica que reconhece expressoes aritmeticas */
/* Elaborada nas primeiras aulas. Retornam o tipo da expressao reconhecida  */
/* (EXPR_INT, EXPR_FLOAT ou EXPR_BOOL, este ultimo apenas entre           */
/* parenteses), ou false em caso de erro.                                   */
int E() {
    int kind;
    kind = T();
    if ( !kind )
        return false;
    return ER(kind);
}

int ER(int left) {
    if (lookahead->tag == '+' || lookahead->tag == '-') {
        int right, operator, kind;
        operator = lookahead->tag;
        match(operator);
        right = T();
        if ( !right )
            return false;
        if ( !isNumeric(left) || !isNumeric(right) ) {
            error("O operador '%c' exige operandos aritmeticos.", operator);
            return false;
        }
        kind = promote(left, right);
        if (operator == '+')
            genAdd(kind);
        else
            genSub(kind);
        return ER(kind);
    }
    return left; //[vazio]: o proximo token eh verificado por quem chamou
}

int T() {
    int kind;
    kind = F();
    if ( !kind )
        return false;
    return TR(kind);
}

int TR(int left) {
    if (lookahead->tag == '*' || lookahead->tag == '/') {
        int right, operator, kind;
        operator = lookahead->tag;
        match(operator);
        right = F();
        if ( !right )
            return false;
        if ( !isNumeric(left) || !isNumeric(right) ) {
            error("O operador '%c' exige operandos aritmeticos.", operator);
            return false;
        }
        kind = promote(left, right);
        if (operator == '*')
            genMult(kind);
        else
            genDiv(kind);
        return TR(kind);
    }
    return left; //[vazio]
}

int F() {
    if (lookahead->tag == '(') {
        int kind;
        match('(');
        kind = BOr(); //permite agrupar tanto expressoes aritmeticas quanto booleanas
        if ( !kind )
            return false;
        if ( !match(')') )
            return false;
        return kind;
    } else if (lookahead->tag == '-') { //menos unario
        int kind;
        match('-');
        kind = F();
        if ( !kind )
            return false;
        if ( !isNumeric(kind) ) {
            error("O menos unario exige um operando aritmetico.");
            return false;
        }
        genNeg(kind);
        return kind;
    } else if (lookahead->tag == NUM) {
        char lexema[MAX_TOKEN];
        strcpy(lexema, lookahead->lexema);
        match(NUM);
        if ( strchr(lexema, '.') != NULL ) {
            genFloatNum(lexema);
            return EXPR_FLOAT;
        }
        genNum(lexema);
        return EXPR_INT;
    } else if (lookahead->tag == CHAR_CONST) { //char eh tratado como inteiro (codigo ASCII)
        char value[MAX_TOKEN];
        sprintf(value, "%d", lookahead->value);
        match(CHAR_CONST);
        genNum(value);
        return EXPR_INT;
    } else if (lookahead->tag == ID) {
        char lexema[MAX_TOKEN];
        type_symbol_table_entry *search_symbol;
        strcpy(lexema, lookahead->lexema);
        search_symbol = sym_find(lexema, &global_symbol_table_variables);
        if ( search_symbol == NULL ) {
            error("Variavel '%s' nao declarada.", lexema);
            return false;
        }
        if ( search_symbol->type == STRING ) {
            error("Variavel '%s' eh do tipo string e nao pode ser usada em expressoes.", lexema);
            return false;
        }
        match(ID);
        gen_push_var(lexema, search_symbol->type); //empilha o conteudo da variavel
        return search_symbol->type == FLOAT ? EXPR_FLOAT : EXPR_INT;
    } else {
        error("Operando esperado (id, numero, constante char ou '('), encontrado '%s'.", currentLexeme());
        return false;
    }
}
/*fim da importação */
//------


//--------------------- MAIN -----------------------

/**
 * @brief Funcao principal (main) do compilador
 *
 * @return int EXIT_SUCCESS ou EXIT_FAILURE
 */
int main(int argc, char *argv[]) {
    int ok;

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

    ok = program(); //Chamada da derivacao/funcao inicial da gramatica

    fclose(output_file);
    free(lookahead);
    if ( !ok || n_errors > 0 ) {
        remove(output_file_name); //nao deixa um .asm incompleto
        printf("FALHA NA COMPILACAO DO PROGRAMA (%d erro(s))\n", n_errors);
        return EXIT_FAILURE;
    }
    printf("Compilacao concluida: %s\n", output_file_name);
    return EXIT_SUCCESS;
}
