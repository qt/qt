/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSOFTKEYSTACK_P_H
#define QSOFTKEYSTACK_P_H

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


#include "qstack.h"
#include <QtCore/private/qobject_p.h>

#include "qsoftkeyaction.h"
#include "qsoftkeystack.h"

QT_BEGIN_NAMESPACE

// The following 2 defines may only be needed for s60. To be seen.
#define SOFTKEYSTART 5000
#define SOFTKEYEND (5000 + Qt::Key_Context4-Qt::Key_Context1)

#define QSoftkeySet QList <QSoftKeyAction*>

class QSoftKeyStackPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSoftKeyStack);
public:
    QSoftKeyStackPrivate();
    ~QSoftKeyStackPrivate();

    void push(QSoftKeyAction *softKey);
    void push(const QList<QSoftKeyAction*> &softKeys);
    void pop();
    void popandPush(QSoftKeyAction *softKey);
    void popandPush(const QList<QSoftKeyAction*> &softkeys);
    const QSoftkeySet& top();
    bool isEmpty();
    void handleSoftKeyPress(int command);
    
private:
    void mapSoftKeys(const QSoftkeySet &top);
    void setNativeSoftKeys();

private:
    QStack <QSoftkeySet> softKeyStack;
};

class QKeyEventSoftKey : QObject
{
    Q_OBJECT
public:
    QKeyEventSoftKey(QSoftKeyAction *softKeyAction, Qt::Key key, QObject *parent);
    static void addSoftKey(QSoftKeyAction::StandardRole standardRole, Qt::Key key, QWidget *actionWidget);
private:
    QSoftKeyAction *m_softKeyAction;
    Qt::Key m_key;
private Q_SLOTS:
    void sendKeyEvent();
};

QT_END_NAMESPACE

#endif // QSOFTKEYSTACK_P_H
