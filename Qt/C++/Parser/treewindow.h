#ifndef TREEWINDOW_H
#define TREEWINDOW_H

#include <QWidget>
#include <QGraphicsScene>
#include <QtMath>
#include "parser.h"
#include "scanner.h"

namespace Ui {
class TreeWindow;
}

class TreeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TreeWindow(QWidget *parent = 0);
    ~TreeWindow();

private:
    Ui::TreeWindow *ui;
    const double NODE_WIDTH = 100, NODE_HEIGHT = 50;
    int INITIAL_X, INITIAL_Y;
    double maxX, maxY, minX, minY;
    QGraphicsScene* canvas;

    void DrawTreeNode(double x, double y, TreeNode* node);
    void DrawTerminalNode(double x, double y, QString name, QString value);
    void DrawNonTerminalNode(double x, double y, QString name, QString value);
    void calcMaxAndMin(double x, double y, TreeNode* node);
    void changeMax(double x, double y);
    void changeMin(double x, double y);
};

#endif // TREEWINDOW_H
