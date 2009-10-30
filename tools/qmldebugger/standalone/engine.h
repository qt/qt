/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef ENGINE_H
#define ENGINE_H

#include <QWidget>
#include <QtCore/qpointer.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlview.h>
#include <private/qmldebug_p.h>

QT_BEGIN_NAMESPACE

class ObjectPropertiesView;
class QmlDebugConnection;
class QmlDebugPropertyReference;
class QmlDebugWatch;
class ObjectTree;
class WatchTableModel;
class WatchTableView;
class ExpressionQueryWidget;

class QTabWidget;

class EnginePane : public QWidget
{
Q_OBJECT
public:
    EnginePane(QmlDebugConnection *, QWidget *parent = 0);

public slots:
    void refreshEngines();

private slots:
    void enginesChanged();

    void queryContext(int);
    void contextChanged();

    void engineSelected(int);

private:
    QmlEngineDebug *m_client;
    QmlDebugEnginesQuery *m_engines;
    QmlDebugRootContextQuery *m_context;

    ObjectTree *m_objTree;
    QTabWidget *m_tabs;
    WatchTableView *m_watchTableView;
    WatchTableModel *m_watchTableModel;
    ExpressionQueryWidget *m_exprQueryWidget;

    QmlView *m_engineView;
    QList<QObject *> m_engineItems;

    ObjectPropertiesView *m_propertiesView;
};

QT_END_NAMESPACE

#endif // ENGINE_H

