/**
 * @file lex.c
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Codificacao do modulo do analisador lexico
 * @version 0.3
 * @date 2021-12-09
 * 
 */

// Inclusao do cabecalho
#include "lex.h"

// Variaveis globais
int pos;
FILE *input_file;

// Pilha de caracteres devolvidos ao fluxo de entrada. O padrao C so garante
// um caractere de 'ungetc', mas o caso "7." precisa devolver dois.
static int pushback[2];
static int n_pushback = 0;

static int nextChar(void) {
    if (n_pushback > 0)
        return pushback[--n_pushback];
    return fgetc(input_file);
}

static void backChar(int c) {
    if (c != EOF && n_pushback < 2)
        pushback[n_pushback++] = c;
}

// Adiciona caractere ao buffer apenas se houver espaco (evita overflow).
// Retorna false quando o lexema excede MAX_CHAR-1 caracteres.
static int addChar(char *buffer, int *pos_buffer, int c) {
    if (*pos_buffer < MAX_CHAR - 1) {
        buffer[(*pos_buffer)++] = (char) c;
        return true;
    }
    return false;
}
//Definicao e inicializacao de estrutura
type_token key_words[] = {
    {IF, "if", 0},
    {THEN, "then", 0},
    {ELSE, "else", 0},
    {WHILE, "while", 0},
    {DO, "do", 0},
    {INT, "integer", 0},
    {READ, "read", 0},
    {WRITE, "write", 0},
    {INT, "int", 0},
    {FLOAT, "float", 0},
    {0, "", 0}  //sentinela: marca o fim da lista para keyWordFind
};


/**
 * @brief Processo de inicializacao do lexico. Recebe o arquivo de entrada
 * contendo codigo e carrega-o em memoria.
 * 
 */
void initLex(char input_file_name[]) {
    input_file = fopen(input_file_name, "r");
    if (input_file == NULL) {
        printf("[ERRO]\n\tArquivo de entrada não encontrado: %s\n\n", input_file_name);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Analisa o proximo comando no código de entrada e retorna estrutura de
 * dados com Token correspondente (ponteiro).
 * 
 * @return type_token* 
 */
type_token *getToken() {
    char buffer[MAX_CHAR];
    int pos_buffer;
    type_token *token;
    type_token *key_found;
    int ch; //int (e nao char) para comparar corretamente com EOF
    
    int too_long = false;

    pos_buffer = 0;
    token = (type_token*) malloc(sizeof(type_token));
    strcpy(buffer, "");
    ch = nextChar();

    // Consome espacos, tabulacoes e quebras de linha
    while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
        ch = nextChar();
    }

    // Verifica se NUMERO (inteiro ou com precisao/float)
    if ( isdigit(ch) ) {
        int isFloat = false;

        // constroi buffer com a parte inteira do numero
        while ( isdigit(ch) ) {
            if (!addChar(buffer, &pos_buffer, ch)) too_long = true;
            ch = nextChar();
        }

        // Verifica se ha parte fracionaria (ponto seguido de digito),
        // caracterizando um numeral de PRECISAO (float). Ex.: 3.14
        if ( ch == '.' ) {
            int next_ch = nextChar();
            if ( isdigit(next_ch) ) {
                isFloat = true;
                if (!addChar(buffer, &pos_buffer, ch)) too_long = true;  // guarda o '.'
                ch = next_ch;
                while ( isdigit(ch) ) {           // guarda os digitos decimais
                    if (!addChar(buffer, &pos_buffer, ch)) too_long = true;
                    ch = nextChar();
                }
            } else {
                // Nao eh um float (ex.: "3." sem digito apos o ponto):
                // devolve o caractere apos o ponto e tambem o proprio ponto
                backChar(next_ch);
            }
        }

        backChar(ch);
        buffer[pos_buffer] = '\0';
        token->tag = isFloat ? FLOAT_NUM : NUM;
        strcpy( token->lexema, buffer ); //copia buffer para lexema
        token->value = 0;
        if (too_long) {
            printf("[ERRO] Numero excede %d caracteres: '%s...'\n", MAX_CHAR - 1, buffer);
            token->tag = ERROR;
        }
    } //Verifica se entrada eh um alfa-numerico (palavra reservada ou identificador)
    else if ( isalpha(ch) ) {
        if (!addChar(buffer, &pos_buffer, ch)) too_long = true;
        ch = nextChar();
        while( isalnum(ch) ) {
            if (!addChar(buffer, &pos_buffer, ch)) too_long = true;
            ch = nextChar();
        }
        backChar(ch);
        buffer[pos_buffer] = '\0';
        key_found = keyWordFind(buffer);
        if (too_long) {
            printf("[ERRO] Identificador excede %d caracteres: '%s...'\n", MAX_CHAR - 1, buffer);
            token->tag = ERROR;
            strcpy(token->lexema, buffer);
            token->value = 0;
        } else if (key_found != NULL) { //Palavra reservada
            token->tag = key_found->tag;
            strcpy(token->lexema, key_found->lexema);
            token->value = key_found->value;
            //o token sera retornado no final da funcao
        } else { //Identificador
            token->tag = ID;
            strcpy(token->lexema, buffer);
            token->value = 0;
        }
    }
    //Verifica se PLUS (+)
    else if (ch == PLUS) {
        token->tag = PLUS;
        strcpy(token->lexema, "+");
        token->value = 0;
    }
    //Verifica se MINUS (-)
    else if (ch == MINUS) {
        token->tag = MINUS;
        strcpy(token->lexema, "-");
        token->value = 0;
    }
    //Verifica se MULT (*)
    else if (ch == MULT) {
        token->tag = MULT;
        strcpy(token->lexema, "*");
        token->value = 0;
    }
    //Verifica se DIV (/)
    else if (ch == DIV) {
        token->tag = DIV;
        strcpy(token->lexema, "/");
        token->value = 0;
    }
    //Verifica se OPEN_PAR -> "("
    else if (ch == OPEN_PAR) {
        token->tag = OPEN_PAR;
        strcpy(token->lexema, "(");
        token->value = 0;
    }
    //Verifica se CLOSE_PAR -> ")"
    else if (ch == CLOSE_PAR) {
        token->tag = CLOSE_PAR;
        strcpy(token->lexema, ")");
        token->value = 0;
    }
    //Verifica se SEMICOLON -> ';'
    else if (ch == SEMICOLON) {
        token->tag = SEMICOLON;
        strcpy(token->lexema, ";");
        token->value = 0;
    }
    //Verifica se FIM DE ARQUIVO (EOF)
    else if (ch == EOF) {
        token->tag = ENDTOKEN;
        strcpy(token->lexema, "\0");
        token->value = 0;
    }
    //ERRO
    else {
        token->tag = ERROR;
        token->lexema[0] = (char) ch; //guarda o caractere invalido para a mensagem de erro
        token->lexema[1] = '\0';
        token->value = 0;
    }

    return token;
}

/**
 * @brief Busca na lista de palavras reservadas o parâmetro informado, retornando
 * o respectivo token, ou nulo caso nao exista.
 * 
 * @param char* palavra a ser verificada se corresponde a uma palavra reservada.
 * @return type_token* com o endereco de memoria do token correspondente, ou nulo.
 */
type_token *keyWordFind(char *word) {
    int i;
    i = 0;

    while( key_words[i].lexema[0] != '\0' ) {
        if ( strcmp(word, key_words[i].lexema) == 0 )
            return &key_words[i];
        i++;
    }
    return NULL;
}