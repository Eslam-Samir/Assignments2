#include "treenode.h"

TreeNode::TreeNode()
{
    this->NextNode = nullptr;
}

TreeNode::TreeNode(QString name, QString value)
{
    this->NextNode = nullptr;
    this->Name = name;
    this->Value = value;
}

void TreeNode::AddChild(TreeNode *child)
{
    this->Children.append(child);
}

void TreeNode::setNextNode(TreeNode *next)
{
    this->NextNode = next;
}

void TreeNode::setName(QString name)
{
    this->Name = name;
}

void TreeNode::setValue(QString value)
{
    this->Value = value;
}

QString TreeNode::getToken()
{
    return this->Name;
}

QString TreeNode::getTokenValue()
{
    return this->Value;
}

TreeNode *TreeNode::getChild(int i)
{
    return this->Children.at(i);
}

TreeNode *TreeNode::getNext()
{
    return this->NextNode;
}

int TreeNode::getChildrenCount()
{
    return this->Children.size();
}

bool TreeNode::isRoot()
{
    return this->Children.isEmpty();
}

bool TreeNode::hasNext()
{
    return (NextNode != nullptr);
}

bool TreeNode::isEmpty()
{
    if(Name.isEmpty() || Value.isEmpty())
    {
        return true;
    }
    return false;
}
