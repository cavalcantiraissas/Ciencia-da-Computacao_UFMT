/**
 * @file gen.c
 * @author Ivairton M. Santos - UFMT - Computacao
 * @brief Codificacao do modulo gerador de codigo
 * @version 0.3
 * @date 2022-02-23
 *
 * O codigo gerado eh o mesmo para todas as plataformas, exceto pelas rotinas
 * de apoio (runtime) de entrada/saida, pela entrada e pelo encerramento do
 * programa. A plataforma eh escolhida em tempo de compilacao do compilador:
 *   - Linux x86-64 (padrao): chamadas de sistema (syscall), sem libc;
 *   - macOS x86-64 (TARGET_MACOS, ver gen_macos.c): idem, com os numeros de
 *     chamada de sistema do macOS;
 *   - Windows x86-64 (TARGET_WIN, ver gen_win.c): printf/scanf da libc.
 */

// Inclusao do cabecalho
#include "gen.h"
#include <stdarg.h>
#include <stdint.h>

// Inclusao explicita de variaveis globais de outro contexto (symbols.h)
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
char output_file_name[FILENAME_MAX];
FILE *output_file;

/**
 * @brief Escreve uma linha de comando (instrucao, diretiva ou comentario) no
 * arquivo de saida, sempre com tabulacao. Apenas os labels (gen_label) ficam
 * sem tabulacao, para facilitar a leitura do codigo gerado.
 *
 * @param fmt formato no padrao printf (sem o '\n' final)
 */
static void emit(const char *fmt, ...) {
    va_list args;
    fprintf(output_file, "\t");
    va_start(args, fmt);
    vfprintf(output_file, fmt, args);
    va_end(args);
    fprintf(output_file, "\n");
}

/**
 * @brief Gera o codigo de uma operacao aritmetica float (SSE) entre os dois
 * valores do topo da pilha
 *
 * @param comment descricao da operacao
 * @param instruction instrucao SSE (addsd, subsd, mulsd ou divsd)
 */
static void gen_float_op(const char *comment, const char *instruction) {
    emit(";%s (float)", comment);
    emit("pop rbx");
    emit("pop rax");
    emit("movq xmm0, rax");
    emit("movq xmm1, rbx");
    emit("%s xmm0, xmm1", instruction);
    emit("movq rax, xmm0");
    emit("push rax");
}

/**
 * @brief Funcao que gera codigo de montagem para SOMA
 *
 * @param type tipo dos operandos (EXPR_INT ou EXPR_FLOAT)
 */
void genAdd(int type) {
    if (type == EXPR_FLOAT) {
        gen_float_op("Adicao", "addsd");
        return;
    }
    emit(";Adicao");
    emit("pop rbx");
    emit("pop rax");
    emit("add rax, rbx");
    emit("push rax");
}

/**
 * @brief Funcao que gera codigo de montagem para SUBTRACAO
 *
 * @param type tipo dos operandos (EXPR_INT ou EXPR_FLOAT)
 */
void genSub(int type) {
    if (type == EXPR_FLOAT) {
        gen_float_op("Subtracao", "subsd");
        return;
    }
    emit(";Subtracao");
    emit("pop rbx");
    emit("pop rax");
    emit("sub rax, rbx");
    emit("push rax");
}

/**
 * @brief Funcao que gera codigo de montagem para MULTIPLICACAO
 *
 * @param type tipo dos operandos (EXPR_INT ou EXPR_FLOAT)
 */
void genMult(int type) {
    if (type == EXPR_FLOAT) {
        gen_float_op("Multiplicacao", "mulsd");
        return;
    }
    emit(";Multiplicacao");
    emit("pop rbx");
    emit("pop rax");
    emit("imul rax, rbx");
    emit("push rax");
}

/**
 * @brief Funcao que gera codigo de montagem para DIVISAO. A divisao inteira
 * por zero encerra o programa com uma mensagem de erro.
 *
 * @param type tipo dos operandos (EXPR_INT ou EXPR_FLOAT)
 */
void genDiv(int type) {
    if (type == EXPR_FLOAT) {
        gen_float_op("Divisao", "divsd");
        return;
    }
    emit(";Divisao");
    emit("pop rbx");
    emit("pop rax");
    emit("test rbx, rbx");
    emit("jz _rt_div_zero");
    emit("cqo");      //estende o sinal de rax para rdx (dividendo = rdx:rax)
    emit("idiv rbx"); //quociente em rax
    emit("push rax");
}

/**
 * @brief Funcao que gera codigo para inverter o sinal do valor do topo da
 * pilha (menos unario)
 *
 * @param type tipo do operando (EXPR_INT ou EXPR_FLOAT)
 */
