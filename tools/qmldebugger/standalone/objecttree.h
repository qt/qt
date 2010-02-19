/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt QML Debugger of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef OBJECTTREE_H
#define OBJECTTREE_H

#include <QtGui/qtreewidget.h>

QT_BEGIN_NAMESPACE

class QTreeWidgetItem;

class QmlEngineDebug;
class QmlDebugObjectReference;
class QmlDebugObjectQuery;
class QmlDebugContextReference;
class QmlDebugConnection;


class ObjectTree : public QTreeWidget
{
    Q_OBJECT
public:
    ObjectTree(QmlEngineDebug *client = 0, QWidget *parent = 0);

    void setEngineDebug(QmlEngineDebug *client);
    
signals:
    void currentObjectChanged(const QmlDebugObjectReference &);
    void activated(const QmlDebugObjectReference &);
    void expressionWatchRequested(const QmlDebugObjectReference &, const QString &);

public slots:
    void reload(int objectDebugId);     // set the root object
    void setCurrentObject(int debugId); // select an object in the tree

protected:
    virtual void mousePressEvent(QMouseEvent *);

private slots:
    void objectFetched();
    void currentItemChanged(QTreeWidgetItem *);
    void activated(QTreeWidgetItem *);

private:
    QTreeWidgetItem *findItemByObjectId(int debugId) const;
    QTreeWidgetItem *findItem(QTreeWidgetItem *item, int debugId) const;
    void dump(const QmlDebugContextReference &, int);
    void dump(const QmlDebugObjectReference &, int);
    void buildTree(const QmlDebugObjectReference &, QTreeWidgetItem *parent);

    QmlEngineDebug *m_client;
    QmlDebugObjectQuery *m_query;
};

QT_END_NAMESPACE


#endif
