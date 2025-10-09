// Implemente uma interface com: Dois campos JTextField para números.
//a. Um JComboBox com as opções “Somar”, “Subtrair”, “Multiplicar” e “Dividir”.
//b. Um JCheckBox para escolher se o resultado deve aparecer em uma
//JOptionPane. Um botão “Calcular”.
//c. Ao clicar, mostre o resultado conforme a operação escolhida e a opção do
//checkbox.

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Calculadora extends JFrame {
    
    private JTextField campoNumero1, campoNumero2;
    private JComboBox<String> comboOperacoes;
    private JCheckBox checkBoxPopup;
    private JButton botaoCalcular;
    private JLabel labelResultado;
    
    public Calculadora() {
        super("Calculadora");
        
        setLayout(new GridLayout(6, 2, 10, 10));
        
        add(new JLabel("Primeiro número:"));
        campoNumero1 = new JTextField();
        add(campoNumero1);
        
        add(new JLabel("Segundo número:"));
        campoNumero2 = new JTextField();
        add(campoNumero2);
        
        add(new JLabel("Operação:"));
        String[] operacoes = {"Somar", "Subtrair", "Multiplicar", "Dividir"};
        comboOperacoes = new JComboBox<>(operacoes);
        add(comboOperacoes);
        
        checkBoxPopup = new JCheckBox("Mostrar resultado em popup");
        add(new JLabel()); 
        add(checkBoxPopup);
        
        botaoCalcular = new JButton("Calcular");
        add(new JLabel()); 
        add(botaoCalcular);
        
        add(new JLabel("Resultado:"));
        labelResultado = new JLabel(" ");
        add(labelResultado);
        
        botaoCalcular.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                calcular();
            }
        });
        
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(400, 250);
        setLocationRelativeTo(null); 
        setVisible(true);
    }
    
    private void calcular() {
        try {
            double num1 = Double.parseDouble(campoNumero1.getText());
            double num2 = Double.parseDouble(campoNumero2.getText());
            String operacao = (String) comboOperacoes.getSelectedItem();
            double resultado = 0;
            
            switch (operacao) {
                case "Somar":
                    resultado = num1 + num2;
                    break;
                case "Subtrair":
                    resultado = num1 - num2;
                    break;
                case "Multiplicar":
                    resultado = num1 * num2;
                    break;
                case "Dividir":
                    if (num2 == 0) {
                        JOptionPane.showMessageDialog(this, 
                            "Erro: Divisão por zero!", 
                            "Erro", 
                            JOptionPane.ERROR_MESSAGE);
                        labelResultado.setText("Erro: Divisão por zero!");
                        return;
                    }
                    resultado = num1 / num2;
                    break;
            }
            
            // Formatar o resultado
            String resultadoFormatado = String.format("%.2f", resultado);
            String mensagem = String.format("%.2f %s %.2f = %.2f", 
                num1, 
                getSimboloOperacao(operacao), 
                num2, 
                resultado);
            
            labelResultado.setText(resultadoFormatado);
            
            if (checkBoxPopup.isSelected()) {
                JOptionPane.showMessageDialog(this, 
                    mensagem, 
                    "Resultado", 
                    JOptionPane.INFORMATION_MESSAGE);
            }
            
        } catch (NumberFormatException ex) {
            JOptionPane.showMessageDialog(this, 
                "Por favor, digite números válidos!", 
                "Erro de Entrada", 
                JOptionPane.ERROR_MESSAGE);
            labelResultado.setText("Erro: Digite números válidos!");
        }
    }
    
    private String getSimboloOperacao(String operacao) {
        switch (operacao) {
            case "Somar": return "+";
            case "Subtrair": return "-";
            case "Multiplicar": return "×";
            case "Dividir": return "÷";
            default: return "?";
        }
    }
    
    public static void main(String[] args) {
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                new Calculadora();
            }
        });
    }
}