void genNeg(int type) {
    emit(";Menos unario");
    emit("pop rax");
    if (type == EXPR_FLOAT)
        emit("btc rax, 63"); //inverte o bit de sinal do double
    else
        emit("neg rax");
    emit("push rax");
}

/**
 * @brief Funcao que gera codigo para converter um valor inteiro da pilha em
 * float (precisao dupla)
 *
 * @param depth posicao do valor: 0 = topo da pilha, 1 = logo abaixo do topo
 */
void genIntToFloat(int depth) {
    emit(";Converte int para float");
    if (depth == 0) {
        emit("pop rax");
        emit("cvtsi2sd xmm0, rax");
        emit("movq rax, xmm0");
        emit("push rax");
    } else {
        emit("pop rbx");
        emit("pop rax");
        emit("cvtsi2sd xmm0, rax");
        emit("movq rax, xmm0");
        emit("push rax");
        emit("push rbx");
    }
}

/**
 * @brief Funcao que gera codigo de montagem para armazenamento de NUMERAL
 *
 * @param num_string
 */
void genNum(char num_string[MAX_TOKEN]) {
    emit(";Armazenamento de numero");
    emit("mov rax, %s", num_string);
    emit("push rax");
}

/**
 * @brief Funcao que gera codigo de montagem para empilhar uma constante float.
 * O valor eh convertido para o padrao de bits IEEE 754 de precisao dupla.
 *
 * @param num_string lexema do numero (ex.: 3.14 ou 10)
 */
void genFloatNum(char num_string[MAX_TOKEN]) {
    double value;
    uint64_t bits;
    value = strtod(num_string, NULL);
    memcpy(&bits, &value, sizeof(bits));
    emit(";Armazenamento de numero float (%s)", num_string);
    emit("mov rax, 0x%016llX", (unsigned long long) bits);
    emit("push rax");
}

/**
 * @brief Funcao que gera codigo para empilhar na memoria o CONTEUDO de uma
 * variavel (e nao apenas o seu endereco).
 *
 * @param lexeme_of_id nome da variavel (registrada na tabela de simbolos)
 * @param type tipo da variavel (INT, FLOAT ou CHAR)
 */
void gen_push_var(char *lexeme_of_id, int type) {
    emit(";Empilha o conteudo da variavel %s", lexeme_of_id);
    if (type == CHAR) {
        emit("movzx rax, byte [%s]", lexeme_of_id); //char ocupa 1 byte
        emit("push rax");
    } else {
        emit("push qword [%s]", lexeme_of_id);
    }
}

/**
 * @brief Funcao que gera codigo de ATRIBUICAO: desempilha o valor calculado
 * (expressao aritmetica ou constante) e o armazena na variavel.
 *
 * @param lexeme_of_id nome da variavel (declarada e registrada na TS)
 * @param type tipo da variavel (INT, FLOAT ou CHAR)
 */
void gen_assign(char *lexeme_of_id, int type) {
    emit(";Atribuicao a variavel %s", lexeme_of_id);
    emit("pop rax");          //traz da memoria o valor calculado
    if (type == CHAR)
        emit("mov [%s], al", lexeme_of_id);  //char ocupa 1 byte
    else
        emit("mov [%s], rax", lexeme_of_id); //int e float ocupam 8 bytes
}

/**
 * @brief Funcao que gera codigo de ATRIBUICAO a uma variavel string: copia a
 * string de origem (constante ou outra variavel) ate o '\0'.
 *
 * @param lexeme_of_id nome da variavel string de destino
 * @param source rotulo da origem (constante strN ou variavel string)
 */
void gen_assign_string(char *lexeme_of_id, char *source) {
    emit(";Atribuicao da string %s a variavel %s", source, lexeme_of_id);
    emit("lea rsi, [%s]", source);
    emit("lea rdi, [%s]", lexeme_of_id);
    emit("call _rt_strcpy");
}

/**
 * @brief Funcao que gera um preambulo com as instrucoes de montagem e as
 * definicoes da plataforma
 *
 */
