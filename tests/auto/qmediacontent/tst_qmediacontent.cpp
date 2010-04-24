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

#include <QtTest/QtTest>

#include <QtMediaServices/qmediacontent.h>


class tst_QMediaContent : public QObject
{
    Q_OBJECT

private slots:
    void testNull();
    void testUrlCtor();
    void testRequestCtor();
    void testResourceCtor();
    void testResourceListCtor();
    void testCopy();
    void testAssignment();
    void testEquality();
    void testResources();
};

void tst_QMediaContent::testNull()
{
    QMediaContent media;

    QCOMPARE(media.isNull(), true);
    QCOMPARE(media.canonicalUrl(), QUrl());
    QCOMPARE(media.canonicalResource(), QMediaResource());
    QCOMPARE(media.resources(), QMediaResourceList());
}

void tst_QMediaContent::testUrlCtor()
{
    QMediaContent media(QUrl("http://example.com/movie.mov"));

    QCOMPARE(media.canonicalUrl(), QUrl("http://example.com/movie.mov"));
    QCOMPARE(media.canonicalResource().url(), QUrl("http://example.com/movie.mov"));
}

void tst_QMediaContent::testRequestCtor()
{
    QNetworkRequest request(QUrl("http://example.com/movie.mov"));
    request.setAttribute(QNetworkRequest::User, QVariant(1234));

    QMediaContent media(request);

    QCOMPARE(media.canonicalRequest(), request);
    QCOMPARE(media.canonicalUrl(), QUrl("http://example.com/movie.mov"));
    QCOMPARE(media.canonicalResource().request(), request);
    QCOMPARE(media.canonicalResource().url(), QUrl("http://example.com/movie.mov"));
}

void tst_QMediaContent::testResourceCtor()
{
    QMediaContent media(QMediaResource(QUrl("http://example.com/movie.mov")));

    QCOMPARE(media.canonicalResource(), QMediaResource(QUrl("http://example.com/movie.mov")));
}

void tst_QMediaContent::testResourceListCtor()
{
    QMediaResourceList  resourceList;
    resourceList << QMediaResource(QUrl("http://example.com/movie.mov"));

    QMediaContent media(resourceList);

    QCOMPARE(media.canonicalUrl(), QUrl("http://example.com/movie.mov"));
    QCOMPARE(media.canonicalResource().url(), QUrl("http://example.com/movie.mov"));
}

void tst_QMediaContent::testCopy()
{
    QMediaContent media1(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaContent media2(media1);

    QVERIFY(media1 == media2);
}

void tst_QMediaContent::testAssignment()
{
    QMediaContent media1(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaContent media2;
    QMediaContent media3;

    media2 = media1;
    QVERIFY(media2 == media1);

    media2 = media3;
    QVERIFY(media2 == media3);
}

void tst_QMediaContent::testEquality()
{
    QMediaContent media1;
    QMediaContent media2;
    QMediaContent media3(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaContent media4(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaContent media5(QMediaResource(QUrl("file:///some/where/over/the/rainbow.mp3")));

    // null == null
    QCOMPARE(media1 == media2, true);
    QCOMPARE(media1 != media2, false);

    // null != something
    QCOMPARE(media1 == media3, false);
    QCOMPARE(media1 != media3, true);

    // equiv
    QCOMPARE(media3 == media4, true);
    QCOMPARE(media3 != media4, false);

    // not equiv
    QCOMPARE(media4 == media5, false);
    QCOMPARE(media4 != media5, true);
}

void tst_QMediaContent::testResources()
{
    QMediaResourceList  resourceList;

    resourceList << QMediaResource(QUrl("http://example.com/movie-main.mov"));
    resourceList << QMediaResource(QUrl("http://example.com/movie-big.mov"));
    QMediaContent    media(resourceList);

    QMediaResourceList  res = media.resources();
    QCOMPARE(res.size(), 2);
    QCOMPARE(res[0], QMediaResource(QUrl("http://example.com/movie-main.mov")));
    QCOMPARE(res[1], QMediaResource(QUrl("http://example.com/movie-big.mov")));
}

QTEST_MAIN(tst_QMediaContent)

#include "tst_qmediacontent.moc"
