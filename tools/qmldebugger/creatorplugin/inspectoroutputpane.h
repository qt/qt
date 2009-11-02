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
#ifndef INSPECTOROUTPUTPANE_H
#define INSPECTOROUTPUTPANE_H 

#include <QtCore/QObject>

#include <coreplugin/ioutputpane.h>


QT_BEGIN_NAMESPACE

class QTextEdit;

class RunControl;

class InspectorOutputPane : public Core::IOutputPane
{
    Q_OBJECT
public:
    InspectorOutputPane(QObject *parent = 0);
    virtual ~InspectorOutputPane();

    virtual QWidget *outputWidget(QWidget *parent);
    virtual QList<QWidget*> toolBarWidgets() const;
    virtual QString name() const;

    virtual int priorityInStatusBar() const;

    virtual void clearContents();
    virtual void visibilityChanged(bool visible);

    virtual void setFocus();
    virtual bool hasFocus();
    virtual bool canFocus();

    virtual bool canNavigate();
    virtual bool canNext();
    virtual bool canPrevious();
    virtual void goToNext();
    virtual void goToPrev();

public slots:
    void addOutput(RunControl *, const QString &text);
    void addOutputInline(RunControl *, const QString &text);

    void addErrorOutput(RunControl *, const QString &text);
    void addInspectorStatus(const QString &text);

private:
    QTextEdit *m_textEdit;
};

QT_END_NAMESPACE

#endif

