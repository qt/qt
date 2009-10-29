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
#ifndef QMLINSPECTORMODE_H
#define QMLINSPECTORMODE_H

#include <QAction>

#include <coreplugin/basemode.h>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QToolButton;
class QLineEdit;
class QSpinBox;
class QLabel;

class QmlEngineDebug;
class QmlDebugConnection;
class QmlDebugEnginesQuery;
class QmlDebugRootContextQuery;
class ObjectTree;
class WatchTableModel;
class WatchTableView;
class ObjectPropertiesView;
class CanvasFrameRate;
class ExpressionQueryWidget;
class EngineSpinBox;
    

class QmlInspectorMode : public Core::BaseMode
{
    Q_OBJECT

public:
    QmlInspectorMode(QObject *parent = 0);
    
    
    quint16 viewerPort() const;
    
signals:
    void startViewer();
    void stopViewer();
    void statusMessage(const QString &text);
    
public slots: 
    void connectToViewer(); // using host, port from widgets
    void disconnectFromViewer(); 

private slots:
    void connectionStateChanged();
    void connectionError();
    void reloadEngines();
    void enginesChanged();
    void queryEngineContext(int);
    void contextChanged();

private:
    struct Actions {
        QAction *startAction;
        QAction *stopAction;
    };
    
    void initActions();
    QWidget *createModeWindow();
    QWidget *createMainView();
    void initWidgets();
    QWidget *createBottomWindow();
    QToolButton *createToolButton(QAction *action);
    
    Actions m_actions;
    
    QmlDebugConnection *m_conn;
    QmlEngineDebug *m_client;

    QmlDebugEnginesQuery *m_engineQuery;
    QmlDebugRootContextQuery *m_contextQuery;
    
    ObjectTree *m_objectTreeWidget;
    ObjectPropertiesView *m_propertiesWidget;
    WatchTableModel *m_watchTableModel;
    WatchTableView *m_watchTableView;
    CanvasFrameRate *m_frameRateWidget;
    ExpressionQueryWidget *m_expressionWidget;

    QLineEdit *m_addressEdit;
    QSpinBox *m_portSpinBox;
    EngineSpinBox *m_engineSpinBox;
};

QT_END_NAMESPACE

#endif
