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

#ifndef QSIGNALEVENT_H
#define QSIGNALEVENT_H

#include <QtCore/qcoreevent.h>

#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class Q_CORE_EXPORT QSignalEvent : public QEvent
{
public:
    QSignalEvent(const QObject *sender, int signalIndex,
                 const QList<QVariant> &arguments);
    ~QSignalEvent();

    inline const QObject *sender() const { return m_sender; }
    inline int signalIndex() const { return m_signalIndex; }
    inline QList<QVariant> arguments() const { return m_arguments; }

private:
    const QObject *m_sender;
    int m_signalIndex;
    QList<QVariant> m_arguments;
};

QT_END_NAMESPACE

#endif
