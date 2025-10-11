package exceptions;

/**
 * Exceção lançada quando um item com o mesmo título já existe
 */

public class DuplicadoException extends Exception {
    public DuplicadoException(String titulo) {
        super("Já existe um item com o título: " + titulo);
    }
}
