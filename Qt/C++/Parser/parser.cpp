#include "Parser.h"

Parser * Parser::ParserSingleton = 0;

Parser *Parser::getParserInstance()
{
    if(ParserSingleton == nullptr)
    {
        ParserSingleton = new Parser();
    }
    return ParserSingleton;
}

void Parser::setTokens(QVector<QString> tokens, QVector<QString> values)
{
    Tokens.clear();
    TokensValues.clear();
    for(int i = 0; i < tokens.size(); i++)
    {
        Tokens.append(tokens.at(i));
        TokensValues.append(values.at(i));
    }
    pointer = 0;
    CurrentToken = Tokens.at(0);
}

TreeNode *Parser::getRoot()
{
    return this->root;
}

void Parser::RunParser()
{
    root = stmt_sequence();
}

Parser::Parser()
{
}

TreeNode *Parser::stmt_sequence()
{
    TreeNode* parent = statment();
    TreeNode* temp = parent;
    while(CurrentToken == "semi")
    {
        match("semi");
        TreeNode* next = statment();
        temp->setNextNode(next);
        temp = next;
    }
    return parent;
}

TreeNode *Parser::statment()
{
    if(CurrentToken == "if")
    {
        return if_stmt();
    }
    else if(CurrentToken == "repeat")
    {
         return repeat_stmt();
    }
    else if(CurrentToken == "identifier")
    {
         return assign_stmt();
    }
    else if(CurrentToken == "read")
    {
         return read_stmt();
    }
    else if(CurrentToken == "write")
    {
         return write_stmt();
    }
    else
    {
        error();
    }
    return nullptr;
}

TreeNode *Parser::if_stmt()
{
    TreeNode* parent = match("if");
    parent->AddChild(exp());
    match("then");
    parent->AddChild(stmt_sequence());

    if(CurrentToken == "else")
    {
            match("else");
            parent->AddChild(stmt_sequence());
    }

    match("end");
    return parent;
}

TreeNode *Parser::repeat_stmt()
{
    TreeNode* parent = match("repeat");
    parent->AddChild(stmt_sequence());
    match("until");
    parent->AddChild(exp());
    return parent;
}

TreeNode *Parser::assign_stmt()
{
    QString value = match("identifier")->getTokenValue();
    TreeNode* parent = match("assign");
    parent->setValue(value);
    parent->AddChild(exp());
    return parent;
}

TreeNode *Parser::read_stmt()
{
    TreeNode* parent = match("read");
    QString value = match("identifier")->getTokenValue();
    parent->setValue(value);
    return parent;
}

TreeNode *Parser::write_stmt()
{
    TreeNode* parent = match("write");
    parent->AddChild(exp());
    return parent;
}

TreeNode *Parser::exp()
{
    TreeNode* parent = new TreeNode();
    TreeNode* left = simple_exp();
    while(CurrentToken == "op" && (TokensValues.at(pointer) == "<" || TokensValues.at(pointer) == "="))
    {
        TreeNode* newLeft = match(CurrentToken);
        newLeft->AddChild(left);
        TreeNode* right = simple_exp();
        newLeft->AddChild(right);
        left = newLeft;
    }
    if(parent->isEmpty())
        return left;
    else
        return parent;
}

TreeNode *Parser::simple_exp()
{
    TreeNode* parent = new TreeNode();
    TreeNode* left = term();
    while(CurrentToken == "op" && (TokensValues.at(pointer) == "+" || TokensValues.at(pointer) == "-"))
    {
        TreeNode* newLeft = match(CurrentToken);
        newLeft->AddChild(left);
        TreeNode* right = term();
        newLeft->AddChild(right);
        left = newLeft;
    }
    if(parent->isEmpty())
        return left;
    else
        return parent;
}

TreeNode *Parser::term()
{
    TreeNode* parent = new TreeNode();
    TreeNode* left = factor();
    while(CurrentToken == "op" && (TokensValues.at(pointer) == "*" || TokensValues.at(pointer) == "/"))
    {
        TreeNode* newLeft = match(CurrentToken);
        newLeft->AddChild(left);
        TreeNode* right = factor();
        newLeft->AddChild(right);
        left = newLeft;
    }
    if(parent->isEmpty())
        return left;
    else
        return parent;
}

TreeNode *Parser::factor()
{
    if(CurrentToken == "(")
    {
        match("(");
        TreeNode* node = exp();
        match(")");
        return node;
    }
    else if(CurrentToken == "number" || CurrentToken == "identifier")
    {
         return match(CurrentToken);
    }
    else
    {
         error();
    }
    return nullptr;
}

TreeNode *Parser::match(QString token)
{
    qDebug() << "matching " << CurrentToken << " With " << token << " " << pointer;
    if(CurrentToken == token)
    {
        TreeNode* node = new TreeNode(CurrentToken, TokensValues.at(pointer));
        qDebug() << "matched " << CurrentToken << " " << pointer;
        pointer++;
        if(pointer < Tokens.size())
            CurrentToken = Tokens.at(pointer);
        return node;
    }
    else
    {
        error();
    }
    return nullptr;
}

void Parser::error()
{
    QString error = "Error At Token: " + CurrentToken;
    QString content = "\nToken Number: " + pointer;
    qDebug() << error << content;
}
