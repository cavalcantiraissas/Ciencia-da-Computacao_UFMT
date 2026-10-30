int troca(int p, int q);
int regressiva(int k);
int leDobra(int n);

begin
    p = 1;
    q = 2;
    troca(q + 0, p + 0);
    regressiva(3);
    leDobra(n);
    write n;
end

int troca(int p, int q) begin
    write p;
    write q;
end

int regressiva(int k) begin
    write k;
    if (k > 0) {
        regressiva(k - 1);
    } else {
        write "fim da recursao";
    }
end

int leDobra(int n) begin
    read n;
    n = n * 2;
end
