// ItemManagerGUI.java
package gui;

import exceptions.CampoVazioException;
import exceptions.DuplicadoException;
import java.awt.*;
import java.io.IOException;
import java.util.List;
import java.util.Map;
import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import model.Filme;
import model.Item;
import model.Livro;
import service.GerenciadorItens;

/**
 * Interface gráfica principal para gerenciamento de itens (Livros e Filmes)
 * Responsável pela interação com o usuário e exibição dos dados
 */

public class ItemManagerGUI extends JFrame {
    private GerenciadorItens gerenciador;
    private Color corExclusiva = Color.decode("#64AC64"); // Cor verde personalizada
    
    // Componentes da GUI
    private JTextField txtTitulo, txtDescricao, txtAutor, txtPaginas, txtDiretor, txtDuracao, txtBusca;
    private JComboBox<String> comboTipo;
    private JTextArea areaResultado;
    private JButton btnAdicionar, btnListar, btnBuscar, btnExportar, btnImportar;
    private JPanel panelLateral;
    private JPanel panelEspecifico;
    private CardLayout cardLayout;
    
    public ItemManagerGUI() {
        this.gerenciador = new GerenciadorItens();
        inicializarComponentes();
        configurarLayout();
        configurarEventos();
    }
    
    /**
     * Inicializa todos os componentes da interface
     */

    private void inicializarComponentes() {
        // Campos de texto para dados básicos
        txtTitulo = new JTextField(20);
        txtDescricao = new JTextField(20);
        
        // Campos específicos para Livro
        txtAutor = new JTextField(20);
        txtPaginas = new JTextField(20);
        
        // Campos específicos para Filme
        txtDiretor = new JTextField(20);
        txtDuracao = new JTextField(20);
        
        // ComboBox para seleção do tipo de item
        comboTipo = new JComboBox<>(new String[]{"Livro", "Filme"});
        
        // Campo de busca
        txtBusca = new JTextField(20);
        
        // Área de texto para exibir resultados
        areaResultado = new JTextArea(15, 40);
        areaResultado.setEditable(false);
        
        // Botões de ação
        btnAdicionar = new JButton("Adicionar");
        btnListar = new JButton("Listar Todos");
        btnBuscar = new JButton("Filtrar");
        btnExportar = new JButton("Exportar Dados");
        btnImportar = new JButton("Importar Dados");
        
        // Painel lateral decorativo
        panelLateral = new JPanel();
        panelLateral.setBackground(corExclusiva);
        panelLateral.setPreferredSize(new Dimension(200, 600));
        
        // Layout de cartas para alternar entre painéis específicos
        cardLayout = new CardLayout();
        panelEspecifico = new JPanel(cardLayout);
    }
    
    /**
     * Configura o layout e posicionamento dos componentes
     */

