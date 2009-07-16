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

/*!
    \class QmlError
    \brief The QmlError class encapsulates a QML error
*/
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

/*!
    Create an empty error object.
*/
QmlError::QmlError()
: d(new QmlErrorPrivate)
{
}

/*!
    Create a copy of \a other.
*/
QmlError::QmlError(const QmlError &other)
: d(new QmlErrorPrivate)
{
    *this = other;
}

/*!
    Assign \a other to this error object.
*/
QmlError &QmlError::operator=(const QmlError &other)
{
    d->url = other.d->url;
    d->description = other.d->description;
    d->line = other.d->line;
    d->column = other.d->column;
    return *this;
}

/*!
    \internal 
*/
QmlError::~QmlError()
{
    delete d; d = 0;
}

/*!
    Return the url for the file that caused this error.
*/
QUrl QmlError::url() const
{
    return d->url;
}

/*!
    Set the \a url for the file that caused this error.
*/
void QmlError::setUrl(const QUrl &url)
{
    d->url = url;
}

/*!
    Return the error description.
*/
QString QmlError::description() const
{
    return d->description;
}

/*!
    Set the error \a description.
*/
void QmlError::setDescription(const QString &description)
{
    d->description = description;
}

/*!
    Return the error line number.
*/
int QmlError::line() const
{
    return d->line;
}

/*!
    Set the error \a line number.
*/
void QmlError::setLine(int line)
{
    d->line = line;
}

/*!
    Return the error column number.
*/
int QmlError::column() const
{
    return d->column;
}

/*!
    Set the error \a column number.
*/
void QmlError::setColumn(int column)
{
    d->column = column;
}

/*!
    Return the error as a human readable string.
*/
QString QmlError::toString() const
{
    QString rv;
    rv = url().toString() + QLatin1String(":") + QString::number(line());
    if(column() != -1) 
        rv += QLatin1String(":") + QString::number(column());

    rv += QLatin1String(": ") + description();

    return rv;
}

/*!
    \relates QmlError
    \fn QDebug operator<<(QDebug debug, const QmlError &error)

    Output a human readable version of \a error to \a debug.
*/

QDebug operator<<(QDebug debug, const QmlError &error)
{
    debug << qPrintable(error.toString());

    QUrl url = error.url();

    if (error.line() > 0 && url.scheme() == QLatin1String("file")) {
        QString file = url.toLocalFile();
        QFile f(file);
        if (f.open(QIODevice::ReadOnly)) {
            QByteArray data = f.readAll();
            QTextStream stream(data, QIODevice::ReadOnly);
            const QString code = stream.readAll();
            const QStringList lines = code.split(QLatin1Char('\n'));

            if (lines.count() >= error.line()) {
                const QString &line = lines.at(error.line() - 1);
                debug << "\n    " << qPrintable(line);

                if(error.column() > 0) {
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
                    debug << "\n    " << ind.constData();
                }
            }
        }
    }
    return debug;
}

QT_END_NAMESPACE
