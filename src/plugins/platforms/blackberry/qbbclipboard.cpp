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

// #define QBBCLIPBOARD_DEBUG

#ifndef QT_NO_CLIPBOARD

#include "qbbclipboard.h"
#include <QUrl>
#include <QStringList>
#include <QColor>
#include <QDebug>

#include <clipboard/clipboard.h>
#include <errno.h>

QT_BEGIN_NAMESPACE
static const char *typeList[] = {"text/html", "text/plain", "application/x-color"};

QBBClipboard::QBBClipboard()
{
    mMimeData = 0;
}

QBBClipboard::~QBBClipboard()
{
    delete mMimeData;
}

void QBBClipboard::setMimeData(QMimeData *data, QClipboard::Mode mode)
{
    if (mode != QClipboard::Clipboard)
        return;

    if (mMimeData != data) {
        delete mMimeData;
        mMimeData = data;
    }

    empty_clipboard();

    if (data == 0)
        return;

    QStringList format = data->formats();
    for (int i = 0; i < format.size(); ++i) {
        QString type = format.at(i);
        QByteArray buf = data->data(type);
        if (!buf.size())
            continue;

        int ret = set_clipboard_data(type.toUtf8().data(), buf.size(), buf.data());
#if defined(QBBCLIPBOARD_DEBUG)
        qDebug() << "QBB: set " << type.toUtf8().data() << "to clipboard, size=" << buf.size() << ";ret=" << ret;
#endif
    }
}

void QBBClipboard::readClipboardBuff(const char *type)
{
    char *pbuffer;
    if (is_clipboard_format_present(type) == 0) {
        int size = get_clipboard_data(type, &pbuffer);
        if (size != -1 && pbuffer) {
            QString qtype = type;
#if defined(QBBCLIPBOARD_DEBUG)
            qDebug() << "QBB: clipboard has " << qtype;
#endif
            mMimeData->setData(qtype, QByteArray(pbuffer, size));
            delete pbuffer;
        }
    }
}

QMimeData* QBBClipboard::mimeData(QClipboard::Mode mode)
{
    if (mode != QClipboard::Clipboard)
        return 0;

    if (!mMimeData)
        mMimeData = new QMimeData();

    mMimeData->clear();

    for (int i = 0; i < 3; i++)
        readClipboardBuff(typeList[i]);

    return mMimeData;
}

QT_END_NAMESPACE
#endif //QT_NO_CLIPBOAR