void gen_preambule(void) {
    fprintf(output_file, ";UFMT-Compiladores\n");
    fprintf(output_file, ";Prof. Ivairton\n");
#if defined(TARGET_WIN)
    fprintf(output_file, ";Procedimento para geracao do executavel apos compilacao (em Windows):\n");
    fprintf(output_file, ";(1) compilacao do Assembly com nasm: $ nasm -f win64 <nome_do_arquivo>\n");
    fprintf(output_file, ";(2) ligacao com a libc: $ gcc <nome_arquivo_objeto> -o programa.exe\n\n");
    emit("default rel");
    emit("extern printf");
    emit("extern scanf");
    emit("extern exit");
#elif defined(TARGET_MACOS)
    fprintf(output_file, ";Procedimento para geracao do executavel apos compilacao (em macOS x86-64):\n");
    fprintf(output_file, ";(1) compilacao do Assembly com nasm: $ nasm -f macho64 <nome_do_arquivo>\n");
    fprintf(output_file, ";(2) ligacao: $ clang -arch x86_64 -nostartfiles -e _start <nome_arquivo_objeto> -o programa\n\n");
    emit("default rel");
    emit("%%define SYS_READ 0x2000003");
    emit("%%define SYS_WRITE 0x2000004");
    emit("%%define SYS_EXIT 0x2000001");
#else
    fprintf(output_file, ";Procedimento para geracao do executavel apos compilacao (em Linux):\n");
    fprintf(output_file, ";(1) compilacao do Assembly com nasm: $ nasm -f elf64 <nome_do_arquivo>\n");
    fprintf(output_file, ";(2) likedicao: $ ld -m elf_x86_64 <nome_arquivo_objeto>\n\n");
    emit("default rel");
    emit("%%define SYS_READ 0");
    emit("%%define SYS_WRITE 1");
    emit("%%define SYS_EXIT 60");
#endif
    emit("%%define _RT_MAXSTR %d", MAX_CHAR - 1); //tamanho maximo de uma string (sem o '\0')
}

/**
 * @brief Escreve no arquivo de saida uma constante string no formato do NASM.
 * Usa crases (`...`), que interpretam as sequencias de escape (\n, \t, ...).
 *
 * @param value lexema da constante, com as aspas duplas
 */
static void gen_string_literal(char *value) {
    int i, len;
    len = (int) strlen(value);
    fprintf(output_file, "`");
    for (i = 1; i < len - 1; i++) { //ignora as aspas duplas
        if (value[i] == '`')
            fprintf(output_file, "\\`");
        else
            fputc(value[i], output_file);
    }
    fprintf(output_file, "`");
}

/**
 * @brief Funcao que gera codigo da secao de dados (declaracao de variaveis).
 * Cada variavel recebe espaco do tamanho do seu tipo, iniciado com zero.
 */
void gen_data_section(void) {
    int i, n;

    fprintf(output_file, "\n");
    emit("section .data");

    // processa cada simbolo da tabela e reserva memoria para cada variavel
    n = global_symbol_table_variables.n_variables;
    for (i = 0; i < n; i++) {
       char *name = global_symbol_table_variables.variable[i].name;
       switch(global_symbol_table_variables.variable[i].type) {
            case INT:
                fprintf(output_file, "%s:\tdq 0\n", name);   //8 bytes (rax)
                break;
            case FLOAT:
                fprintf(output_file, "%s:\tdq 0.0\n", name); //8 bytes (double)
                break;
            case CHAR:
                fprintf(output_file, "%s:\tdb 0\n", name);
                break;
            case STRING:
                fprintf(output_file, "%s:\ttimes %d db 0\n", name, MAX_CHAR);
                break;
            default:
                printf("[ERRO] Tipo desconhecido na variavel '%s'.\n", name);
                break;
       }
    }

    // constantes string (terminadas em '\0')
    n = symbol_table_string.n_strings;
    for (i = 0; i < n; i++) {
        fprintf(output_file, "%s:\tdb ", symbol_table_string.string[i].name);
        gen_string_literal(symbol_table_string.string[i].value);
        fprintf(output_file, ", 0\n");
    }

    // dados usados pelas rotinas de apoio (runtime)
#if defined(TARGET_WIN)
    fprintf(output_file, "_rt_fmt_int:\tdb `%%lld`, 0\n");
    fprintf(output_file, "_rt_fmt_float:\tdb `%%f`, 0\n");
    fprintf(output_file, "_rt_fmt_str:\tdb `%%s`, 0\n");
    fprintf(output_file, "_rt_fmt_bytes:\tdb `%%.*s`, 0\n");
    fprintf(output_file, "_rt_fmt_nl:\tdb `\\n`, 0\n");
    fprintf(output_file, "_rt_fmt_rint:\tdb ` %%lld`, 0\n");
    fprintf(output_file, "_rt_fmt_rfloat:\tdb ` %%lf`, 0\n");
    fprintf(output_file, "_rt_fmt_rchar:\tdb ` %%c`, 0\n");
    fprintf(output_file, "_rt_fmt_rstr:\tdb ` %%%d[^\\n]`, 0\n", MAX_CHAR - 1);
#else
    fprintf(output_file, "_rt_cbuf:\tdb 0\n");
    fprintf(output_file, "_rt_nbuf:\ttimes 32 db 0\n");
    fprintf(output_file, "_rt_nl:\tdb 10\n");
    fprintf(output_file, "_rt_minus:\tdb '-'\n");
    fprintf(output_file, "_rt_dot:\tdb '.'\n");
#endif
    fprintf(output_file, "_rt_msg_div:\tdb `[ERRO] Divisao por zero\\n`, 0\n");
}

