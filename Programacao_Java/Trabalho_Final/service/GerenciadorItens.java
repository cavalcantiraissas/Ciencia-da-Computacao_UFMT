package service;

import exceptions.CampoVazioException;
import exceptions.DuplicadoException;
import java.io.*;
import java.time.LocalDate;
import java.util.*;
import java.util.stream.Collectors;
import model.Filme;
import model.Item;
import model.Livro;

/**
 * Classe responsável por gerenciar a coleção de itens
 * Fornece operações CRUD, busca, ordenação e persistência
 */

public class GerenciadorItens {
    private List<Item> itens;
    
    public GerenciadorItens() {
        this.itens = new ArrayList<>();
    }
    
    /**
     * Adiciona um novo item à lista
     * @param item Item a ser adicionado
     * @throws CampoVazioException Se campos obrigatórios estiverem vazios
     * @throws DuplicadoException Se já existir item com mesmo título
     */

    public void adicionarItem(Item item) throws CampoVazioException, DuplicadoException {
        // Validações de campos obrigatórios
        if (item.getTitulo() == null || item.getTitulo().trim().isEmpty()) {
            throw new CampoVazioException("título");
        }
        if (item.getDescricao() == null || item.getDescricao().trim().isEmpty()) {
            throw new CampoVazioException("descrição");
        }
        
        // Verifica duplicação por título (case insensitive)
        for (Item i : itens) {
            if (i.getTitulo().equalsIgnoreCase(item.getTitulo())) {
                throw new DuplicadoException(item.getTitulo());
            }
        }
        
        itens.add(item);
    }
    
    /**
     * Retorna uma cópia da lista de itens
     * @return Lista de itens
     */
    public List<Item> listarTodos() {
        return new ArrayList<>(itens); // Retorna cópia para evitar modificações externas
    }
    
    /**
     * Busca itens por título (busca parcial case insensitive)
     * @param titulo Termo de busca
     * @return Lista de itens que contém o termo no título
     */
    
    public List<Item> buscarPorTitulo(String titulo) {
        return itens.stream()
                .filter(item -> item.getTitulo().toLowerCase().contains(titulo.toLowerCase()))
                .collect(Collectors.toList());
    }
    
    /**
     * Conta a quantidade de itens por tipo
     * @return Mapa com nome da classe como chave e quantidade como valor
     */
    public Map<String, Long> contarPorTipo() {
        return itens.stream()
                .collect(Collectors.groupingBy(
                    item -> item.getClass().getSimpleName(),
                    Collectors.counting()
                ));
    }
    
    /**
     * Ordena os itens por título (case insensitive)
     */
    public void ordenarPorTitulo() {
        Collections.sort(itens, Comparator.comparing(Item::getTitulo, String.CASE_INSENSITIVE_ORDER));
    }
    
    /**
     * Exporta todos os itens para um arquivo de texto
     * @param caminho Caminho do arquivo de destino
     * @throws IOException Se ocorrer erro de I/O
     */
    public void exportarParaArquivo(String caminho) throws IOException {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(caminho))) {
            for (Item item : itens) {
                writer.write(item.toExportString());
                writer.newLine();
            }
        }
    }
    
    /**
     * Importa itens de um arquivo de texto
     * @param caminho Caminho do arquivo de origem
     * @throws IOException Se ocorrer erro de I/O
     */
    public void importarDeArquivo(String caminho) throws IOException {
        List<Item> itensImportados = new ArrayList<>();
        
        try (BufferedReader reader = new BufferedReader(new FileReader(caminho))) {
            String linha;
            while ((linha = reader.readLine()) != null) {
                Item item = parseLinha(linha);
                if (item != null) {
                    itensImportados.add(item);
                }
            }
        }
        
        // Adiciona apenas itens não duplicados
        for (Item item : itensImportados) {
            boolean duplicado = itens.stream()
                    .anyMatch(i -> i.getTitulo().equalsIgnoreCase(item.getTitulo()));
            if (!duplicado) {
                itens.add(item);
            }
        }
    }
    
    /**
     * Converte uma linha do arquivo em um objeto Item
     * @param linha Linha do arquivo
     * @return Item correspondente ou null se inválido
     */
    private Item parseLinha(String linha) {
        String[] partes = linha.split("\\|");
        if (partes.length < 4) return null;
        
        String tipo = partes[0];
        String titulo = partes[1];
        String descricao = partes[2];
        LocalDate dataCadastro = LocalDate.parse(partes[3]);
        
        try {
            if ("Livro".equals(tipo) && partes.length == 6) {
                String autor = partes[4];
                int paginas = Integer.parseInt(partes[5]);
                return new Livro(titulo, descricao, dataCadastro, autor, paginas);
            } else if ("Filme".equals(tipo) && partes.length == 6) {
                String diretor = partes[4];
                int duracao = Integer.parseInt(partes[5]);
                return new Filme(titulo, descricao, dataCadastro, diretor, duracao);
            }
        } catch (Exception e) {
            System.err.println("Erro ao parsear linha: " + linha);
        }
        
        return null;
    }
    
    /**
     * Remove todos os itens da lista
     */
    public void limparItens() {
        itens.clear();
    }
    
    /**
     * Retorna o total de itens
     * @return Quantidade total de itens
     */
    public int getTotalItens() {
        return itens.size();
    }
}
