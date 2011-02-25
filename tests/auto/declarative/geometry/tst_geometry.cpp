/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt scene graph research project.
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

#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qsggeometry.h>

class GeometryTest : public QObject
{
    Q_OBJECT

public:

private Q_SLOTS:
    void testPoint2D();
    void testTexturedPoint2D();
    void testCustomGeometry();

private:
};

void GeometryTest::testPoint2D()
{
    QSGGeometry geometry(QSGGeometry::defaultAttributes_Point2D(), 4, 0);

    QCOMPARE(geometry.attributeCount(), 1);
    QCOMPARE(geometry.stride(), (int) sizeof(float) * 2);
    QCOMPARE(geometry.vertexCount(), 4);
    QCOMPARE(geometry.indexCount(), 0);
    QVERIFY(geometry.indexData() == 0);

    QSGGeometry::updateRectGeometry(&geometry, QRectF(1, 2, 3, 4));

    QSGGeometry::Point2D *pts = geometry.vertexDataAsPoint2D();
    QVERIFY(pts != 0);

    QCOMPARE(pts[0].x, (float) 1);
    QCOMPARE(pts[0].y, (float) 2);
    QCOMPARE(pts[3].x, (float) 4);
    QCOMPARE(pts[3].y, (float) 6);

    // Verify that resize gives me enough allocated data without crashing...
    geometry.allocate(100, 100);
    pts = geometry.vertexDataAsPoint2D();
    quint16 *is = geometry.indexDataAsUShort();
    for (int i=0; i<100; ++i) {
        pts[i].x = i;
        pts[i].y = i + 100;
        is[i] = i;
    }
    QVERIFY(true);
}


void GeometryTest::testTexturedPoint2D()
{
    QSGGeometry geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4, 0);

    QCOMPARE(geometry.attributeCount(), 2);
    QCOMPARE(geometry.stride(), (int) sizeof(float) * 4);
    QCOMPARE(geometry.vertexCount(), 4);
    QCOMPARE(geometry.indexCount(), 0);
    QVERIFY(geometry.indexData() == 0);

    QSGGeometry::updateTexturedRectGeometry(&geometry, QRectF(1, 2, 3, 4), QRectF(5, 6, 7, 8));

    QSGGeometry::TexturedPoint2D *pts = geometry.vertexDataAsTexturedPoint2D();
    QVERIFY(pts != 0);

    QCOMPARE(pts[0].x, (float) 1);
    QCOMPARE(pts[0].y, (float) 2);
    QCOMPARE(pts[0].tx, (float) 5);
    QCOMPARE(pts[0].ty, (float) 6);

    QCOMPARE(pts[3].x, (float) 4);
    QCOMPARE(pts[3].y, (float) 6);
    QCOMPARE(pts[3].tx, (float) 12);
    QCOMPARE(pts[3].ty, (float) 14);

    // Verify that resize gives me enough allocated data without crashing...
    geometry.allocate(100, 100);
    pts = geometry.vertexDataAsTexturedPoint2D();
    quint16 *is = geometry.indexDataAsUShort();
    for (int i=0; i<100; ++i) {
        pts[i].x = i;
        pts[i].y = i + 100;
        pts[i].tx = i + 200;
        pts[i].ty = i + 300;
        is[i] = i;
    }
    QVERIFY(true);
}

void GeometryTest::testCustomGeometry()
{
    struct V {
        float x, y;
        unsigned char r, g, b, a;
        float v1, v2, v3, v4;
    };

    static QSGGeometry::Attribute attributes[] = {
        { 0, 2, GL_FLOAT },
        { 1, 4, GL_UNSIGNED_BYTE },
        { 2, 4, GL_FLOAT },
    };
    static QSGGeometry::AttributeSet set = { 4, 6 * sizeof(float) + 4 * sizeof(unsigned char), attributes };

    QSGGeometry geometry(set, 1000, 4000);

    // Verify that space has been allocated.
    quint16 *ii = geometry.indexDataAsUShort();
    for (int i=0; i<geometry.indexCount(); ++i) {
        ii[i] = i;
    }

    V *v = (V *) geometry.vertexData();
    for (int i=0; i<geometry.vertexCount(); ++i) {
        v[i].x = 0;
        v[i].y = 1;
        v[i].r = 2;
        v[i].g = 3;
        v[i].b = 4;
        v[i].a = 5;
        v[i].v1 = 6;
        v[i].v2 = 7;
        v[i].v3 = 8;
        v[i].v4 = 9;
    }

    // Verify the data's integrity
    for (int i=0; i<4000; ++i)
        QCOMPARE(ii[i], (quint16) i);
    for (int i=0; i<1000; ++i)
        QVERIFY(v[i].v1 == 6);

}


QTEST_MAIN(GeometryTest);

#include "tst_geometry.moc"
