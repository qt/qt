#ifndef FILEREADER_H
#define FILEREADER_H

#include <QTextEdit>
#include <QPushButton>
#include <QSignalMapper>

class FileReader : public QWidget
{
    Q_OBJECT

public:
    FileReader(QWidget *parent=0);
    void readFromFile(QString filename);

public slots:
    void readFile(const QString &);

private:
    QTextEdit *textEdit;
    QPushButton *taxFileButton;
    QPushButton *accountFileButton;
    QPushButton *reportFileButton;
    QSignalMapper *signalMapper;
};

#endif

