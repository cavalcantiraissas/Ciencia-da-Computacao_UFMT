import { inventarioLoja } from './dados.js';
import {
  exibirProduto,
  listarProdutosEmEstoque,
  calcularValorTotalInventario
} from './funcoes.js';

console.log("--- Executando o programa modular ---\n");

listarProdutosEmEstoque(inventarioLoja);

const valorTotal = calcularValorTotalInventario(inventarioLoja);
console.log("\n--- Valor Total do Inventário ---");
console.log(`O valor total é: R$ ${valorTotal.toFixed(2)}`);
