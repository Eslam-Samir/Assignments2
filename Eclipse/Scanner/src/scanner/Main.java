package scanner;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

public class Main 
{
	public static void main(String[] args) 
	{

			BufferedReader file;
			try {
				file = new BufferedReader(new FileReader("input.txt"));
				String line = "";
				TinyScanner scanner = new TinyScanner();
				while((line = file.readLine()) != null)
				{
					scanner.Scan(line);
				}	
				scanner.PrintTokens("output.txt");
				file.close();
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
	}
}
