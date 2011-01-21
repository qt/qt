
#include <QtGui>

class Notepad : public QWidget
{
    Q_OBJECT

public:
    Notepad();

private slots:
    void quit();

private:
    QTextEdit *textEdit;
    QPushButton *quitButton;

};

Notepad::Notepad()
{
    textEdit = new QTextEdit;
    quitButton = new QPushButton(tr("Quit"));

    connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(textEdit);
    layout->addWidget(quitButton);

    setLayout(layout); 

    setWindowTitle(tr("Notepad"));
}

void Notepad::quit()
{
    QMessageBox messageBox;
    messageBox.setWindowTitle(tr("Notepad"));
    messageBox.setText(tr("Do you really want to quit?"));
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::No);
    if (messageBox.exec() == QMessageBox::Yes) 
        qApp->quit();
}

int main(int argv, char **args)
{
    QApplication app(argv, args);

    Notepad notepad;
    notepad.show();

    return app.exec();
}

#include "main.moc"

