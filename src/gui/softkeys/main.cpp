
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include "qsoftkeystack.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow() {}
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    QPushButton *button = new QPushButton("Hello");
    layout->addWidget(button);

    QSoftKeyStack *stack = new QSoftKeyStack(central);
    QSoftKeyAction action(central);
    action.setRole(QSoftKeyAction::Back);
    int role = action.role();
    stack->push(&action);

    setCentralWidget(central);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mw;
    mw.show();
    return app.exec();
}

#include "main.moc"
