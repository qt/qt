/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QGSTREAMERVIDEOOUTPUTCONTROL_H
#define QGSTREAMERVIDEOOUTPUTCONTROL_H

#include <QtMediaServices/qvideooutputcontrol.h>

#include <gst/gst.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGstreamerVideoRendererInterface
{
public:
    virtual ~QGstreamerVideoRendererInterface();
    virtual GstElement *videoSink() = 0;
    virtual void precessNewStream() {}
};

class QGstreamerVideoOutputControl : public QVideoOutputControl
{
    Q_OBJECT
public:
    QGstreamerVideoOutputControl(QObject *parent = 0);

    QList<Output> availableOutputs() const;
    void setAvailableOutputs(const QList<Output> &outputs);

    Output output() const;
    void setOutput(Output output);

Q_SIGNALS:
    void outputChanged(QVideoOutputControl::Output output);

private:
    QList<Output> m_outputs;
    Output m_output;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
