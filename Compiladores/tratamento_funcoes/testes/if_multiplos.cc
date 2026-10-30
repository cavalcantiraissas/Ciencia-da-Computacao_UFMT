int x;
int y;
int z;
x = 5;
y = 10;
z = x + y * 2;
if (x < y && y < z || z == 0) {
    write "ordenados ou z zero";
}
if (x + 1 >= y - 4 && (z / 5 != 1 || x * 2 <= 10)) {
    write "expressoes nos dois lados";
}
if (10 < x || (x == y && y != z) || 3 * (x - 1) > z) {
    write "tres testes com OU";
} else {
    write "nenhum teste verdadeiro";
}
if ((x > 1) && (y > 2) && (z > 3) && (x < y)) {
    write "quatro testes com E";
}