/**
 * @brief Funcao que gera a marcacao do inicio da secao de codigo
 *
 */
void gen_preambule_code(void) {
    fprintf(output_file, "\n");
    emit("section .text");
#if defined(TARGET_WIN)
    emit("global main");
    gen_label("main");
#else
    emit("global main,_start");
    gen_label("main");
    gen_label("_start");
#endif
}

/**
 * @brief Funcao que encerra o codigo inserindo comandos de fechamento
 *
 */
void gen_epilog_code(void) {
    fprintf(output_file, "\n");
    emit(";encerra programa");
#if defined(TARGET_WIN)
    emit("and rsp, -16");
    emit("sub rsp, 32");
    emit("xor ecx, ecx");
    emit("call exit");
#else
    emit("mov rax, SYS_EXIT");
    emit("xor rdi, rdi");
    emit("syscall");
#endif
}

/**
 * @brief Funcao que gera automaticamente um novo nome para um label
 * (usado pelos comandos if/else e while)
 * @param string name
 */
void gen_label_name( char *name ) {
    static int nlabels=0;
    sprintf(name, "label%d", nlabels);
    nlabels++;
}

/**
 * @brief Funcao que registra no codigo um label (sem tabulacao)
 * @param string label
 */
void gen_label( char *label ) {
    fprintf(output_file, "%s:\n", label);
}

/**
 * @brief Funcao que gera um jump condicional: desvia para o label se o valor
 * no topo da pilha for falso (zero)
 * @param string label
 */
void gen_cond_jump(char *label) {
    emit(";jump condicional");
    emit("pop rax");
    emit("cmp rax, 0");
    emit("jz %s", label);
}

/**
 * @brief Funcao que gera um jump incondicional
 * @param string label
 */
void gen_incond_jump(char *label) {
    emit(";jump incondicional");
    emit("jmp %s", label);
}

/**
 * @brief Funcao que gera codigo a partir do processamento da expressao
 * relacional: compara os dois valores do topo da pilha e empilha 1
 * (verdadeiro) ou 0 (falso)
 *
 * @param oper operador relacional (EQUAL, NE, LT, GT, LE ou GE)
 * @param type tipo dos operandos (EXPR_INT ou EXPR_FLOAT)
 */
void gen_bool(int oper, int type) {
    char bool_label_name[MAX_CHAR];
    const char *jump;
    int is_float = (type == EXPR_FLOAT);

    //Para float (ucomisd) os desvios sao os de comparacao sem sinal
    switch (oper) {
        case EQUAL: jump = "je";  break;
        case NE:    jump = "jne"; break;
        case LT:    jump = is_float ? "jb"  : "jl";  break;
        case GT:    jump = is_float ? "ja"  : "jg";  break;
        case LE:    jump = is_float ? "jbe" : "jle"; break;
        case GE:    jump = is_float ? "jae" : "jge"; break;
        default:
            printf("[ERRO] operador booleano nao suportado.\n");
            return;
    }

    gen_bool_label_name(bool_label_name);
    emit(";Aplica operador booleano/exp.logica%s", is_float ? " (float)" : "");
    emit("pop rbx");
    emit("pop rax");
    emit("mov rcx, 1");
    if (is_float) {
        emit("movq xmm0, rax");
        emit("movq xmm1, rbx");
        emit("ucomisd xmm0, xmm1");
    } else {
        emit("cmp rax, rbx");
    }
    emit("%s %s", jump, bool_label_name);
    emit("mov rcx, 0");
    gen_label(bool_label_name); //Imprime no codigo o label
    emit("mov rax, rcx");
    emit("push rax");
}

/**
 * @brief Funcao que gera automaticamente o nome de um label da expressao
 * booleana, com o prefixo "label_bool"
 * @param name buffer que recebe o nome gerado
 */
void gen_bool_label_name(char *name) {
    static int nbool_labels = 0;
    sprintf(name, "label_bool%d", nbool_labels);
    nbool_labels++;
}

/**
 * @brief Funcao que gera o inicio de um E (&&) ou OU (||) logico com
 * curto-circuito. Deve ser chamada apos o codigo do operando da esquerda e
 * antes do codigo do operando da direita:
 *   - &&: se o operando da esquerda for falso, o resultado eh 0 e o da direita
 *         nao eh avaliado;
 *   - ||: se o operando da esquerda for verdadeiro, o resultado eh 1 e o da
 *         direita nao eh avaliado.
 * Caso contrario, o resultado eh o valor do operando da direita. Apos o codigo
 * do operando da direita, deve-se registrar o label 'label_end' (gen_label).
 *
 * @param oper AND ou OR
 * @param label_end buffer que recebe o nome do label de fim
 */
