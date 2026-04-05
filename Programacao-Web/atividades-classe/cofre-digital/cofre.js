const meuCofre = (function () {
    let senha = "1234";
    let saldo = 0;
    let tentativasFalhas = 0;
    let bloqueado = false;
  
    function verificarSenha(senhaFornecida) {
      if (bloqueado) {
        return { ok: false, mensagem: "Cofre bloqueado. Acesso negado." };
      }
  
      if (senhaFornecida === senha) {
        tentativasFalhas = 0;
        return { ok: true };
      } else {
        tentativasFalhas++;
        if (tentativasFalhas >= 3) {
          bloqueado = true;
          return { ok: false, mensagem: "Cofre bloqueado após 3 tentativas falhas." };
        }
        return { ok: false, mensagem: "Senha incorreta." };
      }
    }
  
    return {
      depositar(valor) {
        if (bloqueado) {
          return "Cofre bloqueado. Depósito não permitido.";
        }
        saldo += valor;
      },
  
      sacar(senhaFornecida, valor) {
        const resultado = verificarSenha(senhaFornecida);
        if (!resultado.ok) return resultado.mensagem;
  
        if (valor > saldo) {
          return "Saldo insuficiente.";
        }
  
        saldo -= valor;
        return `Saque de R$${valor} realizado com sucesso.`;
      },
  
      consultarSaldo(senhaFornecida) {
        const resultado = verificarSenha(senhaFornecida);
        if (!resultado.ok) return resultado.mensagem;
  
        return `Saldo atual: R$${saldo}`;
      },
  
      alterarSenha(senhaAntiga, senhaNova) {
        const resultado = verificarSenha(senhaAntiga);
        if (!resultado.ok) return resultado.mensagem;
  
        senha = senhaNova;
        return "Senha alterada com sucesso.";
      }
    };
  })();
  
