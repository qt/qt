/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "linuxiskbdhandler.h"

#include <QSocketNotifier>

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <linux/input.h>
#include <linux/kd.h>

#include <qdebug.h>
#if 1
#define qLog(x) qDebug()
#else
#define qLog(x) while (0) qDebug()
#endif

struct LinuxInputSubsystemKbdHandler::keytable_s LinuxInputSubsystemKbdHandler::keytable[] = {
    { KEY_PAGEDOWN,	0xffff, Qt::Key_Context1 },
    { KEY_END,		0xffff, Qt::Key_Back },
    { KEY_RIGHTCTRL,	0xffff, Qt::Key_Home },
    { KEY_SPACE,	0xffff, Qt::Key_Menu },
    { KEY_ENTER,	0xffff, Qt::Key_Select },
    { KEY_UP,		0xffff, Qt::Key_Up },
    { KEY_LEFT,		0xffff, Qt::Key_Left },
    { KEY_RIGHT,	0xffff, Qt::Key_Right },
    { KEY_DOWN,		0xffff, Qt::Key_Down },
    { KEY_POWER,	0xffff, Qt::Key_Call },
    { KEY_BACKSPACE,	0xffff, Qt::Key_Backspace },
    { KEY_F1,		0xffff, Qt::Key_Hangup },
    { KEY_KP1,		'1',	Qt::Key_1 },
    { KEY_KP2,		'2',	Qt::Key_2 },
    { KEY_KP3,		'3',	Qt::Key_3 },
    { KEY_KP4,		'4',	Qt::Key_4 },
    { KEY_KP5,		'5',	Qt::Key_5 },
    { KEY_KP6,		'6',	Qt::Key_6 },
    { KEY_KP7,		'7',	Qt::Key_7 },
    { KEY_KP8,		'8',	Qt::Key_8 },
    { KEY_KP9,		'9',	Qt::Key_9 },
    { KEY_KP0,		'0',	Qt::Key_0 },
    { KEY_APOSTROPHE,	'*',	Qt::Key_Asterisk },
    { KEY_3,		'#',	Qt::Key_NumberSign },
    { KEY_F2,		0xffff, Qt::Key_F2 },
    { KEY_F3,		0xffff, Qt::Key_F3 },
    { KEY_F4,		0xffff, Qt::Key_F4 },
    { KEY_F5,		0xffff, Qt::Key_F5 },
    { KEY_F6,		0xffff, Qt::Key_F6 },
    { KEY_F7,		0xffff, Qt::Key_VolumeUp },
    { KEY_F8,		0xffff, Qt::Key_VolumeDown },
    { KEY_F9,		0xffff, Qt::Key_F9 },
    { 0,		0,	Qt::Key_unknown },
};

struct LinuxInputSubsystemKbdHandler::keymap_s LinuxInputSubsystemKbdHandler::keymap[KEY_MAX];

LinuxInputSubsystemKbdHandler::LinuxInputSubsystemKbdHandler(const QString &device)
{
    qLog(Input) << "Loaded LinuxInputSubsystem keypad plugin!";
    setObjectName( "LinuxInputSubsystem Keypad Handler" );
    kbdFD = ::open(device.toLocal8Bit().constData(), O_RDONLY, 0);
    if (kbdFD >= 0) {
	qLog(Input) << "Opened" << device << "as keypad input";
#if 0
	struct kbd_repeat kbdrep;
	kbdrep.delay =  500; /* ms */
	kbdrep.period = 250; /* ms */
	ioctl(kbdFD, KDKBDREP, &kbdrep);
#endif
	m_notify = new QSocketNotifier( kbdFD, QSocketNotifier::Read, this );
	connect( m_notify, SIGNAL(activated(int)), this, SLOT(readKbdData()));
    } else {
	qWarning("Cannot open '%s' for keypad (%s)",
		device.toLocal8Bit().constData(), strerror(errno));
	return;
    }
    shift = false;

    initmap();
}

LinuxInputSubsystemKbdHandler::~LinuxInputSubsystemKbdHandler()
{
}

void LinuxInputSubsystemKbdHandler::initmap()
{
    for (int i = 0; i < KEY_MAX; i++) {
	keymap[i].unicode = 0xffff;
	keymap[i].keycode = Qt::Key_unknown;
    }
    for (int i = 0; keytable[i].unicode; i++) {
	int idx = keytable[i].code;
	keymap[idx].unicode = keytable[i].unicode;
	keymap[idx].keycode = keytable[i].keycode;
    }
}

void LinuxInputSubsystemKbdHandler::readKbdData()
{
    struct input_event *ie;
    struct input_event iebuf[32];

    uint n = ::read(kbdFD, iebuf, sizeof(iebuf));

    bool pressed;
    bool autorepeat;
    int modifiers = 0;
    int unicode, keycode;

    n /= sizeof(struct input_event);
    ie = iebuf;
    for (uint i = 0; i < n; i++) {

	pressed = ie->value != 0;
	autorepeat = ie->value == 2;
        qLog() << "keyEvent" << hex << ie->type << ie->code << ie->value;
	unicode = keymap[ie->code].unicode;
	keycode = keymap[ie->code].keycode;

	processKeyEvent(unicode, keycode, (Qt::KeyboardModifiers)modifiers,
	    pressed, autorepeat);

	ie++;
    }

}

