/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtCore/QDirIterator>
#include <QtTest/QtTest>

/* We use Patternist, so without it, we won't build. */
#ifdef QTEST_XMLPATTERNS

#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/QXmlSerializer>
#include "../qxmlquery/TestFundament.h"

/*!
 \class tst_CheckXMLFiles
 \internal
 \since 4.4
 \brief Checks whether the XML files found in $QTDIR are well-formed.
 */
class tst_CheckXMLFiles : public QObject
                        , private TestFundament
{
    Q_OBJECT

private Q_SLOTS:
    void checkXMLFiles() const;
    void checkXMLFiles_data() const;
};

void tst_CheckXMLFiles::checkXMLFiles() const
{
    QFETCH(QString, file);

    QXmlQuery query;
    query.setQuery(QLatin1String("doc-available('") + inputFileAsURI(file).toString() + QLatin1String("')"));
    QVERIFY(query.isValid());

    /* We don't care about the result, we only want to ensure the files can be parsed. */
    QByteArray dummy;
    QBuffer buffer(&dummy);
    QVERIFY(buffer.open(QIODevice::WriteOnly));
    QXmlSerializer serializer(query, &buffer);

    /* This is the important one. */
    QVERIFY(query.evaluateTo(&serializer));
}

void tst_CheckXMLFiles::checkXMLFiles_data() const
{
    QTest::addColumn<QString>("file");

    QStringList patterns;
    /* List possible XML files in Qt. */
    patterns.append(QLatin1String("*.xml"));
    patterns.append(QLatin1String("*.gccxml"));
    patterns.append(QLatin1String("*.svg"));
    patterns.append(QLatin1String("*.ui"));
    patterns.append(QLatin1String("*.qrc"));
    patterns.append(QLatin1String("*.ts"));
    /* We don't do HTML files currently because so many of them in 3rd party are broken. */
    patterns.append(QLatin1String("*.xhtml"));

#ifndef Q_OS_WINCE
    QString path = QLatin1String("../../../");
#else
    QString path = QLatin1String("xmlfiles");
#endif
    QDirIterator it(inputFile(path), patterns, QDir::AllEntries, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        it.next();

        /* We got tons of broken XML files as part of auto tests. */
        if(!it.filePath().contains(QLatin1String("/tests/auto/")) &&
           !it.filePath().contains(QLatin1String("/tests/arthur/")))
            QTest::newRow(it.filePath().toUtf8().constData()) << it.filePath();
    }
}

QTEST_MAIN(tst_CheckXMLFiles)

#include "tst_checkxmlfiles.moc"
#else
QTEST_NOOP_MAIN
#endif

// vim: et:ts=4:sw=4:sts=4
