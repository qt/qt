/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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

#include "qmlerror.h"
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

class QmlErrorPrivate
{
public:
    QmlErrorPrivate();

    QUrl url;
    QString description;
    int line;
    int column;
};

QmlErrorPrivate::QmlErrorPrivate()
: line(-1), column(-1)
{
}

QmlError::QmlError()
: d(new QmlErrorPrivate)
{
}

QmlError::QmlError(const QmlError &other)
: d(new QmlErrorPrivate)
{
    *this = other;
}

QmlError &QmlError::operator=(const QmlError &other)
{
    d->url = other.d->url;
    d->description = other.d->description;
    d->line = other.d->line;
    d->column = other.d->column;
    return *this;
}

QmlError::~QmlError()
{
    delete d; d = 0;
}

QUrl QmlError::url() const
{
    return d->url;
}

void QmlError::setUrl(const QUrl &url)
{
    d->url = url;
}

QString QmlError::description() const
{
    return d->description;
}

void QmlError::setDescription(const QString &description)
{
    d->description = description;
}

int QmlError::line() const
{
    return d->line;
}

void QmlError::setLine(int line)
{
    d->line = line;
}

int QmlError::column() const
{
    return d->column;
}

void QmlError::setColumn(int column)
{
    d->column = column;
}

QDebug operator<<(QDebug debug, const QmlError &error)
{
    QUrl url = error.url();

    QString output;

    output = url.toString() + QLatin1String(":") + 
             QString::number(error.line());

    if(error.column() != -1) 
        output += QLatin1String(":") + QString::number(error.column());

    output += QLatin1String(": ") + error.description();

    debug << qPrintable(output) << "\n";

    if (error.line() > 0 && error.column() > 0 && 
        url.scheme() == QLatin1String("file")) {
        QString file = url.toLocalFile();
        QFile f(file);
        if (f.open(QIODevice::ReadOnly)) {
            QByteArray data = f.readAll();
            QTextStream stream(data, QIODevice::ReadOnly);
            const QString code = stream.readAll();
            const QStringList lines = code.split(QLatin1Char('\n'));

            if (lines.count() >= error.line()) {
                const QString &line = lines.at(error.line() - 1);
                debug << qPrintable(line) << "\n";

                int column = qMax(0, error.column() - 1);
                column = qMin(column, line.length()); 

                QByteArray ind;
                ind.reserve(column);
                for (int i = 0; i < column; ++i) {
                    const QChar ch = line.at(i);
                    if (ch.isSpace())
                        ind.append(ch.unicode());
                    else
                        ind.append(' ');
                }
                ind.append('^');
                debug << ind.constData();
            }
        }
    }
    return debug;
}

QT_END_NAMESPACE
