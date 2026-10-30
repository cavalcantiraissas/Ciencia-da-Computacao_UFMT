int total;
float media;
int mostra();
int soma(int a, int b);
int saudacao(string quem);
float metade(float v);
int conta(int limite);
int naoChamada(char c);

begin
    write "inicio do principal";
    mostra();
    soma(2, 3);
    write total;
    a = 10;
    soma(a, 4 * 5 - 1);
    write total;
    saudacao("UFMT");
    quem = "turma de Compiladores";
    saudacao(quem);
    metade(7);
    write media;
    metade(2.5 + 0.5);
    write media;
    conta(3);
    write "fim do principal";
end

int mostra() begin
    write "dentro de mostra";
end

int soma(int a, int b) {
    total = a + b;
}

int saudacao(string quem) begin
    write "ola,";
    write quem;
end

float metade(float v) begin
    media = v / 2;
end

int conta(int limite) begin
    int i;
    i = 1;
    while (i <= limite) {
        write i;
        if (i == 2) {
            mostra();
        }
        i = i + 1;
    }
end
