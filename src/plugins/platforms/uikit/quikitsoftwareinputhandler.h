#ifndef QUIKITSOFTWAREINPUTHANDLER_H
#define QUIKITSOFTWAREINPUTHANDLER_H

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QUIKitSoftwareInputHandler : public QObject
{
    Q_OBJECT

public:
    bool eventFilter(QObject *obj, QEvent *event);
};

QT_END_NAMESPACE

#endif
