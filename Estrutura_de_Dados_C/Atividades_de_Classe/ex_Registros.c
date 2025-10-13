#include <stdio.h>
#include <stdlib.h>

#define TAM_VET 3
#define TAM_VENDA 2

// Definicao de tipo abstrado de dados
struct est_produto {
    int codigo;
    float preco_custo;
    float preco_venda;
};
typedef struct est_produto tipo_produto;

struct est_venda {
    tipo_produto produtos[TAM_VENDA];
    int qtd_produtos[TAM_VENDA];
    float sub_total[TAM_VENDA];
    float total;
};
typedef struct est_venda tipo_venda;

int main() {
    tipo_produto vet_produtos[TAM_VET];
    int i, j, cod, lucro;

    // Le os dados dos protudos
    for (i=0; i < TAM_VET; i++) {
        printf("Informe o codigo do produto %d:", i+1);
        scanf("%d", &vet_produtos[i].codigo);
        printf("Informe o preco de custo:");
        scanf("%f", &vet_produtos[i].preco_custo);
    }

    // Le o lucro
    printf("Informe o lucro:");
    scanf("%d", &lucro);

    // Calcula o preco de venda de cada produto
    for (i=0; i < TAM_VET; i++) {
        vet_produtos[i].preco_venda = vet_produtos[i].preco_custo + (vet_produtos[i].preco_custo * (float)lucro/100);
    }

    //Imprime os dados dos produtos
    for (i=0; i < TAM_VET; i++) {
        printf("Produto %d:\n\tCodigo: %d\n\tPreco de custo: %f\n\tPreco de venda: %f\n", 
                i+1, 
                vet_produtos[i].codigo,
                vet_produtos[i].preco_custo,
                vet_produtos[i].preco_venda);
    }

    //Simulacao de venda de produtos
    tipo_venda reg_venda;
    printf("Informe os produtos comprados:");
    for (i=0; i < TAM_VENDA; i++) {
        printf("Informe o código do produto %d:", i+1);
        scanf("%d", &cod);
        printf("\tInforme a quantidade:");
        scanf("%d", &reg_venda.qtd_produtos[i]);
        //Busca pelo produto para registra-lo na venda
        for (j=0; j<TAM_VET; j++) {
            if (vet_produtos[j].codigo == cod) {
                reg_venda.produtos[i] = vet_produtos[j];
            }
        }
        //Calcula sub total
        reg_venda.sub_total[i] = reg_venda.produtos[i].preco_venda * reg_venda.qtd_produtos[i];
    }
    // Calcula o total da venda realizada
    printf("Calculando total da venda...");
    reg_venda.total = 0;
    for (i=0; i < TAM_VENDA; i++)
        reg_venda.total += reg_venda.sub_total[i];

    //Imprime os dados da venda
    printf("RESUMO DA VENDA\n");
    for (i=0; i<TAM_VENDA; i++) {
        printf("Item %d - Código %d - Qtd %d - Sub total R$ %.2f\n",
                i+1,
                reg_venda.produtos[i].codigo,
                reg_venda.qtd_produtos[i],
                reg_venda.sub_total[i]);
    }
    printf("TOTAL = R$ %.2f\n", reg_venda.total);

    return 0;
}
