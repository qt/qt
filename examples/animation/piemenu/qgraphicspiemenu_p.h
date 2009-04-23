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

#ifndef QGRAPHICSPIEMENU_P_H
#define QGRAPHICSPIEMENU_P_H

#include "qgraphicspiemenu.h"
#include "qgraphicspiemenusection_p.h"

#include <QtGui/qicon.h>
#include <QtCore/qstate.h>
#include <QtCore/qstatemachine.h>

class QAction;
class QEventLoop;
class QGraphicsPieMenuSection;

class QGraphicsPieMenuPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsPieMenu);
public:
    void init(const QIcon &icon = QIcon(), const QString &title = QString());

    QIcon icon;
    QString title;
    QStateMachine *machine;
    QState *popupState;
    QList<QGraphicsPieMenuSection *> sections;

    QEventLoop *eventLoop;

    QAction *menuAction;
    QGraphicsPieMenu *q_ptr;

    void updatePopupState();
};

#endif
