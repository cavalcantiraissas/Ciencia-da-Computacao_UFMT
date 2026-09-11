package exceptions;

/**
 * Exceção lançada quando um campo obrigatório está vazio
 */

public class CampoVazioException extends Exception {
    public CampoVazioException(String campo) {
        super("O campo '" + campo + "' não pode estar vazio.");
    }
}
