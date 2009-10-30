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
#ifndef QMLINSPECTORRUNCONTROL_H
#define QMLINSPECTORRUNCONTROL_H

#include <QtCore/qobject.h>

#include <projectexplorer/runconfiguration.h>

#include "qmlinspector.h"

namespace ProjectExplorer {
    class ApplicationLauncher;
}

class QmlInspectorRunControlFactory : public ProjectExplorer::IRunControlFactory
{
    Q_OBJECT

public:
    explicit QmlInspectorRunControlFactory(QObject *parent);

    virtual bool canRun(
                const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration,
                const QString &mode) const;

    virtual ProjectExplorer::RunControl *create(
                const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration,
                const QString &mode);

    ProjectExplorer::RunControl *create(
                const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration,
                const QString &mode,
                const QmlInspector::StartParameters &sp);
                
    virtual QString displayName() const;

    virtual QWidget *configurationWidget(const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration);
};

class QmlInspectorRunControl : public ProjectExplorer::RunControl
{
    Q_OBJECT

public:
    explicit QmlInspectorRunControl(const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration,
                                    const QmlInspector::StartParameters &sp = QmlInspector::StartParameters());
    ~QmlInspectorRunControl();
    
    virtual void start();
    virtual void stop();
    virtual bool isRunning() const;

private slots:
    void appendOutput(const QString &s);
    void appStarted();
    void delayedStart();
    void viewerExited();
    void applicationError(const QString &error);

private:
    bool m_running;
    ProjectExplorer::ApplicationLauncher *m_viewerLauncher;
    QmlInspector::StartParameters m_startParams;
};

#endif
