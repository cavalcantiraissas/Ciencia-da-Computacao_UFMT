int x;
int y;

begin
    x = 0;
    y = 5;

    if (x != 0 && 10 / x > 1) {
        write "nao deveria imprimir";
    } else {
        write "&& nao avaliou a divisao por zero";
    }
    if (x == 0 || 10 / x > 1) {
        write "|| nao avaliou a divisao por zero";
    }
    if (y > 1 && y < 10 && y != 7) {
        write "tres testes verdadeiros";
    }
    if (y < 1 || y > 10 || y == 5) {
        write "ultimo teste verdadeiro";
    }
    while (y > 0 && 100 / y >= 20) {
        y = y - 1;
    }
    write y;
    write "linha1\nlinha2 \"aspas\"";
    y = 10 / x;
    write "nao chega aqui";
end
