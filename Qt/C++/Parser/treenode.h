#ifndef TREENODE_H
#define TREENODE_H

#include<QString>
#include<QVector>

class TreeNode
{
public:
    TreeNode();
    TreeNode(QString name, QString value);
    void AddChild(TreeNode* child);
    void setNextNode(TreeNode* next);
    void setName(QString name);
    void setValue(QString value);
    QString getToken();
    QString getTokenValue();
    TreeNode* getChild(int i);
    TreeNode* getNext();
    int getChildrenCount();
    bool isRoot();
    bool hasNext();
    bool isEmpty();

private:
    QString Name;
    QString Value;
    TreeNode * NextNode;
    QVector<TreeNode*> Children;
};

#endif // TREENODE_H
