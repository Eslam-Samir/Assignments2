package scanner;

public class Main 
{
	public static void main(String[] args) 
	{
		String line = "read := x[index] + b[y] - c(u);\n x = 0; \ny <3  {input an integer }";
		TinyScanner scanner = new TinyScanner();
		scanner.Scan(line);
		scanner.PrintTokens();
	}
}
