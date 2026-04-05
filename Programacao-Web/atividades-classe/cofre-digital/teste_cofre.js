meuCofre.depositar(100);
console.log("Saldo após depósito: " + meuCofre.consultarSaldo("senha-errada")); // tentativa 1
console.log("Saldo após depósito: " + meuCofre.consultarSaldo("senha-errada")); // tentativa 2
console.log("Saldo após depósito: " + meuCofre.consultarSaldo("senha-errada")); // tentativa 3 (bloqueia)
console.log("Saldo após depósito: " + meuCofre.consultarSaldo("1234"));         // já bloqueado

console.log("---");

console.log(meuCofre.sacar("1234", 50));

console.log("---");

console.log(meuCofre.alterarSenha("1234", "5678"));
console.log("Saldo com a nova senha: " + meuCofre.consultarSaldo("5678"));
