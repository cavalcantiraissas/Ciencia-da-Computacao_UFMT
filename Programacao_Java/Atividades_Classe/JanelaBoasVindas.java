// Crie uma janela (JFrame) com: Um título personalizado. Um JLabel exibindo uma mensagem de boas-vindas. Um JButton com texto “Fechar”. Quando o botão for clicado, a janela deve se fechar.

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class JanelaBoasVindas extends JFrame {
    
    public JanelaBoasVindas() {
        super("Janela Personalizada");
        
        setLayout(new FlowLayout());
        
        JLabel labelBoasVindas = new JLabel("Bem-vindo à minha aplicação Java!");
        add(labelBoasVindas);
        
        JButton botaoFechar = new JButton("Fechar");
        
        botaoFechar.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                dispose();
            }
        });
        
        add(botaoFechar);
        
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(300, 150);
        setLocationRelativeTo(null); 
        setVisible(true);
    }
    
    public static void main(String[] args) {
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                new JanelaBoasVindas();
            }
        });
    }
}
