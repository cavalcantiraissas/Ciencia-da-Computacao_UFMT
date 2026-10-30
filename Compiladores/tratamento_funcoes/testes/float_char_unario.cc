int a;
int b;
float f;
float g;
char c;

a = -7;
b = -a * 2 + -(3 - 5);
write b;
write a;
a = a / 2;
write a;

f = 2.5;
g = f * 2 + a;
write g;
g = -g / 4;
write g;
f = 10;
write f;
f = 1 / 3.0;
write f;

c = 'a';
c = c + 2;
write c;
if (c == 'c' && c > 'b') {
    write "char comparado como inteiro";
}
if (g < 0 && f > 0.3333 && f < 0.3334) {
    write "comparacao float";
}
if (a == -3.5 || a == -3) {
    write "int promovido para float";
}
