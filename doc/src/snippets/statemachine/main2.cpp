
#include <QtGui>

int main(int argv, char **args)
{
  QApplication app(argv, args);

    QStateMachine machine;

//![0]
    QState *s1 = new QState();
    QState *s11 = new QState(s1);
    QState *s12 = new QState(s1);
    QState *s13 = new QState(s1);
    s1->setInitialState(s11);
    machine.addState(s1);
//![0]

//![2]
    s12>addTransition(quitButton, SIGNAL(clicked()), s12);
//![2]

//![1]
    QFinalState *s2 = new QFinalState();
    s1->addTransition(quitButton, SIGNAL(clicked()), s2);
    machine.addState(s2);

    QObject::connect(&machine, SIGNAL(finished()), QApplication::instance(), SLOT(quit()));
//![1]

  QButton *interruptButton = new QPushButton("Interrupt Button");

//![3]
    QHistoryState *s1h = s1->addHistoryState();

    QState *s3 = new QState();
    s3->assignProperty(label, "text", "In s3");
    QMessageBox mbox;
    mbox.addButton(QMessageBox::Ok);
    mbox.setText("Interrupted!");
    mbox.setIcon(QMessageBox::Information);
    QObject::connect(s3, SIGNAL(entered()), &mbox, SLOT(exec()));
    s3->addTransition(s1h);
    machine.addState(s3);

    s1->addTransition(interruptButton, SIGNAL(clicked()), s3);
//![3]

  return app.exec();
}

