/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qtest.h>
#include <QmlEngine>
#include <QmlComponent>
#include <QDebug>
#include "testtypes.h"

class tst_valuetypes : public QObject
{
    Q_OBJECT
public:
    tst_valuetypes() {}

private slots:
    void point();
    void pointf();
    void size();
    void sizef();
    void rect();
    void rectf();
    void vector3d();
    void font();

    // ###
    // Test binding assignment
    // Test static assignment
    // Test JS assignment
    // Test "font.x: blah; font: blah2;" conflict
    // Test constant binding removal
    // Test value sources
    // Test behaviours
private:
    QmlEngine engine;
};

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

void tst_valuetypes::point()
{
    {
        QmlComponent component(&engine, TEST_FILE("point_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("p_x").toInt(), 10);
        QCOMPARE(object->property("p_y").toInt(), 4);
        QCOMPARE(object->property("copy"), QVariant(QPoint(10, 4)));

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("point_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->point(), QPoint(11, 12));

        delete object;
    }
}

void tst_valuetypes::pointf()
{
    {
        QmlComponent component(&engine, TEST_FILE("pointf_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("p_x").toDouble(), 11.3);
        QCOMPARE(object->property("p_y").toDouble(), -10.9);
        QCOMPARE(object->property("copy"), QVariant(QPointF(11.3, -10.9)));

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("pointf_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->pointf(), QPointF(6.8, 9.3));

        delete object;
    }
}

void tst_valuetypes::size()
{
    {
        QmlComponent component(&engine, TEST_FILE("size_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("s_width").toInt(), 1912);
        QCOMPARE(object->property("s_height").toInt(), 1913);
        QCOMPARE(object->property("copy"), QVariant(QSize(1912, 1913)));

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("size_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->size(), QSize(13, 88));

        delete object;
    }
}

void tst_valuetypes::sizef()
{
    {
        QmlComponent component(&engine, TEST_FILE("sizef_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("s_width").toDouble(), 0.1);
        QCOMPARE(object->property("s_height").toDouble(), 100923.2);
        QCOMPARE(object->property("copy"), QVariant(QSizeF(0.1, 100923.2)));

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("sizef_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->sizef(), QSizeF(44.3, 92.8));

        delete object;
    }
}

void tst_valuetypes::rect()
{
    {
        QmlComponent component(&engine, TEST_FILE("rect_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("r_x").toInt(), 2);
        QCOMPARE(object->property("r_y").toInt(), 3);
        QCOMPARE(object->property("r_width").toInt(), 109);
        QCOMPARE(object->property("r_height").toInt(), 102);
        QCOMPARE(object->property("copy"), QVariant(QRect(2, 3, 109, 102)));

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("rect_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->rect(), QRect(1234, 7, 56, 63));

        delete object;
    }
}

void tst_valuetypes::rectf()
{
    {
        QmlComponent component(&engine, TEST_FILE("rectf_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("r_x").toDouble(), 103.8);
        QCOMPARE(object->property("r_y").toDouble(), 99.2);
        QCOMPARE(object->property("r_width").toDouble(), 88.1);
        QCOMPARE(object->property("r_height").toDouble(), 77.6);
        QCOMPARE(object->property("copy"), QVariant(QRectF(103.8, 99.2, 88.1, 77.6)));

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("rectf_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->rectf(), QRectF(70.1, -113.2, 80924.8, 99.2));

        delete object;
    }
}

void tst_valuetypes::vector3d()
{
    {
        QmlComponent component(&engine, TEST_FILE("vector3d_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE((float)object->property("v_x").toDouble(), (float)23.88);
        QCOMPARE((float)object->property("v_y").toDouble(), (float)3.1);
        QCOMPARE((float)object->property("v_z").toDouble(), (float)4.3);
        QCOMPARE(object->property("copy"), QVariant(QVector3D(23.88, 3.1, 4.3)));

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("vector3d_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->vector(), QVector3D(-0.3, -12.9, 907.4));

        delete object;
    }
}

void tst_valuetypes::font()
{
    {
        QmlComponent component(&engine, TEST_FILE("font_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("f_family").toString(), object->font().family());
        QCOMPARE(object->property("f_bold").toBool(), object->font().bold());
        QCOMPARE(object->property("f_weight").toInt(), object->font().weight());
        QCOMPARE(object->property("f_italic").toBool(), object->font().italic());
        QCOMPARE(object->property("f_underline").toBool(), object->font().underline());
        QCOMPARE(object->property("f_overline").toBool(), object->font().overline());
        QCOMPARE(object->property("f_strikeout").toBool(), object->font().strikeOut());
        QCOMPARE(object->property("f_pointSize").toDouble(), object->font().pointSizeF());
        QCOMPARE(object->property("f_pixelSize").toInt(), object->font().pixelSize());
        QCOMPARE(object->property("f_capitalization").toInt(), (int)object->font().capitalization());
        QCOMPARE(object->property("f_letterSpacing").toDouble(), object->font().letterSpacing());
        QCOMPARE(object->property("f_wordSpacing").toDouble(), object->font().wordSpacing());

        QCOMPARE(object->property("copy"), QVariant(object->font()));

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("font_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QFont font;
        font.setFamily("Helvetica");
        font.setBold(false);
        font.setWeight(QFont::Normal);
        font.setItalic(false);
        font.setUnderline(false);
        font.setStrikeOut(false);
        font.setPointSize(15);
        font.setCapitalization(QFont::AllLowercase);
        font.setLetterSpacing(QFont::AbsoluteSpacing, 9.7);
        font.setWordSpacing(11.2);

        QCOMPARE(object->font(), font);

        delete object;
    }

    // Test pixelSize
    {
        QmlComponent component(&engine, TEST_FILE("font_write.2.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->font().pixelSize(), 10);
    }

    // Test pixelSize and pointSize
    {
        QmlComponent component(&engine, TEST_FILE("font_write.3.qml"));
        QTest::ignoreMessage(QtWarningMsg, "Both point size and pixel size set. Using pixel size. ");
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->font().pixelSize(), 10);
    }
}

QTEST_MAIN(tst_valuetypes)

#include "tst_valuetypes.moc"
