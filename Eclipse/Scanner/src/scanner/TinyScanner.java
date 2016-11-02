package scanner;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class TinyScanner {
	private static final Map<String, String> TOKEN_NAMES = TokensMap();
	private static final List<String> RESERVED_WORDS = Arrays.asList(
		"if", "then", "else", "end", "repeat", "until", "read", "write"
		);
	
	private static State CurrentState;
	private static State NextState;
	private static ArrayList<String> Tokens = new ArrayList<>();

	public void Scan(String line)
	{
		String token;
		char CurrentChar;
		for(int i = 0; i < line.length();)
		{
			token = "";
			NextState = State.START;
			while(NextState != State.DONE && i < line.length())
			{	
				CurrentChar = line.charAt(i);
				CurrentState = NextState;
				if(CurrentState != State.DONE)
				{
					i++;
				}
				switch(CurrentState)
				{
				case START:
					if(CurrentChar == ' ')
					{
						NextState = State.START;
					}
					else if(CurrentChar == '{')
					{
						NextState = State.IN_COMMENT;
					}
					else if(CurrentChar == ':')
					{
						token += CurrentChar;
						NextState = State.IN_ASSIGN;
					}
					else if(Character.isLetter(CurrentChar))
					{
						token += CurrentChar;
						NextState = State.IN_IDENTIFIER;
					}
					else if(Character.isDigit(CurrentChar))
					{
						token += CurrentChar;
						NextState = State.IN_NUMBER;
					}
					else if(CurrentChar == '+' || CurrentChar == '-' || CurrentChar == '*' ||
							CurrentChar == '/' || CurrentChar == '(' || CurrentChar == ')' ||
							CurrentChar == '<' || CurrentChar == '>' || CurrentChar == ';' || 
							CurrentChar == '=')
					{
						token += CurrentChar;
						NextState = State.DONE;
					}
					else
					{
						NextState = State.DONE;
					}
					break;
					
				case IN_COMMENT:
					if(CurrentChar == '}')
					{
						NextState = State.START;
					}
					else
					{
						NextState = State.IN_COMMENT;
					}
					break;
					
				case IN_NUMBER:
					if(Character.isDigit(CurrentChar))
					{
						token += CurrentChar;
						NextState = State.IN_NUMBER;
					}
					else if(CurrentChar == ')') // handle right bracket after number
					{
						i--;
						NextState = State.DONE;
					}
					else
					{
						NextState = State.DONE;
					}
					break;
					
				case IN_IDENTIFIER:
					if(Character.isLetter(CurrentChar))
					{
						token += CurrentChar;
						NextState = State.IN_IDENTIFIER;
					}
					else if(CurrentChar == '(' || CurrentChar == ')') // handle brackets after letters
					{
						i--;
						NextState = State.DONE;
					}
					else
					{
						NextState = State.DONE;
					}
					break;
				
				case IN_ASSIGN:
					if(CurrentChar == '=')
					{
						token += CurrentChar;
						NextState = State.DONE;
					}
					else
					{
						NextState = State.DONE;
					}
					break;
					
				case DONE:
					break;
				}
			}
			if(!token.isEmpty())
			{
				Tokens.add(token);
			}
		}
	}
	
	public void PrintTokens()
	{
		String token;
		System.out.printf("%-30.30s  %-30.30s%n", "Token Value", "Token Type");
		System.out.print("\n");
		for(int i = 0; i < Tokens.size(); i++)
		{
			token = Tokens.get(i);
			if(TOKEN_NAMES.containsKey(token))
			{
				System.out.printf("%-30.30s  %-30.30s%n", token, TOKEN_NAMES.get(token));
			}
			else if(RESERVED_WORDS.contains(token))
			{
				System.out.printf("%-30.30s  %-30.30s%n", token, "RESERVED");
			}
			else
			{
				boolean isNumber = true;
				for(int j = 0; j < token.length(); j++)
				{
					if(!Character.isDigit(token.charAt(j)))
					{
						isNumber = false;
						break;
					}
				}
				if(isNumber)
				{
					System.out.printf("%-30.30s  %-30.30s%n", token, "NUMBER");
				}
				else
				{
					System.out.printf("%-30.30s  %-30.30s%n", token, "IDENTIFIER");
				}
			}
		}
	}
	
	public ArrayList<String> getTokens()
	{
		return Tokens;
	}
	
	private static Map<String, String> TokensMap() {
        Map<String, String> result = new HashMap<String, String>();
        result.put(":=", "ASSIGN");
        result.put(";", "SEMI");
        result.put("+", "PLUS");
        result.put("-", "MINUS");
        result.put("/", "DIVIDE");
        result.put("<", "SMALLER THAN");
        result.put(">", "LARGER THAN");
        result.put("*", "MULTI");
        result.put("(", "LEFT BRACKET");
        result.put(")", "RIGHT BRACKET");
        result.put("=", "EQUAL");
        return result;
    }
}
