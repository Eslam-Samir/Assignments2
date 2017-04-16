#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "treewindow.h"
#include "Scanner.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_LoadFile_clicked();

    void on_ScanTokens_clicked();

    void on_SaveFile_clicked();

    void on_LoadTokens_clicked();

    void on_SaveTokens_clicked();

    void on_CreateTree_clicked();

private:
    Ui::MainWindow *ui;
    TreeWindow *treeWindow;
};

#endif // MAINWINDOW_H
