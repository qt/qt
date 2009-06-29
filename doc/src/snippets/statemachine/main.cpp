
#include <QtGui>

int main(int argv, char **args)
{
    QApplication app(argv, args);

    QLabel *label = new QLabel;

//![0]
    QStateMachine machine;
    QState *s1 = new QState();
    QState *s2 = new QState();
    QState *s3 = new QState();
//![0]

//![4]
    s1->assignProperty(label, "text", "In state s1");
    s2->assignProperty(label, "text", "In state s2");
    s3->assignProperty(label, "text", "In state s3");
//![4]

//![5]
    QObject::connect(s3, SIGNAL(entered()), button, SLOT(showMaximized()));
    QObject::connect(s3, SIGNAL(exited()), button, SLOT(showMinimized()));
//![5]

//![1]
    s1->addTransition(button, SIGNAL(clicked()), s2);
    s2->addTransition(button, SIGNAL(clicked()), s3);
    s3->addTransition(button, SIGNAL(clicked()), s1);
//![1]

//![2]
    machine.addState(s1);
    machine.addState(s2);
    machine.addState(s3);
    machine.setInitialState(s1);
//![2]

//![3]
    machine.start();
//![3]

    label->show();

    return app.exec();
}
