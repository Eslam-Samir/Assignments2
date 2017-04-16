#ifndef PARSER_H
#define PARSER_H

#include"treenode.h"
#include<QDebug>

class Parser
{
public:
    static Parser * getParserInstance();
    void setTokens(QVector<QString> tokens, QVector<QString> values);
    TreeNode* getRoot();
    void RunParser();

private:
    static Parser * ParserSingleton;
    QVector<QString> TokensValues;
    QVector<QString> Tokens;
    TreeNode* root;
    QString CurrentToken;
    int pointer;

    Parser();
    TreeNode* stmt_sequence();
    TreeNode* statment();
    TreeNode* if_stmt();
    TreeNode* repeat_stmt();
    TreeNode* assign_stmt();
    TreeNode* read_stmt();
    TreeNode* write_stmt();
    TreeNode* exp();
    TreeNode* simple_exp();
    TreeNode* term();
    TreeNode* factor();
    TreeNode* match(QString token);
    void error();
};

#endif // PARSER_H
