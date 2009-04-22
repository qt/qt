/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QSIGNALEVENTGENERATOR_P_H
#define QSIGNALEVENTGENERATOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QStateMachine;

class QSignalEventGenerator : public QObject
{
public:
    QSignalEventGenerator(
#ifdef QT_STATEMACHINE_SOLUTION
        int signalIndex,
#endif
        QStateMachine *parent);

    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **argv);

private:
#ifdef QT_STATEMACHINE_SOLUTION
    int signalIndex;
#endif
    Q_DISABLE_COPY(QSignalEventGenerator)
};

QT_END_NAMESPACE

#endif
