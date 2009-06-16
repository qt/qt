/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "linuxismousehandler.h"

#include <QFile>
#include <QTextStream>
#include <QScreen>
#include <QSocketNotifier>
#include <QDebug>

#include <qdebug.h>

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#include <linux/input.h>


#if 1
#define qLog(x) qDebug()
#else
#define qLog(x) while (0) qDebug()
#endif


// sanity check values of the range of possible mouse positions
#define MOUSE_SAMPLE_MIN    0
#define MOUSE_SAMPLE_MAX    2000

LinuxInputSubsystemMouseHandler::LinuxInputSubsystemMouseHandler(const QString &device)
    : mouseX(0), mouseY(0), mouseBtn(0), mouseIdx(0)
{
    qLog(Input) << "Loaded LinuxInputSubsystem touchscreen plugin!";
    setObjectName("LinuxInputSubsystem Mouse Handler");
    mouseFd = ::open(device.toLocal8Bit().constData(), O_RDONLY | O_NDELAY);
    // mouseFd = ::open(device.toLocal8Bit().constData(), O_RDONLY);
    if (mouseFd >= 0) {
        qLog(Input) << "Opened" << device << "as touchscreen input";
        m_notify = new QSocketNotifier(mouseFd, QSocketNotifier::Read, this);
        connect(m_notify, SIGNAL(activated(int)), this, SLOT(readMouseData()));
    } else {
        qWarning("Cannot open %s for touchscreen input (%s)",
                 device.toLocal8Bit().constData(), strerror(errno));
        return;
    }
}

LinuxInputSubsystemMouseHandler::~LinuxInputSubsystemMouseHandler()
{
    if (mouseFd >= 0)
        ::close(mouseFd);
}

void LinuxInputSubsystemMouseHandler::suspend()
{
    m_notify->setEnabled( false );
}

void LinuxInputSubsystemMouseHandler::resume()
{
    m_notify->setEnabled( true );
}

void LinuxInputSubsystemMouseHandler::readMouseData()
{
    if (!qt_screen)
        return;

    int n;

    do {
        n = read(mouseFd, mouseBuf + mouseIdx, mouseBufSize - mouseIdx);
        if (n > 0)
            mouseIdx += n;

        struct input_event *data;
        int idx = 0;

        while (mouseIdx-idx >= (int)sizeof(struct input_event)) {
            uchar *mb = mouseBuf + idx;
            data = (struct input_event *) mb;
	    // qLog(Input) << "mouse event type =" << data->type << "code =" << data->code << "value =" << data->value;
	    bool unknown = false;
	    if (data->type == EV_ABS) {
		if (data->code == ABS_X) {
                    //qLog(Input) << "\tABS_X" << data->value;
		    mouseX = data->value;
		} else if (data->code == ABS_Y) {
                    //qLog(Input) << "\tABS_Y" << data->value;
		    mouseY = data->value;
		} else {
		    unknown = true;
		}
	    } else if (data->type == EV_REL) {
                //qLog(Input) << "\tEV_REL" << hex << data->code << dec << data->value;
                if (data->code == REL_X) {
                    mouseX += data->value;
                } else if (data->code == REL_Y) {
                    mouseY += data->value;
                } else {
                    unknown = true;
                }
	    } else if (data->type == EV_KEY && data->code == BTN_TOUCH) {
                qLog(Input) << "\tBTN_TOUCH" << data->value;
		mouseBtn = data->value ? Qt::LeftButton : 0;
	    } else if (data->type == EV_KEY) {
                int button = 0;
                switch (data->code) {
                case BTN_LEFT: button = Qt::LeftButton; break;
                case BTN_MIDDLE: button = Qt::MidButton; break;
                case BTN_RIGHT: button = Qt::RightButton; break;
                }
                if (data->value)
                    mouseBtn |= button;
                else
                    mouseBtn &= ~button;
	    } else if (data->type == EV_SYN && data->code == SYN_REPORT) {
		QPoint pos( mouseX, mouseY );
                oldmouse = transform( pos );
                //qLog(Input) << "\tSYN_REPORT" << mouseBtn << pos << oldmouse;
                emit mouseChanged(oldmouse, mouseBtn);

	    } else {
		unknown = true;
	    }
	    if (unknown) {
		qWarning("unknown mouse event type=%x, code=%x, value=%x", data->type, data->code, data->value);
	    }
	    idx += sizeof(struct input_event);
        }
        int surplus = mouseIdx - idx;
        for (int i = 0; i < surplus; i++)
            mouseBuf[i] = mouseBuf[idx+i];
        mouseIdx = surplus;
    } while (n > 0);
}

