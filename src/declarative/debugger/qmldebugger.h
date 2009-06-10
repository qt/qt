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

#ifndef QMLDEBUGGER_H
#define QMLDEBUGGER_H

#include <QtCore/qpointer.h>
#include <QtCore/qset.h>
#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QTreeWidget;
class QTreeWidgetItem;
class QPlainTextEdit;
class QmlDebuggerItem;
class QTableView;
class QmlPropertyView;
class QmlWatches;
class QmlObjectTree;
class QmlContext;
class QSimpleCanvas;
class QmlDebugger : public QWidget
{
Q_OBJECT
public:
    QmlDebugger(QWidget *parent = 0);

    void setDebugObject(QObject *);
    void setCanvas(QSimpleCanvas *);

public slots:
    void refresh();

private slots:
    void itemClicked(QTreeWidgetItem *);
    void itemDoubleClicked(QTreeWidgetItem *);
    void highlightObject(quint32);
    void addWatch(QObject *, const QString &);

private:
    void buildTree(QObject *obj, QmlDebuggerItem *parent);
    bool makeItem(QObject *obj, QmlDebuggerItem *item);
    QmlObjectTree *m_tree;
    QTreeWidget *m_warnings;
    QTableView *m_watchTable;
    QmlWatches *m_watches;
    QmlPropertyView *m_properties;
    QPlainTextEdit *m_text;
    QPointer<QObject> m_object;
    QPointer<QObject> m_selectedItem;

    QTreeWidgetItem *m_highlightedItem;
    QHash<quint32, QTreeWidgetItem *> m_items;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLDEBUGGER_H

