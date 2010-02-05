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
#ifndef EXPRESSIONQUERYWIDGET_H
#define EXPRESSIONQUERYWIDGET_H

#include <QWidget>

#include <private/qmldebug_p.h>

QT_BEGIN_NAMESPACE

class QGroupBox;
class QTextEdit;
class QLineEdit;
class QPushButton;

class ExpressionQueryWidget : public QWidget
{
    Q_OBJECT
public:
    enum Mode {
        SeparateEntryMode,
        ShellMode
    };

    ExpressionQueryWidget(Mode mode = SeparateEntryMode, QmlEngineDebug *client = 0, QWidget *parent = 0);
    
    void setEngineDebug(QmlEngineDebug *client);
    void clear();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void setCurrentObject(const QmlDebugObjectReference &obj);

private slots:
    void executeExpression();
    void showResult();

private:
    void appendPrompt();
    void checkCurrentContext();
    void showCurrentContext();
    void updateTitle();

    Mode m_mode;

    QmlEngineDebug *m_client;
    QmlDebugExpressionQuery *m_query;
    QTextEdit *m_textEdit;
    QLineEdit *m_lineEdit;
    QPushButton *m_button;
    QString m_prompt;
    QString m_expr;
    QString m_lastExpr;

    QString m_title;

    QmlDebugObjectReference m_currObject;
    QmlDebugObjectReference m_objectAtLastFocus;
};

QT_END_NAMESPACE

#endif