void gen_logic_begin(int oper, char *label_end) {
    char label_eval[MAX_CHAR];
    gen_bool_label_name(label_eval);
    gen_bool_label_name(label_end);
    emit(oper == AND ? ";E logico (&&) com curto-circuito" : ";OU logico (||) com curto-circuito");
    emit("pop rax");
    emit("cmp rax, 0");
    emit("%s %s", oper == AND ? "jne" : "je", label_eval);
    emit("push rax"); //resultado ja decidido: 0 para &&, 1 para ||
    emit("jmp %s", label_end);
    gen_label(label_eval); //avalia o operando da direita, que sera o resultado
}

/**
 * @brief Gera codigo para o comando READ: le uma linha da entrada padrao e
 * converte o texto para o tipo da variavel
 *
 * @param lexeme_of_id nome do identificador
 * @param type tipo da variavel
 */
void gen_read(char *lexeme_of_id, int type) {
    fprintf(output_file, "\n");
    switch (type) {
        case INT:
            emit(";le valor inteiro");
            emit("call _rt_read_int");
            emit("mov [%s], rax", lexeme_of_id);
            break;
        case FLOAT:
            emit(";le valor float");
            emit("call _rt_read_float");
            emit("movq [%s], xmm0", lexeme_of_id);
            break;
        case CHAR:
            emit(";le valor char");
            emit("call _rt_read_char");
            emit("mov [%s], al", lexeme_of_id);
            break;
        case STRING:
            emit(";le valor string");
            emit("lea rdi, [%s]", lexeme_of_id);
            emit("call _rt_read_str");
            break;
    }
}

/**
 * @brief Gera codigo para o comando WRITE: converte o valor da variavel (ou
 * a constante string) para texto e o escreve, seguido de quebra de linha
 *
 * @param lexeme_of_id nome do identificador (ou rotulo da constante string)
 * @param type tipo da variavel
 */
void gen_write(char *lexeme_of_id, int type) {
    fprintf(output_file, "\n");
    switch (type) {
        case INT:
            emit(";escreve valor inteiro");
            emit("mov rax, [%s]", lexeme_of_id);
            emit("call _rt_write_int");
            break;
        case FLOAT:
            emit(";escreve valor float");
            emit("movq xmm0, [%s]", lexeme_of_id);
            emit("call _rt_write_float");
            break;
        case CHAR:
            emit(";escreve valor char");
            emit("lea rsi, [%s]", lexeme_of_id);
            emit("mov rdx, 1");
            emit("call _rt_write");
            break;
        case STRING:
            emit(";escreve valor string");
            emit("lea rsi, [%s]", lexeme_of_id);
            emit("call _rt_write_str");
            break;
    }
    emit("call _rt_newline");
}

//------------------------- ROTINAS DE APOIO (RUNTIME) -------------------------
/*
 * Convencao das rotinas: inteiro em rax, float em xmm0, endereco de origem em
 * rsi, tamanho em rdx e endereco de destino em rdi. O retorno fica em rax
 * (int/char) ou xmm0 (float). As rotinas podem alterar rax, rbx, rcx, rdx,
 * rsi, rdi, r8-r14 e xmm0-xmm2, que nao guardam valores entre os comandos.
 */

//Rotina comum a todas as plataformas: copia string ate o '\0' (no maximo _RT_MAXSTR bytes)
static const char *runtime_common =
";_rt_strcpy: copia a string de rsi para rdi\n"
"_rt_strcpy:\n"
"\txor rcx, rcx\n"
".loop:\n"
"\tcmp rcx, _RT_MAXSTR\n"
"\tjge .limit\n"
"\tmov al, [rsi+rcx]\n"
"\tmov [rdi+rcx], al\n"
"\ttest al, al\n"
"\tjz .done\n"
"\tinc rcx\n"
"\tjmp .loop\n"
".limit:\n"
"\tmov byte [rdi+rcx], 0\n"
".done:\n"
"\tret\n";

