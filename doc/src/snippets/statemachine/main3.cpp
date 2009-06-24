
#include <QtGui>

int main(int argv, char **args)
{
    QApplication app(argv, args);

//![0]
    QState *s1 = new QState(QState::ParallelStates);
    // s11 and s12 will be entered in parallel
    QState *s11 = new QState(s1);
    QState *s12 = new QState(s1);
//![0]

//![1]
  s1->addTransition(s1, SIGNAL(finished()), s2);
//![1]

    return app.exec();
}

