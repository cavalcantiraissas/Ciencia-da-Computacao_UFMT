//Desenvolva uma janela contendo: Um JLabel e um JTextField para o nome de usuário. Um JLabel e um JPasswordField para senha. Um JButton “Entrar”. Ao clicar em “Entrar”, exiba uma JOptionPane com uma mensagem de boas-vindas que inclua o nome digitado.

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class JanelaLogin extends JFrame {
    
    private JTextField campoUsuario;
    private JPasswordField campoSenha;
    
    public JanelaLogin() {
        super("Sistema de Login");
        
        setLayout(new GridLayout(3, 2, 10, 10));
        
        JLabel labelUsuario = new JLabel("Usuário:");
        campoUsuario = new JTextField();
        add(labelUsuario);
        add(campoUsuario);
        
        JLabel labelSenha = new JLabel("Senha:");
        campoSenha = new JPasswordField();
        add(labelSenha);
        add(campoSenha);
        
        JButton botaoEntrar = new JButton("Entrar");
        
        botaoEntrar.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                realizarLogin();
            }
        });
        
        add(new JLabel()); 
        add(botaoEntrar);
        
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(300, 150);
        setLocationRelativeTo(null);
        setVisible(true);
    }
    
    private void realizarLogin() {
        String usuario = campoUsuario.getText();
        String senha = new String(campoSenha.getPassword());
        
        if (usuario.isEmpty() || senha.isEmpty()) {
            JOptionPane.showMessageDialog(this, 
                "Por favor, preencha todos os campos!", 
                "Aviso", 
                JOptionPane.WARNING_MESSAGE);
            return;
        }
        
        String mensagem = String.format("Bem-vindo, %s!\nLogin realizado com sucesso!", usuario);
        JOptionPane.showMessageDialog(this, 
            mensagem, 
            "Login Bem-sucedido", 
            JOptionPane.INFORMATION_MESSAGE);
        
        campoUsuario.setText("");
        campoSenha.setText("");
    }
    
    public static void main(String[] args) {
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                new JanelaLogin();
            }
        });
    }
}
