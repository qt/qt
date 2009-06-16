/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>


#include <qpixmapcache.h>





//TESTED_CLASS=
//TESTED_FILES=

class tst_QPixmapCache : public QObject
{
    Q_OBJECT

public:
    tst_QPixmapCache();
    virtual ~tst_QPixmapCache();


public slots:
    void init();
private slots:
    void cacheLimit();
    void setCacheLimit();
    void find();
    void insert();
    void remove();
    void clear();
};


static int originalCacheLimit;

tst_QPixmapCache::tst_QPixmapCache()
{
    originalCacheLimit = QPixmapCache::cacheLimit();
}

tst_QPixmapCache::~tst_QPixmapCache()
{
}

void tst_QPixmapCache::init()
{
    QPixmapCache::setCacheLimit(originalCacheLimit);
    QPixmapCache::clear();
}

void tst_QPixmapCache::cacheLimit()
{
    // make sure the default is reasonable;
    // it was between 2048 and 10240 last time I looked at it
    QVERIFY(originalCacheLimit >= 1024 && originalCacheLimit <= 20480);

    QPixmapCache::setCacheLimit(100);
    QCOMPARE(QPixmapCache::cacheLimit(), 100);

    QPixmapCache::setCacheLimit(-50);
    QCOMPARE(QPixmapCache::cacheLimit(), -50);
}

void tst_QPixmapCache::setCacheLimit()
{
    QPixmap *p1 = new QPixmap(2, 3);
    QPixmapCache::insert("P1", *p1);
    QVERIFY(QPixmapCache::find("P1") != 0);
    delete p1;

    QPixmapCache::setCacheLimit(0);
    QVERIFY(QPixmapCache::find("P1") == 0);

    p1 = new QPixmap(2, 3);
    QPixmapCache::setCacheLimit(1000);
    QPixmapCache::insert("P1", *p1);
    QVERIFY(QPixmapCache::find("P1") != 0);

    delete p1;
}

void tst_QPixmapCache::find()
{
    QPixmap p1(10, 10);
    p1.fill(Qt::red);
    QPixmapCache::insert("P1", p1);

    QPixmap p2;
    QVERIFY(QPixmapCache::find("P1", p2));
    QCOMPARE(p2.width(), 10);
    QCOMPARE(p2.height(), 10);
    QCOMPARE(p1, p2);

    // obsolete
    QPixmap *p3 = QPixmapCache::find("P1");
    QVERIFY(p3);
    QCOMPARE(p1, *p3);
}

void tst_QPixmapCache::insert()
{
    QPixmap p1(10, 10);
    p1.fill(Qt::red);

    QPixmap p2(10, 10);
    p2.fill(Qt::yellow);

    // make sure it doesn't explode
    for (int i = 0; i < 20000; ++i)
        QPixmapCache::insert("0", p1);

    // ditto
    for (int j = 0; j < 20000; ++j)
        QPixmapCache::insert(QString::number(j), p1);

    int num = 0;
    for (int k = 0; k < 20000; ++k) {
        if (QPixmapCache::find(QString::number(k)))
            ++num;
    }

    int estimatedNum = (1024 * QPixmapCache::cacheLimit())
                       / ((p1.width() * p1.height() * p1.depth()) / 8);
    QVERIFY(estimatedNum - 1 <= num <= estimatedNum + 1);
    QPixmap p3;
    QPixmapCache::insert("null", p3);

    QPixmap c1(10, 10);
    c1.fill(Qt::yellow);
    QPixmapCache::insert("custom", c1);
    QVERIFY(!c1.isDetached());
    QPixmap c2(10, 10);
    c2.fill(Qt::red);
    QPixmapCache::insert("custom", c2);
    //We have deleted the old pixmap in the cache for the same key
    QVERIFY(c1.isDetached());
}

void tst_QPixmapCache::remove()
{
    QPixmap p1(10, 10);
    p1.fill(Qt::red);

    QPixmapCache::insert("red", p1);
    p1.fill(Qt::yellow);

    QPixmap p2;
    QVERIFY(QPixmapCache::find("red", p2));
    QVERIFY(p1.toImage() != p2.toImage());
    QVERIFY(p1.toImage() == p1.toImage()); // sanity check

    QPixmapCache::remove("red");
    QVERIFY(QPixmapCache::find("red") == 0);
    QPixmapCache::remove("red");
    QVERIFY(QPixmapCache::find("red") == 0);

    QPixmapCache::remove("green");
    QVERIFY(QPixmapCache::find("green") == 0);
}

void tst_QPixmapCache::clear()
{
    QPixmap p1(10, 10);
    p1.fill(Qt::red);

    for (int i = 0; i < 20000; ++i) {
        QVERIFY(QPixmapCache::find("x" + QString::number(i)) == 0);
    }
    for (int j = 0; j < 20000; ++j) {
        QPixmapCache::insert(QString::number(j), p1);
    }

    int num = 0;
    for (int k = 0; k < 20000; ++k) {
        if (QPixmapCache::find(QString::number(k), p1))
            ++num;
    }
    QVERIFY(num > 0);

    QPixmapCache::clear();

    for (int k = 0; k < 20000; ++k) {
        QVERIFY(QPixmapCache::find(QString::number(k)) == 0);
    }
}

QTEST_MAIN(tst_QPixmapCache)
#include "tst_qpixmapcache.moc"
