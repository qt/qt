/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QCoreApplication>
#include <QtCore/QRegExp>
#include <QtCore/QMutexLocker>
#include <QtGui/QTextDocument>

#include "qhelp_global.h"

QString QHelpGlobal::uniquifyConnectionName(const QString &name, void *pointer)
{
    static int counter = 0;
    static QMutex mutex;

    QMutexLocker locker(&mutex);
    if (++counter > 1000)
        counter = 0;

    return QString::fromLatin1("%1-%2-%3").
        arg(name).arg(quintptr(pointer)).arg(counter);
}

QString QHelpGlobal::documentTitle(const QString &content)
{
    QString title = QCoreApplication::translate("QHelp", "Untitled");
    if (!content.isEmpty()) {
        int start = content.indexOf(QLatin1String("<title>"), 0, Qt::CaseInsensitive) + 7;
        int end = content.indexOf(QLatin1String("</title>"), 0, Qt::CaseInsensitive);
        if ((end - start) > 0) {
            title = content.mid(start, end - start);
            if (Qt::mightBeRichText(title) || title.contains(QLatin1Char('&'))) {
                QTextDocument doc;
                doc.setHtml(title);
                title = doc.toPlainText();
            }
        }
    }
    return title;
}

QString QHelpGlobal::codecFromData(const QByteArray &data)
{
    QString codec = codecFromXmlData(data);
    if (codec.isEmpty())
        codec = codecFromHtmlData(data);
    return codec.isEmpty() ? QLatin1String("utf-8") : codec;
}

QString QHelpGlobal::codecFromHtmlData(const QByteArray &data)
{
    QString head = QString::fromUtf8(data.constData(), qMin(1000, data.size()));
    int start = head.indexOf(QLatin1String("<meta"), 0, Qt::CaseInsensitive);
    if (start > 0) {
        QRegExp r(QLatin1String("charset=([^\"\\s]+)"));
        while (start != -1) {
            const int end = head.indexOf(QLatin1Char('>'), start) + 1;
            if (end <= start)
                break;
            const QString &meta = head.mid(start, end - start).toLower();
            if (r.indexIn(meta) != -1)
                return r.cap(1);
            start = head.indexOf(QLatin1String("<meta"), end,
                                    Qt::CaseInsensitive);
        }
    }
    return QString();
}

QString QHelpGlobal::codecFromXmlData(const QByteArray &data)
{
    QString head = QString::fromUtf8(data.constData(), qMin(1000, data.size()));
    const QRegExp encodingExp(QLatin1String("^\\s*<\\?xml version="
        "\"\\d\\.\\d\" encoding=\"([^\"]+)\"\\?>.*"));
    return encodingExp.exactMatch(head) ? encodingExp.cap(1) : QString();
}
