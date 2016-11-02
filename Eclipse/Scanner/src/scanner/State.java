package scanner;

public enum State {
	START,
	IN_COMMENT,
	IN_NUMBER,
	IN_IDENTIFIER,
	IN_ASSIGN,
	DONE
}
