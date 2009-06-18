
#include <QtGui>


//![0]
struct StringEvent : public QEvent
{
    StringEvent(const QString &val)
    : QEvent(QEvent::Type(QEvent::User+1)),
      value(val) {}

    QString value;
};
//![0]

//![1]
class StringTransition : public QAbstractTransition
{
public:
    StringTransition(const QString &value)
        : m_value(value) {}

protected:
    virtual bool eventTest(QEvent *e) const
    {
        if (e->type() != QEvent::Type(QEvent::User+1)) // StringEvent
            return false;
        StringEvent *se = static_cast<StringEvent*>(e);
        return (m_value == se->value);
    }
    
    virtual void onTransition(QEvent *) {}

private:
    QString m_value;
};
//![1]

int main(int argv, char **args)
{
    QApplication app(argv, args);

//![2]
    QStateMachine machine;
    QState *s1 = new QState();
    QState *s2 = new QState();
    QFinalState *done = new QFinalState();

    StringTransition *t1 = new StringTransition("Hello");
    t1->setTargetState(s2);
    s1->addTransition(t1);
    StringTransition *t2 = new StringTransition("world");
    t2->setTargetState(done);
    s2->addTransition(t2);

    machine.addState(s1);
    machine.addState(s2);
    machine.addState(done);
    machine.setInitialState(s1);
//![2]

//![3]
    machine.postEvent(new StringEvent("Hello"));
    machine.postEvent(new StringEvent("world"));
//![3]

    return app.exec();
}

#include "main4.moc"

