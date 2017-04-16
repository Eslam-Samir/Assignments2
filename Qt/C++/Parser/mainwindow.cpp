#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_LoadFile_clicked()
{
    QFileDialog fileDialog;
    QString name = fileDialog.getOpenFileName(this, tr("Open Tiny Code File"),
                                 "//", tr("Text Files (*.txt)"));

    if(!name.isEmpty())
    {
        QFile file(name);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            QString content = "";
            while (!in.atEnd())
            {
                content += in.readLine();
            }
            ui->Field1->document()->setPlainText(content);
            file.close();
        }
        else
        {
            QMessageBox::information(0,"error",file.errorString());
        }
    }
}

void MainWindow::on_ScanTokens_clicked()
{
    if(!ui->Field1->document()->isEmpty())
    {
        Scanner * scanner = Scanner::getScannerInstance();
        scanner->Scan(ui->Field1->toPlainText());
        ui->Field2->document()->setPlainText(scanner->getTokensString());
    }
}

void MainWindow::on_SaveFile_clicked()
{
    QFileDialog fileDialog;
    QString name = fileDialog.getSaveFileName(this, tr("Save Tiny Code File"),
                                 "//", tr("Text Files (*.txt)"));

    if(!name.isEmpty())
    {
        QFile file(name);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            QString content = ui->Field1->toPlainText();
            out << content;
            file.close();
        }
        else
        {
            QMessageBox::information(0,"error",file.errorString());
        }
    }
}

void MainWindow::on_LoadTokens_clicked()
{
    QFileDialog fileDialog;
    QString name = fileDialog.getOpenFileName(this, tr("Open Tiny Tokens File"),
                                 "//", tr("Text Files (*.txt)"));

    if(!name.isEmpty())
    {
        Scanner * scanner = Scanner::getScannerInstance();
        QFile file(name);
        scanner->readTokensFromFile(&file);
        ui->Field2->document()->setPlainText(scanner->getTokensString());
    }
}

void MainWindow::on_SaveTokens_clicked()
{
    QFileDialog fileDialog;
    QString name = fileDialog.getSaveFileName(this, tr("Save Tiny Tokens File"),
                                 "//", tr("Text Files (*.txt)"));

    if(!name.isEmpty())
    {
        QFile file(name);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            QString content = ui->Field2->toPlainText();
            out << content;
            file.close();
        }
        else
        {
            QMessageBox::information(0,"error",file.errorString());
        }
    }
}

void MainWindow::on_CreateTree_clicked()
{
    treeWindow = new TreeWindow();
    treeWindow->show();
}
