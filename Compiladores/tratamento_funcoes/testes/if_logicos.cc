int x;
int y;

begin
    x = 5;
    y = 10;
    if (x > 0 && y > 0) {
        write "ambos positivos";
    }
    if (x == 0 || y == 0) {
        write "algum eh zero";
    }
    if (x <= y) {
        write "id dos dois lados";
    }
    if (2 * x == y) {
        write "expressao a esquerda, id a direita";
    }
end