#if defined(TARGET_WIN)
//Windows: entrada e saida com printf/scanf (convencao de chamada x64 da Microsoft:
//argumentos em rcx, rdx, r8, r9, 32 bytes de "shadow space" e pilha alinhada em 16)
static const char *runtime_platform =
";_rt_write: escreve rdx bytes a partir de rsi\n"
"_rt_write:\n"
"\tpush rbp\n"
"\tmov rbp, rsp\n"
"\tand rsp, -16\n"
"\tsub rsp, 32\n"
"\tmov r8, rsi\n"
"\tlea rcx, [_rt_fmt_bytes]\n"
"\tcall printf\n"
"\tmov rsp, rbp\n"
"\tpop rbp\n"
"\tret\n"
";_rt_write_str: escreve a string terminada em \\0 apontada por rsi\n"
"_rt_write_str:\n"
"\tpush rbp\n"
"\tmov rbp, rsp\n"
"\tand rsp, -16\n"
"\tsub rsp, 32\n"
"\tmov rdx, rsi\n"
"\tlea rcx, [_rt_fmt_str]\n"
"\tcall printf\n"
"\tmov rsp, rbp\n"
"\tpop rbp\n"
"\tret\n"
";_rt_newline: escreve uma quebra de linha\n"
"_rt_newline:\n"
"\tpush rbp\n"
"\tmov rbp, rsp\n"
"\tand rsp, -16\n"
"\tsub rsp, 32\n"
"\tlea rcx, [_rt_fmt_nl]\n"
"\tcall printf\n"
"\tmov rsp, rbp\n"
"\tpop rbp\n"
"\tret\n"
";_rt_write_int: escreve o inteiro de rax em decimal\n"
"_rt_write_int:\n"
"\tpush rbp\n"
"\tmov rbp, rsp\n"
"\tand rsp, -16\n"
"\tsub rsp, 32\n"
"\tmov rdx, rax\n"
"\tlea rcx, [_rt_fmt_int]\n"
"\tcall printf\n"
"\tmov rsp, rbp\n"
"\tpop rbp\n"
"\tret\n"
";_rt_write_float: escreve o float de xmm0 com 6 casas decimais\n"
"_rt_write_float:\n"
"\tpush rbp\n"
"\tmov rbp, rsp\n"
"\tand rsp, -16\n"
"\tsub rsp, 32\n"
"\tmovq rdx, xmm0\n"
"\tmovapd xmm1, xmm0\n"
"\tlea rcx, [_rt_fmt_float]\n"
"\tcall printf\n"
"\tmov rsp, rbp\n"
"\tpop rbp\n"
"\tret\n"
";_rt_read_int: le um inteiro -> rax\n"
"_rt_read_int:\n"
"\tpush rbp\n"
"\tmov rbp, rsp\n"
"\tand rsp, -16\n"
"\tsub rsp, 48\n"
"\tmov qword [rsp+32], 0\n"
"\tlea rdx, [rsp+32]\n"
"\tlea rcx, [_rt_fmt_rint]\n"
"\tcall scanf\n"
"\tmov rax, [rsp+32]\n"
"\tmov rsp, rbp\n"
"\tpop rbp\n"
"\tret\n"
";_rt_read_float: le um float -> xmm0\n"
"_rt_read_float:\n"
"\tpush rbp\n"
"\tmov rbp, rsp\n"
"\tand rsp, -16\n"
"\tsub rsp, 48\n"
"\tmov qword [rsp+32], 0\n"
"\tlea rdx, [rsp+32]\n"
"\tlea rcx, [_rt_fmt_rfloat]\n"
"\tcall scanf\n"
"\tmovq xmm0, [rsp+32]\n"
"\tmov rsp, rbp\n"
"\tpop rbp\n"
"\tret\n"
";_rt_read_char: le um caractere -> rax\n"
"_rt_read_char:\n"
"\tpush rbp\n"
"\tmov rbp, rsp\n"
"\tand rsp, -16\n"
"\tsub rsp, 48\n"
"\tmov qword [rsp+32], 0\n"
"\tlea rdx, [rsp+32]\n"
"\tlea rcx, [_rt_fmt_rchar]\n"
"\tcall scanf\n"
"\tmovzx rax, byte [rsp+32]\n"
"\tmov rsp, rbp\n"
"\tpop rbp\n"
"\tret\n"
";_rt_read_str: le uma linha (sem a quebra de linha) para o endereco rdi\n"
"_rt_read_str:\n"
"\tpush rbp\n"
"\tmov rbp, rsp\n"
"\tand rsp, -16\n"
"\tsub rsp, 32\n"
"\tmov byte [rdi], 0\n"
"\tmov rdx, rdi\n"
"\tlea rcx, [_rt_fmt_rstr]\n"
"\tcall scanf\n"
"\tmov rsp, rbp\n"
"\tpop rbp\n"
"\tret\n"
";_rt_div_zero: encerra o programa por divisao inteira por zero\n"
"_rt_div_zero:\n"
"\tlea rsi, [_rt_msg_div]\n"
"\tcall _rt_write_str\n"
"\tand rsp, -16\n"
"\tsub rsp, 32\n"
"\tmov ecx, 1\n"
"\tcall exit\n";
#else
//Linux e macOS: entrada e saida com chamadas de sistema (read/write), sem libc
static const char *runtime_platform =
";_rt_getc: le um caractere da entrada padrao -> rax (-1 no fim de arquivo)\n"
"_rt_getc:\n"
"\tmov rax, SYS_READ\n"
"\tmov rdi, 0\n"
"\tlea rsi, [_rt_cbuf]\n"
"\tmov rdx, 1\n"
"\tsyscall\n"
"\tcmp rax, 1\n"
"\tjne .eof\n"
"\tmovzx rax, byte [_rt_cbuf]\n"
"\tret\n"
".eof:\n"
"\tmov rax, -1\n"
"\tret\n"
";_rt_skip_line: descarta o restante da linha (rax = ultimo caractere lido)\n"
"_rt_skip_line:\n"
"\tcmp rax, 10\n"
"\tje .done\n"
"\tcmp rax, -1\n"
"\tje .done\n"
"\tcall _rt_getc\n"
"\tjmp _rt_skip_line\n"
".done:\n"
"\tret\n"
";_rt_read_sign: ignora espacos e le o sinal -> r13 = 1 ou -1, rax = proximo caractere\n"
"_rt_read_sign:\n"
"\tmov r13, 1\n"
".skip:\n"
"\tcall _rt_getc\n"
"\tcmp rax, ' '\n"
"\tje .skip\n"
"\tcmp rax, 9\n"
"\tje .skip\n"
"\tcmp rax, 10\n"
"\tje .skip\n"
"\tcmp rax, 13\n"
"\tje .skip\n"
"\tcmp rax, '-'\n"
"\tjne .plus\n"
"\tmov r13, -1\n"
"\tjmp _rt_getc\n"
".plus:\n"
"\tcmp rax, '+'\n"
"\tjne .done\n"
"\tjmp _rt_getc\n"
".done:\n"
"\tret\n"
";_rt_read_int: le uma linha e converte o texto para inteiro -> rax\n"
"_rt_read_int:\n"
"\txor r12, r12\n"
"\tcall _rt_read_sign\n"
".digits:\n"
"\tcmp rax, '0'\n"
"\tjl .end\n"
"\tcmp rax, '9'\n"
"\tjg .end\n"
"\timul r12, r12, 10\n"
"\tsub rax, '0'\n"
"\tadd r12, rax\n"
"\tcall _rt_getc\n"
"\tjmp .digits\n"
".end:\n"
"\tcall _rt_skip_line\n"
"\tmov rax, r12\n"
"\timul rax, r13\n"
"\tret\n"
";_rt_read_float: le uma linha e converte o texto para float -> xmm0\n"
"_rt_read_float:\n"
"\txor r12, r12\n"            //todos os digitos, como inteiro
"\txor r14, r14\n"            //quantidade de casas decimais
"\tcall _rt_read_sign\n"
".digits:\n"
"\tcmp rax, '0'\n"
"\tjl .point\n"
"\tcmp rax, '9'\n"
"\tjg .point\n"
"\timul r12, r12, 10\n"
"\tsub rax, '0'\n"
"\tadd r12, rax\n"
"\tcall _rt_getc\n"
"\tjmp .digits\n"
".point:\n"
"\tcmp rax, '.'\n"
"\tjne .end\n"
"\tcall _rt_getc\n"
".frac:\n"
"\tcmp rax, '0'\n"
"\tjl .end\n"
"\tcmp rax, '9'\n"
"\tjg .end\n"
"\timul r12, r12, 10\n"
"\tsub rax, '0'\n"
"\tadd r12, rax\n"
"\tinc r14\n"
"\tcall _rt_getc\n"
"\tjmp .frac\n"
".end:\n"
"\tcall _rt_skip_line\n"
"\timul r12, r13\n"
"\tcvtsi2sd xmm0, r12\n"
"\tmov rax, 10\n"
"\tcvtsi2sd xmm1, rax\n"
"\tmov rax, 1\n"
"\tcvtsi2sd xmm2, rax\n"     //xmm2 = 10^(casas decimais)
".scale:\n"
"\ttest r14, r14\n"
"\tjz .divide\n"
"\tmulsd xmm2, xmm1\n"
"\tdec r14\n"
"\tjmp .scale\n"
".divide:\n"
"\tdivsd xmm0, xmm2\n"
"\tret\n"
";_rt_read_char: le uma linha e devolve o primeiro caractere -> rax\n"
"_rt_read_char:\n"
"\tcall _rt_getc\n"
"\tcmp rax, 10\n"
"\tje _rt_read_char\n"
"\tcmp rax, 13\n"
"\tje _rt_read_char\n"
"\tcmp rax, -1\n"
"\tje .eof\n"
"\tmov r12, rax\n"
"\tcall _rt_skip_line\n"
"\tmov rax, r12\n"
"\tret\n"
".eof:\n"
"\txor rax, rax\n"
"\tret\n"
";_rt_read_str: le uma linha (sem a quebra de linha) para o endereco rdi\n"
"_rt_read_str:\n"
"\tmov r12, rdi\n"
"\txor r13, r13\n"
".loop:\n"
"\tcall _rt_getc\n"
"\tcmp rax, 10\n"
"\tje .end\n"
"\tcmp rax, -1\n"
"\tje .end\n"
"\tcmp rax, 13\n"
"\tje .loop\n"
"\tcmp r13, _RT_MAXSTR\n"
"\tjge .loop\n"              //descarta o que nao cabe na variavel
"\tmov [r12+r13], al\n"
"\tinc r13\n"
"\tjmp .loop\n"
".end:\n"
"\tmov byte [r12+r13], 0\n"
"\tret\n"
";_rt_write: escreve rdx bytes a partir de rsi na saida padrao\n"
"_rt_write:\n"
"\tmov rax, SYS_WRITE\n"
"\tmov rdi, 1\n"
"\tsyscall\n"
"\tret\n"
";_rt_newline: escreve uma quebra de linha\n"
"_rt_newline:\n"
"\tlea rsi, [_rt_nl]\n"
"\tmov rdx, 1\n"
"\tjmp _rt_write\n"
";_rt_write_str: escreve a string terminada em \\0 apontada por rsi\n"
"_rt_write_str:\n"
"\txor rdx, rdx\n"
".len:\n"
"\tcmp byte [rsi+rdx], 0\n"
"\tje _rt_write\n"
"\tinc rdx\n"
"\tjmp .len\n"
";_rt_write_int: escreve o inteiro de rax em decimal\n"
"_rt_write_int:\n"
"\tlea rdi, [_rt_nbuf+32]\n"
"\txor r8, r8\n"
"\tcmp rax, 0\n"
"\tjge .digits\n"
"\tneg rax\n"
"\tmov r8, 1\n"
".digits:\n"
"\tmov rbx, 10\n"
".loop:\n"
"\txor rdx, rdx\n"
"\tdiv rbx\n"
"\tadd dl, '0'\n"
"\tdec rdi\n"
"\tmov [rdi], dl\n"
"\ttest rax, rax\n"
"\tjnz .loop\n"
"\ttest r8, r8\n"
"\tjz .print\n"
"\tdec rdi\n"
"\tmov byte [rdi], '-'\n"
".print:\n"
"\tlea rdx, [_rt_nbuf+32]\n"
"\tsub rdx, rdi\n"
"\tmov rsi, rdi\n"
"\tjmp _rt_write\n"
";_rt_write_float: escreve o float de xmm0 com 6 casas decimais (arredondado)\n"
"_rt_write_float:\n"
"\tmov rax, 1000000\n"
"\tcvtsi2sd xmm1, rax\n"
"\tmulsd xmm0, xmm1\n"
"\tcvtsd2si rax, xmm0\n"      //arredonda para o inteiro mais proximo
"\tcmp rax, 0\n"
"\tjge .positive\n"
"\tneg rax\n"
"\tpush rax\n"
"\tlea rsi, [_rt_minus]\n"
"\tmov rdx, 1\n"
"\tcall _rt_write\n"
"\tpop rax\n"
".positive:\n"
"\txor rdx, rdx\n"
"\tmov rbx, 1000000\n"
"\tdiv rbx\n"                 //rax = parte inteira, rdx = parte fracionaria
"\tpush rdx\n"
"\tcall _rt_write_int\n"
"\tlea rsi, [_rt_dot]\n"
"\tmov rdx, 1\n"
"\tcall _rt_write\n"
"\tpop rax\n"
"\tlea rdi, [_rt_nbuf+6]\n"
"\tmov rcx, 6\n"
"\tmov rbx, 10\n"
".frac:\n"
"\txor rdx, rdx\n"
"\tdiv rbx\n"
"\tadd dl, '0'\n"
"\tdec rdi\n"
"\tmov [rdi], dl\n"
"\tdec rcx\n"
"\tjnz .frac\n"
"\tlea rsi, [_rt_nbuf]\n"
"\tmov rdx, 6\n"
"\tjmp _rt_write\n"
";_rt_div_zero: encerra o programa por divisao inteira por zero\n"
"_rt_div_zero:\n"
"\tlea rsi, [_rt_msg_div]\n"
"\tcall _rt_write_str\n"
"\tmov rax, SYS_EXIT\n"
"\tmov rdi, 1\n"
"\tsyscall\n";
#endif

/**
 * @brief Funcao que gera as rotinas de apoio (runtime) usadas pelo codigo
 * gerado: conversao entre texto e numero na entrada/saida, copia de strings e
 * tratamento de divisao por zero
 */
void gen_runtime(void) {
    fprintf(output_file, "\n");
    emit(";---------------- rotinas de apoio (runtime) ----------------");
    fputs(runtime_platform, output_file);
    fputs(runtime_common, output_file);
}