    private void configurarLayout() {
        setTitle("Gerenciador de Itens - RAISSA CAROLINE CAVALCANTI DA SILVA");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(new BorderLayout());
        
        // Painel superior com título
        JPanel panelSuperior = new JPanel();
        panelSuperior.setBackground(corExclusiva);
        JLabel lblTitulo = new JLabel("Sistema de Gerenciamento de Itens");
        lblTitulo.setFont(new Font("Arial", Font.BOLD, 16));
        lblTitulo.setForeground(Color.WHITE);
        panelSuperior.add(lblTitulo);
        
        JPanel panelPrincipal = new JPanel(new BorderLayout());
        
        // Painel de formulário com layout de grade
        JPanel panelForm = new JPanel(new GridLayout(0, 2, 5, 5));
        
        // Adiciona componentes ao formulário
        panelForm.add(new JLabel("Tipo:"));
        panelForm.add(comboTipo);
        
        panelForm.add(new JLabel("Título:*"));
        panelForm.add(txtTitulo);
        
        panelForm.add(new JLabel("Descrição:*"));
        panelForm.add(txtDescricao);
        
        // Painel específico para Livro
        JPanel panelLivro = new JPanel(new GridLayout(0, 2, 5, 5));
        panelLivro.add(new JLabel("Autor:*"));
        panelLivro.add(txtAutor);
        panelLivro.add(new JLabel("Número de Páginas:*"));
        panelLivro.add(txtPaginas);
        
        // Painel específico para Filme
        JPanel panelFilme = new JPanel(new GridLayout(0, 2, 5, 5));
        panelFilme.add(new JLabel("Diretor:*"));
        panelFilme.add(txtDiretor);
        panelFilme.add(new JLabel("Duração (min):*"));
        panelFilme.add(txtDuracao);
        
        // Adiciona painéis específicos ao card layout
        panelEspecifico.add(panelLivro, "Livro");
        panelEspecifico.add(panelFilme, "Filme");
        
        panelForm.add(new JLabel("Detalhes Específicos:"));
        panelForm.add(panelEspecifico);
        
        // Painel de botões principais
        JPanel panelBotoes = new JPanel(new FlowLayout());
        panelBotoes.add(btnAdicionar);
        panelBotoes.add(btnListar);
        
        // Painel de busca
        JPanel panelBusca = new JPanel(new FlowLayout());
        panelBusca.add(new JLabel("Buscar:"));
        panelBusca.add(txtBusca);
        panelBusca.add(btnBuscar);
        
        // Painel de exportação/importação
        JPanel panelIO = new JPanel(new FlowLayout());
        panelIO.add(btnExportar);
        panelIO.add(btnImportar);
        
        // Área de resultado com scroll
        JScrollPane scrollPane = new JScrollPane(areaResultado);
        
        // Montagem do painel central
        JPanel panelCentral = new JPanel(new BorderLayout());
        panelCentral.add(panelForm, BorderLayout.NORTH);
        panelCentral.add(panelBotoes, BorderLayout.CENTER);
        panelCentral.add(panelBusca, BorderLayout.SOUTH);
        
        // Montagem do painel principal
        panelPrincipal.add(panelCentral, BorderLayout.NORTH);
        panelPrincipal.add(scrollPane, BorderLayout.CENTER);
        panelPrincipal.add(panelIO, BorderLayout.SOUTH);
        
        // Adiciona todos os painéis ao frame
        add(panelSuperior, BorderLayout.NORTH);
        add(panelLateral, BorderLayout.WEST);
        add(panelPrincipal, BorderLayout.CENTER);
        
        pack();
        setLocationRelativeTo(null); // Centraliza a janela
    }
    
    /**
     * Configura os eventos e listeners dos componentes
     */

    private void configurarEventos() {
        // Evento para mudar campos específicos baseado no tipo selecionado
        comboTipo.addActionListener(e -> {
            String tipoSelecionado = (String) comboTipo.getSelectedItem();
            cardLayout.show(panelEspecifico, tipoSelecionado);
        });
        
        // Evento do botão Adicionar
        btnAdicionar.addActionListener(e -> adicionarItem());
        
        // Evento do botão Listar
        btnListar.addActionListener(e -> listarItens());
        
        // Evento do botão Buscar
        btnBuscar.addActionListener(e -> buscarItens());
        
        // Evento do botão Exportar
        btnExportar.addActionListener(e -> exportarDados());
        
        // Evento do botão Importar
        btnImportar.addActionListener(e -> importarDados());
    }
    
    /**
     * Adiciona um novo item (Livro ou Filme) à lista
     * Realiza validações e trata exceções
     */


    private void adicionarItem() {
        try {
            String titulo = txtTitulo.getText().trim();
            String descricao = txtDescricao.getText().trim();
            String tipo = (String) comboTipo.getSelectedItem();
            
            // Validações básicas de campos obrigatórios
            if (titulo.isEmpty()) throw new CampoVazioException("título");
            if (descricao.isEmpty()) throw new CampoVazioException("descrição");
            
            Item item;
            if ("Livro".equals(tipo)) {
                String autor = txtAutor.getText().trim();
                String paginasStr = txtPaginas.getText().trim();
                
                if (autor.isEmpty()) throw new CampoVazioException("autor");
                if (paginasStr.isEmpty()) throw new CampoVazioException("número de páginas");
                
                int paginas = Integer.parseInt(paginasStr);
                item = new Livro(titulo, descricao, autor, paginas);
            } else {
                String diretor = txtDiretor.getText().trim();
                String duracaoStr = txtDuracao.getText().trim();
                
                if (diretor.isEmpty()) throw new CampoVazioException("diretor");
                if (duracaoStr.isEmpty()) throw new CampoVazioException("duração");
                
                int duracao = Integer.parseInt(duracaoStr);
                item = new Filme(titulo, descricao, diretor, duracao);
            }
            
            gerenciador.adicionarItem(item);
            limparCampos();
            JOptionPane.showMessageDialog(this, "Item adicionado com sucesso!", "Sucesso", JOptionPane.INFORMATION_MESSAGE);
            
        } catch (CampoVazioException | DuplicadoException ex) {
            JOptionPane.showMessageDialog(this, ex.getMessage(), "Erro de Validação", JOptionPane.ERROR_MESSAGE);
        } catch (NumberFormatException ex) {
            JOptionPane.showMessageDialog(this, "Número inválido em campos numéricos", "Erro de Formato", JOptionPane.ERROR_MESSAGE);
        } catch (IllegalArgumentException ex) {
            JOptionPane.showMessageDialog(this, ex.getMessage(), "Erro de Validação", JOptionPane.ERROR_MESSAGE);
        }
    }
    
