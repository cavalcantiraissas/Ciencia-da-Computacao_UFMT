int i;
int soma;
string msg;
char c;
float f;
i = 0;
soma = 0;
msg = "fim";
c = 'k';
f = 3.14;
while (i < 10) {
    if (i / 2 * 2 == i) {
        if (i != 4 && i != 6) {
            soma = soma + i;
        } else {
            soma = soma - 1;
        }
    }
    i = i + 1;
}
if (soma > 0) {
}
write soma;
write msg;
write c;
write f;
