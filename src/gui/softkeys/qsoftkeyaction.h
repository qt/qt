/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QSOFTKEYACTION_H
#define QSOFTKEYACTION_H

#include <QtGui/qaction.h>

class QSoftKeyActionPrivate;

class QSoftKeyAction : public QAction
{
    Q_OBJECT
public:
    enum StandardRole
    {
        Options,
        Select,
        Back,
        Next,
        Previous,
        Ok,
        Cancel,
        Edit,
        View,
        BackSpace,
        EndEdit,
        RevertEdit,
        Deselect,
        Finish,
        Custom
    };

    QSoftKeyAction(QObject *parent);
    QSoftKeyAction(QSoftKeyAction::StandardRole role, QObject *parent);
    QSoftKeyAction(const QString &text, QObject* parent);
    QSoftKeyAction(QSoftKeyAction::StandardRole role, const QString &text, QObject* parent);
    QSoftKeyAction(const QIcon &icon, const QString &text, QObject* parent);
    QSoftKeyAction(QSoftKeyAction::StandardRole role, const QIcon &icon, const QString &text, QObject* parent);
    ~QSoftKeyAction();

    QSoftKeyAction::StandardRole role() const;
    QString roleName() const;

    void setRole(QSoftKeyAction::StandardRole role);
    void setRole(const QString& role);

    QSoftKeyActionPrivate *d;
};

#endif // QSOFTKEYACTION_H
