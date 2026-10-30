int idade;
float altura;
char letra;
string nome;
string copia;

write "Nome:";
read nome;
write "Idade:";
read idade;
write "Altura:";
read altura;
write "Letra:";
read letra;

copia = nome;
write copia;
write idade;
write altura;
write letra;

idade = idade + 1;
write "Idade no ano que vem:";
write idade;
if (idade >= 18 && altura > 1.5) {
    write "Adulto e alto\tOK";
}
