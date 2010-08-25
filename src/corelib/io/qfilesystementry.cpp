/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qfilesystementry_p.h"
#include "qdir.h"

QFileSystemEntry::QFileSystemEntry(const QString &filePath)
    : m_filePath(filePath),
    m_lastSeparator(-2),
    m_firstDotInFileName(-2),
    m_lastDotInFileName(0)
{
}

QFileSystemEntry::QFileSystemEntry(const QByteArray &nativeFilePath)
    : m_nativeFilePath(nativeFilePath),
    m_lastSeparator(-2),
    m_firstDotInFileName(-2),
    m_lastDotInFileName(0)
{
}

QFileSystemEntry::QFileSystemEntry(const QByteArray &nativeFilePath, const QString &filePath)
    : m_filePath(filePath),
    m_nativeFilePath(nativeFilePath),
    m_lastSeparator(-2),
    m_firstDotInFileName(-2),
    m_lastDotInFileName(0)
{
}

QString QFileSystemEntry::filePath() const
{
    resolveFilePath();
    return m_filePath;
}

QByteArray QFileSystemEntry::nativeFilePath() const
{
    resolveNativeFilePath();
    return m_nativeFilePath;
}

void QFileSystemEntry::resolveFilePath() const
{
    if (m_filePath.isEmpty() && !m_nativeFilePath.isEmpty()) {
        m_filePath = QDir::fromNativeSeparators(QString::fromLocal8Bit(m_nativeFilePath));
    }
}

void QFileSystemEntry::resolveNativeFilePath() const
{
    if (!m_filePath.isEmpty() && m_nativeFilePath.isEmpty()) {
        m_nativeFilePath = QDir::toNativeSeparators(m_filePath).toLocal8Bit();
    }
}

QString QFileSystemEntry::fileName() const
{
    findLastSeparator();
    return m_filePath.mid(m_lastSeparator + 1);
}

QString QFileSystemEntry::suffix() const
{
    findFileNameSeparators();

    if (m_lastDotInFileName == -1)
        return QString();

    return m_filePath.mid(m_lastSeparator + m_firstDotInFileName + m_lastDotInFileName + 1);
}

QString QFileSystemEntry::completeSuffix() const
{
    findFileNameSeparators();
    if (m_firstDotInFileName == -1)
        return QString();

    return m_filePath.mid(m_lastSeparator + m_firstDotInFileName + 1);
}

bool QFileSystemEntry::isAbsolute() const
{
    resolveFilePath();
    return (!m_filePath.isEmpty() && (m_filePath[0].unicode() == '/')
#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
        || (m_filePath.length() >= 2
                && ((m_filePath[0].isLetter() && m_filePath[1] == QLatin1Char(':'))
                    || (m_filePath.at(0) == QLatin1Char('/') && m_filePath.at(1) == QLatin1Char('/'))))
#endif
    );

}

// private methods

void QFileSystemEntry::findLastSeparator() const
{
    if (m_lastSeparator == -2) {
        resolveFilePath();
        m_lastSeparator = -1;
        for (int i = m_filePath.size() - 1; i >= 0; --i) {
            if (m_filePath[i].unicode() == '/') {
                m_lastSeparator = i;
                break;
            }
        }
    }
}

void QFileSystemEntry::findFileNameSeparators() const
{
    if (m_firstDotInFileName == -2) {
        resolveFilePath();
        int firstDotInFileName = -1;
        int lastDotInFileName = -1;
        int lastSeparator = m_lastSeparator;

        int stop;
        if (lastSeparator < 0) {
            lastSeparator = -1;
            stop = 0;
        } else {
            stop = lastSeparator;
        }

        int i = m_filePath.size() - 1;
        for (; i >= stop; --i) {
            if (m_filePath[i].unicode() == '.') {
                firstDotInFileName = lastDotInFileName = i;
                break;
            } else if (m_filePath[i].unicode() == '/') {
                lastSeparator = i;
                break;
            }
        }

        if (lastSeparator != i) {
            for (--i; i >= stop; --i) {
                if (m_filePath[i].unicode() == '.')
                    firstDotInFileName = i;
                else if (m_filePath[i].unicode() == '/') {
                    lastSeparator = i;
                    break;
                }
            }
        }

        m_lastSeparator = lastSeparator;
        m_firstDotInFileName = firstDotInFileName == -1 ? -1 : firstDotInFileName - lastSeparator;
        if (lastDotInFileName == -1)
            m_lastDotInFileName = -1;
        else if (firstDotInFileName == lastDotInFileName)
            m_lastDotInFileName = 0;
        else
            m_lastDotInFileName = firstDotInFileName - lastSeparator;
    }
}
