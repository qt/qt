/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

// #define QBBINPUTCONTEXT_DEBUG

#include <qbbinputcontext.h>
#include <qbbabstractvirtualkeyboard.h>

#include <QDebug>
#include <QAbstractSpinBox>

QT_BEGIN_NAMESPACE

QBBInputContext::QBBInputContext(QBBAbstractVirtualKeyboard &keyboard, QObject* parent)
    : QInputContext(parent),
      mVirtualKeyboard(keyboard)
{
}

QBBInputContext::~QBBInputContext()
{
}

QString QBBInputContext::language()
{
    // Once we enable full IMF support, we need to hook that up here.
    return mVirtualKeyboard.languageId();
}

bool QBBInputContext::hasPhysicalKeyboard()
{
    // TODO: This should query the system to check if a USB keyboard is connected.
    return false;
}

void QBBInputContext::reset()
{
}

bool QBBInputContext::filterEvent( const QEvent *event )
{
    if (hasPhysicalKeyboard())
        return false;

    if (event->type() == QEvent::CloseSoftwareInputPanel) {
        mVirtualKeyboard.hideKeyboard();
#if defined(QBBINPUTCONTEXT_DEBUG)
        qDebug() << "QBB: hiding virtual keyboard";
#endif
        return false;
    }

    if (event->type() == QEvent::RequestSoftwareInputPanel) {
        mVirtualKeyboard.showKeyboard();
#if defined(QBBINPUTCONTEXT_DEBUG)
        qDebug() << "QBB: requesting virtual keyboard";
#endif
        return false;
    }

    return false;

}

bool QBBInputContext::handleKeyboardEvent(int flags, int sym, int mod, int scan, int cap)
{
    return false;
}

void QBBInputContext::setFocusWidget(QWidget *w)
{
#if defined(QBBINPUTCONTEXT_DEBUG)
    qDebug() << Q_FUNC_INFO << (w ? "requesting" : "hiding") << "virtual keyboard";
#endif
    QInputContext::setFocusWidget(w);

    if (w) {
        if (qobject_cast<QAbstractSpinBox*>(w))
            mVirtualKeyboard.setKeyboardMode(QBBAbstractVirtualKeyboard::Phone);
        else
            mVirtualKeyboard.setKeyboardMode(QBBAbstractVirtualKeyboard::Default);

        mVirtualKeyboard.showKeyboard();
    } else {
        mVirtualKeyboard.hideKeyboard();
    }
}

QT_END_NAMESPACE
