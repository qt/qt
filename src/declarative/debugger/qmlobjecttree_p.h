/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QMLOBJECTTREE_P_H
#define QMLOBJECTTREE_P_H

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

#include <QtGui/qtreewidget.h>
#include <QtCore/qurl.h>
#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

class QmlBindableValue;
class QmlDebuggerItem : public QTreeWidgetItem
{
public:
    QmlDebuggerItem(QTreeWidget *wid)
        : QTreeWidgetItem(wid), startLine(-1), endLine(-1)
    {
    }

    QmlDebuggerItem(QTreeWidgetItem *item)
        : QTreeWidgetItem(item), startLine(-1), endLine(-1)
    {
    }

    int startLine;
    int endLine;
    QUrl url;

    QPointer<QObject> object;
    QPointer<QmlBindableValue> bindableValue;
};

class QmlContext;
class QmlObjectTree : public QTreeWidget
{
    Q_OBJECT
public:
    QmlObjectTree(QWidget *parent = 0);

signals:
    void addWatch(QObject *, const QString &);

protected:
    virtual void mousePressEvent(QMouseEvent *);
};

QT_END_NAMESPACE

#endif // QMLOBJECTTREE_P_H

