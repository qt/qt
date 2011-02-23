#ifndef QVSYNCANIMATIONDRIVER_H
#define QVSYNCANIMATIONDRIVER_H

#include <QtCore/qabstractanimation.h>


class QVSyncAnimationDriverPrivate;
class QVSyncAnimationDriver : public QAnimationDriver
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QVSyncAnimationDriver)

public:
    QVSyncAnimationDriver(QObject *parent = 0);

    void setWidget(QWidget *window);
    QWidget *widget() const;

    void started();
    void stopped();

protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // QVSYNCANIMATIONDRIVER_H
