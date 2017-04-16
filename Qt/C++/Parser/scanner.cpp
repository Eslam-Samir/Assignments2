#include "Scanner.h"

Scanner * Scanner::ScannerSingleton = 0;

Scanner::Scanner()
{
    NextState = START;
    RESERVED_WORDS.append("if");
    RESERVED_WORDS.append("then");
    RESERVED_WORDS.append("else");
    RESERVED_WORDS.append("end");
    RESERVED_WORDS.append("repeat");
    RESERVED_WORDS.append("until");
    RESERVED_WORDS.append("read");
    RESERVED_WORDS.append("write");
}

Scanner * Scanner::getScannerInstance() {
    if(ScannerSingleton == nullptr)
    {
        ScannerSingleton = new Scanner();
    }
    return ScannerSingleton;
}

QString Scanner::getTokensString()
{
    QString TokensString = "";
    for(int i = 0; i < Tokens.size(); i++)
    {
        TokensString += QString("%1\t\t%2\r\n").arg(TokensValues.at(i), Tokens.at(i));
    }
    return TokensString;
}

void Scanner::Scan(QString line)
{
    QString token = "";
    QString value = "";
    QChar CurrentChar = 0;
    int i = 0;
    while(i < line.size() || NextState == DONE)
    {
        if(NextState != DONE)
        {
            CurrentChar = line.at(i);
        }
        CurrentState = NextState;
        switch(CurrentState)
        {
        case START:
            if(CurrentChar == ' ')
            {
                NextState = START;
            }
            else if(CurrentChar == '{')
            {
                NextState = IN_COMMENT;
            }
            else if(CurrentChar == ':')
            {
                value += CurrentChar;
                token = "assign";
                NextState = IN_ASSIGN;
            }
            else if(CurrentChar.isLetter())
            {
                value += CurrentChar;
                token = "identifier";
                NextState = IN_IDENTIFIER;
            }
            else if(CurrentChar.isDigit())
            {
                value += CurrentChar;
                token = "number";
                NextState = IN_NUMBER;
            }
            else if(CurrentChar == '+' || CurrentChar == '-' || CurrentChar == '*' ||
                    CurrentChar == '/' || CurrentChar == '(' || CurrentChar == ')' ||
                    CurrentChar == '<' || CurrentChar == ';' ||
                    CurrentChar == '=')
            {
                if(CurrentChar == ';')
                    token = "semi";
                else if(CurrentChar == '(' || CurrentChar == ')')
                    token = CurrentChar;
                else
                    token = "op";
                value += CurrentChar;
                NextState = DONE;
            }
            else
            {
                NextState = DONE;
            }
            i++;
            if(i >= line.length())
                NextState = DONE;
            break;

        case IN_COMMENT:
            if(CurrentChar == '}')
            {
                NextState = START;
            }
            else
            {
                NextState = IN_COMMENT;
            }
            i++;
            break;

        case IN_NUMBER:
            if(CurrentChar.isDigit())
            {
                value += CurrentChar;
                i++;
                if(i == line.length())
                    NextState = DONE;
                else
                    NextState = IN_NUMBER;
            }
            else
            {
                NextState = DONE;
            }
            break;

        case IN_IDENTIFIER:
            if(CurrentChar.isLetter())
            {
                value += CurrentChar;
                NextState = IN_IDENTIFIER;
                i++;
                if(i == line.length())
                    NextState = DONE;
                else
                    NextState = IN_IDENTIFIER;
            }
            else
            {
                NextState = DONE;
            }
            break;

        case IN_ASSIGN:
            if(CurrentChar == '=')
            {
                value += CurrentChar;
                i++;
            }
            NextState = DONE;
            break;

        case DONE:
            NextState = START;
            if(!value.isEmpty())
            {
                TokensValues.append(value);
                Tokens.append(token);
                value = "";
            }
            break;
        }
    }
    for(i = 0; i < TokensValues.size(); i++)
    {
        if(RESERVED_WORDS.contains(TokensValues.at(i)))
        {
            Tokens.remove(i); //replace identifier with reserved word
            Tokens.insert(i, TokensValues.at(i));
        }
    }
}

void Scanner::readTokensFromFile(QFile * file)
{
   if(file->open(QIODevice::ReadOnly | QIODevice::Text))
   {
       QTextStream in(file);
       QString line = "";
       while (!in.atEnd())
       {
           line = in.readLine();
           QStringList fields = line.split(QRegExp("\\s+")); // split on any whitespace character
           if(fields.length() > 1)
           {
               TokensValues.append(fields.at(0));
               Tokens.append(fields.at(1));
           }
       }
       file->close();
   }
   else
   {
       QMessageBox::information(0,"error",file->errorString());
   }
}

QVector<QString> *Scanner::getTokensValues()
{
    return &TokensValues;
}

QVector<QString> *Scanner::getTokens()
{
    return &Tokens;
}



void Scanner::clear()
{
    Tokens.clear();
    TokensValues.clear();
}

bool Scanner::isEmpty()
{
    if(Tokens.isEmpty() || TokensValues.isEmpty())
    {
        return true;
    }
    return false;
}