    /**
     * Lista todos os itens ordenados por título
     * Inclui contagem por tipo
     */


    private void listarItens() {
        gerenciador.ordenarPorTitulo();
        List<Item> itens = gerenciador.listarTodos();
        exibirItens(itens);
        
        // Mostrar contagem por tipo
        Map<String, Long> contagem = gerenciador.contarPorTipo();
        areaResultado.append("\n=== CONTAGEM POR TIPO ===\n");
        for (Map.Entry<String, Long> entry : contagem.entrySet()) {
            areaResultado.append(entry.getKey() + ": " + entry.getValue() + "\n");
        }
    }
    
    /**
     * Busca itens por título
     * Se o termo de busca estiver vazio, lista todos os itens
     */

    private void buscarItens() {
        String termo = txtBusca.getText().trim();
        if (termo.isEmpty()) {
            listarItens();
            return;
        }
        
        List<Item> resultados = gerenciador.buscarPorTitulo(termo);
        exibirItens(resultados);
        areaResultado.append("\n=== " + resultados.size() + " resultado(s) encontrado(s) ===\n");
    }
    
    /**
     * Exibe a lista de itens na área de resultado
     * @param itens Lista de itens a ser exibida
     */


    private void exibirItens(List<Item> itens) {
        areaResultado.setText("");
        if (itens.isEmpty()) {
            areaResultado.setText("Nenhum item encontrado.");
            return;
        }
        
        for (int i = 0; i < itens.size(); i++) {
            areaResultado.append("=== ITEM " + (i + 1) + " ===\n");
            areaResultado.append(itens.get(i).exibirDetalhes() + "\n\n");
        }
    }
    
    /**
     * Exporta os dados para um arquivo de texto
     */

     
    private void exportarDados() {
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setDialogTitle("Exportar Dados");
        fileChooser.setFileFilter(new FileNameExtensionFilter("Arquivos de Texto (*.txt)", "txt"));
        
        if (fileChooser.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {
            try {
                String caminho = fileChooser.getSelectedFile().getAbsolutePath();
                // Garante que o arquivo tenha extensão .txt
                if (!caminho.toLowerCase().endsWith(".txt")) {
                    caminho += ".txt";
                }
                
                gerenciador.exportarParaArquivo(caminho);
                JOptionPane.showMessageDialog(this, "Dados exportados com sucesso para: " + caminho, 
                        "Exportação Concluída", JOptionPane.INFORMATION_MESSAGE);
            } catch (IOException ex) {
                JOptionPane.showMessageDialog(this, "Erro ao exportar dados: " + ex.getMessage(), 
                        "Erro de Exportação", JOptionPane.ERROR_MESSAGE);
            }
        }
    }
    
    /**
     * Importa dados de um arquivo de texto
     */
    private void importarDados() {
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setDialogTitle("Importar Dados");
        fileChooser.setFileFilter(new FileNameExtensionFilter("Arquivos de Texto (*.txt)", "txt"));
        
        if (fileChooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            try {
                gerenciador.importarDeArquivo(fileChooser.getSelectedFile().getAbsolutePath());
                JOptionPane.showMessageDialog(this, "Dados importados com sucesso!", 
                        "Importação Concluída", JOptionPane.INFORMATION_MESSAGE);
                listarItens();
            } catch (IOException ex) {
                JOptionPane.showMessageDialog(this, "Erro ao importar dados: " + ex.getMessage(), 
                        "Erro de Importação", JOptionPane.ERROR_MESSAGE);
            }
        }
    }
    
    /**
     * Limpa todos os campos de entrada
     */
    private void limparCampos() {
        txtTitulo.setText("");
        txtDescricao.setText("");
        txtAutor.setText("");
        txtPaginas.setText("");
        txtDiretor.setText("");
        txtDuracao.setText("");
    }
    
    /**
     * Método principal - ponto de entrada da aplicação
     */

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            try {
                // Configura o look and feel do sistema
                String systemLookAndFeel = UIManager.getSystemLookAndFeelClassName();
                UIManager.setLookAndFeel(systemLookAndFeel);
            } catch (Exception e) {
                // Fallback para o look and feel padrão em caso de erro
                try {
                    UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
            
            new ItemManagerGUI().setVisible(true);
        });
    }
}
