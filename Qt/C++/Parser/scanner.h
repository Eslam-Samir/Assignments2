#ifndef SCANNER_H
#define SCANNER_H

#include <QVector>
#include <QList>
#include <QString>
#include <QChar>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

class Scanner
{
private:
    Scanner();
    enum State{
        START,
        IN_COMMENT,
        IN_NUMBER,
        IN_IDENTIFIER,
        IN_ASSIGN,
        DONE
    };

    State CurrentState;
    State NextState;
    QVector<QString> TokensValues;
    QVector<QString> Tokens;

    static Scanner * ScannerSingleton;
    QList<QString> RESERVED_WORDS;

public:
    static Scanner * getScannerInstance();
    QString getTokensString();
    void Scan(QString line);
    void readTokensFromFile(QFile * file);
    QVector<QString> * getTokensValues();
    QVector<QString> * getTokens();
    void clear();
    bool isEmpty();
};

#endif // SCANNER_H
