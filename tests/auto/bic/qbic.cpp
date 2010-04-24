/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include "qbic.h"

#include "QtCore/qfile.h"
#include "QtCore/qdebug.h"

void QBic::addBlacklistedClass(const QString &wildcard)
{
    blackList.append(QRegExp(wildcard, Qt::CaseSensitive, QRegExp::Wildcard));
}

void QBic::removeBlacklistedClass(const QString &wildcard)
{
    blackList.removeAll(QRegExp(wildcard, Qt::CaseSensitive, QRegExp::Wildcard));
}

bool QBic::isBlacklisted(const QString &className) const
{
    // all templates are blacklisted
    if (className.contains('<'))
        return true;

    for (int i = 0; i < blackList.count(); ++i)
        if (blackList.at(i).exactMatch(className))
            return true;
    return false;
}

static QStringList normalizedVTable(const QStringList &entry)
{
    QStringList normalized;

    for (int i = 2; i < entry.count(); ++i) {
        const QString line = entry.at(i).simplified();
        bool isOk = false;
        int num = line.left(line.indexOf(QLatin1Char(' '))).toInt(&isOk);
        if (!isOk) {
            qWarning("unrecognized line: %s", qPrintable(line));
            continue;
        }

        QString sym = line.mid(line.indexOf(QLatin1Char(' ')) + 1);
        if (sym.startsWith(QLatin1Char('('))) {
            if (sym.endsWith(QLatin1Char(')'))) {
                sym = sym.mid(sym.lastIndexOf('(') + 1);
                sym.chop(1);
            } else {
                sym = sym.mid(sym.lastIndexOf(QLatin1Char(')')) + 1);
            }
        } else {
            sym = sym.left(sym.indexOf(QLatin1Char('(')));
        }

        if (sym.startsWith(QLatin1String("& ")))
            sym.remove(1, 1);

        if (sym.startsWith(QLatin1String("-0")) || sym.startsWith(QLatin1String("0"))) {
            if (sym.endsWith('u'))
                sym.chop(1);

            bool isOk = false;
            qint64 num = sym.toLongLong(&isOk, 16);
            if (!isOk) {
                qWarning("unrecognized token: %s", qPrintable(sym));
                continue;
            }
            if (sizeof(void*) == 4)
                sym = QString::number(int(num));
            else
                sym = QString::number(num);
        }

        normalized << QString::number(num) + QLatin1Char(' ') + sym;
    }

    return normalized;
}

QBic::Info QBic::parseOutput(const QByteArray &ba) const
{
    Info info;
    const QStringList source = QString::fromLatin1(ba).split("\n\n");

    foreach(QString str, source) {
        QStringList entry = str.split('\n');
        if (entry.count() < 2)
            continue;
        if (entry.at(0).startsWith("Class ")) {
            const QString className = entry.at(0).mid(6);
            if (isBlacklisted(className))
                continue;
            QRegExp rx("size=(\\d+)");
            if  (rx.indexIn(entry.at(1)) == -1) {
                qWarning("Could not parse class information for className %s", className.toLatin1().constData());
                continue;
            }
            info.classSizes[className] = rx.cap(1).toInt();
        } else if (entry.at(0).startsWith("Vtable for ")) {
            const QString className = entry.at(0).mid(11);
            if (isBlacklisted(className))
                continue;
            info.classVTables[className] = normalizedVTable(entry);
        }
    }

    return info;
}

QBic::Info QBic::parseFile(const QString &fileName) const
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return Info();

    QByteArray ba = f.readAll();
    f.close();

    return parseOutput(ba);
}

enum VTableDiffResult { Match, Mismatch, Reimp };
static VTableDiffResult diffVTableEntry(const QString &v1, const QString &v2)
{
    if (v1 == v2)
        return Match;
    if (v2.endsWith(QLatin1String("__cxa_pure_virtual")))
        return Reimp;
    if (!v1.contains(QLatin1String("::")) || !v2.contains(QLatin1String("::")))
        return Mismatch;

    const QString sym1 = v1.mid(v1.lastIndexOf(QLatin1String("::")) + 2);
    const QString sym2 = v2.mid(v2.lastIndexOf(QLatin1String("::")) + 2);

    if (sym1 == sym2)
        return Reimp;

    return Mismatch;
}

QBic::VTableDiff QBic::diffVTables(const Info &oldLib, const Info &newLib) const
{
    VTableDiff result;

    for (QHash<QString, QStringList>::const_iterator it = newLib.classVTables.constBegin();
            it != newLib.classVTables.constEnd(); ++it) {
        if (!oldLib.classVTables.contains(it.key())) {
            result.addedVTables.append(it.key());
            continue;
        }
        const QStringList oldVTable = oldLib.classVTables.value(it.key());
        const QStringList vTable = it.value();
        if (vTable.count() != oldVTable.count()) {
            result.modifiedVTables.append(QPair<QString, QString>(it.key(),
                        QLatin1String("size mismatch")));
            continue;
        }

        for (int i = 0; i < vTable.count(); ++i) {
            VTableDiffResult diffResult = diffVTableEntry(vTable.at(i), oldVTable.at(i));
            switch (diffResult) {
            case Match:
                // do nothing
                break;
            case Mismatch:
                result.modifiedVTables.append(QPair<QString, QString>(oldVTable.at(i),
                            vTable.at(i)));
                break;
            case Reimp:
                result.reimpMethods.append(QPair<QString, QString>(oldVTable.at(i), vTable.at(i)));
                break;
            }
        }
    }

    for (QHash<QString, QStringList>::const_iterator it = oldLib.classVTables.constBegin();
            it != oldLib.classVTables.constEnd(); ++it) {
        if (!newLib.classVTables.contains(it.key()))
            result.removedVTables.append(it.key());
    }

    return result;
}

QBic::SizeDiff QBic::diffSizes(const Info &oldLib, const Info &newLib) const
{
    QBic::SizeDiff result;

    for (QHash<QString, int>::const_iterator it = newLib.classSizes.constBegin();
            it != newLib.classSizes.constEnd(); ++it) {
        if (!oldLib.classSizes.contains(it.key())) {
            result.added.append(it.key());
            continue;
        }
        int oldSize = oldLib.classSizes.value(it.key());
        int newSize = it.value();

        if (oldSize != newSize)
            result.mismatch.append(it.key());
    }

    for (QHash<QString, int>::const_iterator it = oldLib.classSizes.constBegin();
            it != oldLib.classSizes.constEnd(); ++it) {
        if (!newLib.classSizes.contains(it.key()))
            result.removed.append(it.key());
    }

    return result;
}

