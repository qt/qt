
#include <QtGui>

class MyTransition : public QAbstractTransition
{
    Q_OBJECT
public:
    MyTransition() {}

protected:
//![0]
    bool eventTest(QEvent *event)
    {
        if (event->type() == QEvent::Wrapped) {
            QEvent *wrappedEvent = static_cast<QWrappedEvent *>(event)->event();
            if (wrappedEvent->type() == QEvent::KeyPress) {
                QKeyEvent *keyEvent = static_cast<QKeyEvent *>(wrappedEvent);
                // Do your event test
            }       
        }
        return false;
    }
//![0]      

    void onTransition(QEvent *event)
    {

    }
};

int main(int argv, char **args)
{
    return 0;
}
