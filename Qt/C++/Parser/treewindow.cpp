#include "treewindow.h"
#include "ui_treewindow.h"

TreeWindow::TreeWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TreeWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setWindowState(Qt::WindowMaximized);

    canvas = new QGraphicsScene();
    ui->graphicsView->setScene(canvas);

    INITIAL_X = 0;
    INITIAL_Y = 0;
    maxX = 0;
    maxY = 0;
    minX = 0;
    minY = 0;

    Parser* parser = Parser::getParserInstance();
    Scanner* scanner = Scanner::getScannerInstance();
    parser->setTokens(*scanner->getTokens(), *scanner->getTokensValues());
    parser->RunParser();

    TreeNode * root = parser->getRoot();
    calcMaxAndMin(INITIAL_X, INITIAL_Y, root);
    maxX = maxX - minX + 2 * NODE_WIDTH;
    maxY = maxY - minY + 2 * NODE_HEIGHT;
    INITIAL_X = (int)(NODE_WIDTH - minX/2);
    INITIAL_Y = (int)(NODE_HEIGHT);

    minX = NODE_WIDTH;
    minY = NODE_HEIGHT;
    canvas->setSceneRect(0,0,maxX,maxY);
    DrawTreeNode(INITIAL_X, INITIAL_Y, root);
}

TreeWindow::~TreeWindow()
{
    delete ui;
}

void TreeWindow::DrawTreeNode(double x, double y, TreeNode *node)
{
    QString token = node->getToken();
    QString tokenValue = "(" + node->getTokenValue() + ")";
    bool isRoot = node->isRoot();
    int childrenCount = node->getChildrenCount();
    int padding = 0;

    if(token == "op" || token == "identifier" || token == "number")
        DrawTerminalNode(x, y, token, tokenValue);
    else
        DrawNonTerminalNode(x, y, token, tokenValue);

    if(!isRoot)
    {
        double x1, y1, x2, y2, theta, r, initial_theta;
        if((initial_theta = 70 - 10*childrenCount) <= 0)
            initial_theta = 15;

        x1 = x + NODE_WIDTH/2;
        y1 = y + NODE_HEIGHT;
        if(childrenCount == 1)
        {
            theta = qDegreesToRadians(90.0);
            r = NODE_HEIGHT/qSin(theta);
            x2 = x1 + r * qCos(theta);
            y2 = y1 + r * qSin(theta);

            canvas->addLine(x1, y1, x2, y2);
            DrawTreeNode(x2 - NODE_WIDTH/2, y2, node->getChild(0));
        }
        else
        {
            for(int i = 0; i < childrenCount; i++)
            {
                if(node->getChild(0)->getChildrenCount() == 2 && node->getChild(1)->getChildrenCount() == 2)
                {
                    initial_theta = 35;
                }
                theta = -(initial_theta + i*((180 - 2*initial_theta) / (childrenCount-1)));
                theta = qDegreesToRadians(theta);
                r = NODE_HEIGHT/qSin(theta);
                r *= (childrenCount/1); // scale r depending on number of children
                x2 = x1 + r * qCos(theta) + padding;
                y2 = y1 + r * qSin(theta);

                canvas->addLine(x1, y1, x2, y2);
                DrawTreeNode(x2 - NODE_WIDTH/2, y2, node->getChild(i));

                TreeNode* next = node->getChild(i)->getNext();
                while(next != nullptr)
                {
                    padding += NODE_WIDTH * (childrenCount + 1);
                    next = next->getNext();
                }
            }
        }
    }
    if(node->hasNext())
    {
        double x1, y1, x2, y2;
        x1 = x + NODE_WIDTH;
        y1 = y + NODE_HEIGHT/2;

        x2 = x1 + NODE_WIDTH * (childrenCount + 1);
        y2 = y1;

        canvas->addLine(x1, y1, x2, y2);
        DrawTreeNode(x2, y2 - NODE_HEIGHT/2, node->getNext());
    }
}

void TreeWindow::DrawTerminalNode(double x, double y, QString name, QString value)
{
    canvas->addEllipse(x, y, NODE_WIDTH, NODE_HEIGHT);
}

void TreeWindow::DrawNonTerminalNode(double x, double y, QString name, QString value)
{
    canvas->addRect(x, y, NODE_WIDTH, NODE_HEIGHT);
}

void TreeWindow::calcMaxAndMin(double x, double y, TreeNode *node)
{
    bool isRoot = node->isRoot();
    int childrenCount = node->getChildrenCount();
    int padding = 0;

    if(!isRoot)
    {
        double x1, y1, x2, y2, theta, r, initial_theta;
        if((initial_theta = 70 - 10*childrenCount) <= 0)
            initial_theta = 15;

        x1 = x + NODE_WIDTH/2;
        y1 = y + NODE_HEIGHT;
        if(childrenCount == 1)
        {
            theta = qDegreesToRadians(90.0);
            r = NODE_HEIGHT/qSin(theta);
            x2 = x1 + r * qCos(theta);
            y2 = y1 + r * qSin(theta);

            changeMax(x2 + NODE_WIDTH/2, y2 + NODE_HEIGHT);
            changeMax(x2 - NODE_WIDTH/2, y2);
            calcMaxAndMin(x2 - NODE_WIDTH/2, y2, node->getChild(0));
        }
        else
        {
            for(int i = 0; i < childrenCount; i++)
            {
                if(node->getChild(0)->getChildrenCount() == 2 && node->getChild(1)->getChildrenCount() == 2)
                {
                    initial_theta = 35;
                }
                theta = -(initial_theta + i*((180 - 2*initial_theta) / (childrenCount-1)));
                theta = qDegreesToRadians(theta);
                r = NODE_HEIGHT/qSin(theta);
                r *= (childrenCount/1); // scale r depending on number of children
                x2 = x1 + r * qCos(theta) + padding;
                y2 = y1 + r * qSin(theta);

                changeMax(x2 + NODE_WIDTH/2, y2 + NODE_HEIGHT);
                changeMin(x2 - NODE_WIDTH/2, y2);
                calcMaxAndMin(x2 - NODE_WIDTH/2, y2, node->getChild(i));

                TreeNode* next = node->getChild(i)->getNext();
                while(next != nullptr)
                {
                    padding += NODE_WIDTH * (childrenCount + 1);
                    next = next->getNext();
                }
            }
        }
    }

    if(node->hasNext())
    {
        double x1, y1, x2, y2;
        x1 = x + NODE_WIDTH;
        y1 = y + NODE_HEIGHT/2;

        x2 = x1 + NODE_WIDTH * (childrenCount + 1);
        y2 = y1;

        changeMax(x2, y2 + NODE_HEIGHT);
        calcMaxAndMin(x2, y2 - NODE_HEIGHT/2, node->getNext());
    }
}
void TreeWindow::changeMax(double x, double y)
{
    if(x > maxX)
        maxX = x;
    if(y > maxY)
        maxY = y;
}
void TreeWindow::changeMin(double x, double y)
{
    if(x < minX)
        minX = x;
    if(y < minY)
        minY = y;
}
