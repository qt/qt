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

#include <qtest.h>
#include <QmlEngine>
#include <QmlComponent>
#include <QDebug>
#include <private/qmlvaluetype_p.h>
#include "testtypes.h"

class tst_qmlvaluetypes : public QObject
{
    Q_OBJECT
public:
    tst_qmlvaluetypes() {}

private slots:
    void point();
    void pointf();
    void size();
    void sizef();
    void rect();
    void rectf();
    void vector3d();
    void font();

    void bindingAssignment();
    void bindingRead();
    void staticAssignment();
    void scriptAccess();
    void autoBindingRemoval();
    void valueSources();
    void valueInterceptors();
    void bindingConflict();
    void deletedObject();
    void bindingVariantCopy();
    void scriptVariantCopy();
    void cppClasses();

private:
    QmlEngine engine;
};

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

void tst_qmlvaluetypes::point()
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

void tst_qmlvaluetypes::pointf()
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

void tst_qmlvaluetypes::size()
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

void tst_qmlvaluetypes::sizef()
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

void tst_qmlvaluetypes::rect()
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

void tst_qmlvaluetypes::rectf()
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

void tst_qmlvaluetypes::vector3d()
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

void tst_qmlvaluetypes::font()
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

        QEXPECT_FAIL("", "QT-2920", Continue);
        QCOMPARE(object->font(), font);

        delete object;
    }

    // Test pixelSize
    {
        QmlComponent component(&engine, TEST_FILE("font_write.2.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->font().pixelSize(), 10);

        delete object;
    }

    // Test pixelSize and pointSize
    {
        QmlComponent component(&engine, TEST_FILE("font_write.3.qml"));
        QTest::ignoreMessage(QtWarningMsg, "Both point size and pixel size set. Using pixel size. ");
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->font().pixelSize(), 10);

        delete object;
    }
}

// Test bindings can write to value types
void tst_qmlvaluetypes::bindingAssignment()
{
    QmlComponent component(&engine, TEST_FILE("bindingAssignment.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->rect().x(), 10);

    object->setProperty("value", QVariant(92));

    QCOMPARE(object->rect().x(), 92);

    delete object;
}

// Test bindings can read from value types
void tst_qmlvaluetypes::bindingRead()
{
    QmlComponent component(&engine, TEST_FILE("bindingRead.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->property("value").toInt(), 2);

    object->setRect(QRect(19, 3, 88, 2));

    QCOMPARE(object->property("value").toInt(), 19);

    delete object;
}

// Test static values can assign to value types
void tst_qmlvaluetypes::staticAssignment()
{
    QmlComponent component(&engine, TEST_FILE("staticAssignment.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->rect().x(), 9);

    delete object;
}

// Test scripts can read/write value types
void tst_qmlvaluetypes::scriptAccess()
{
    QmlComponent component(&engine, TEST_FILE("scriptAccess.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->property("valuePre").toInt(), 2);
    QCOMPARE(object->rect().x(), 19);
    QCOMPARE(object->property("valuePost").toInt(), 19);

    delete object;
}

// Test that assigning a constant from script removes any binding
void tst_qmlvaluetypes::autoBindingRemoval()
{
    {
        QmlComponent component(&engine, TEST_FILE("autoBindingRemoval.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->rect().x(), 10);

        object->setProperty("value", QVariant(13));

        QCOMPARE(object->rect().x(), 13);

        object->emitRunScript();

        QCOMPARE(object->rect().x(), 42);

        object->setProperty("value", QVariant(92));

        QEXPECT_FAIL("", "QT-2920", Continue);
        QCOMPARE(object->rect().x(), 42);

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("autoBindingRemoval.2.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->rect().x(), 10);

        object->setProperty("value", QVariant(13));

        QCOMPARE(object->rect().x(), 13);

        object->emitRunScript();

        QCOMPARE(object->rect(), QRect(10, 10, 10, 10));

        object->setProperty("value", QVariant(92));

        QCOMPARE(object->rect(), QRect(10, 10, 10, 10));

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("autoBindingRemoval.3.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        object->setProperty("value", QVariant(QRect(9, 22, 33, 44)));

        QCOMPARE(object->rect(), QRect(9, 22, 33, 44));

        object->emitRunScript();

        QCOMPARE(object->rect(), QRect(44, 22, 33, 44));

        object->setProperty("value", QVariant(QRect(19, 3, 4, 8)));

        QEXPECT_FAIL("", "QT-2920", Continue);
        QCOMPARE(object->rect(), QRect(44, 22, 33, 44));

        delete object;
    }

}

// Test that property value sources assign to value types
void tst_qmlvaluetypes::valueSources()
{
    QmlComponent component(&engine, TEST_FILE("valueSources.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->rect().x(), 3345);

    delete object;
}

static void checkNoErrors(QmlComponent& component)
{
    QList<QmlError> errors = component.errors();
    if (errors.isEmpty())
        return;
    for (int ii = 0; ii < errors.count(); ++ii) {
        const QmlError &error = errors.at(ii);
        qWarning("%d:%d:%s",error.line(),error.column(),error.description().toUtf8().constData());
    }
}

// Test that property value interceptors can be applied to value types
void tst_qmlvaluetypes::valueInterceptors()
{
    QmlComponent component(&engine, TEST_FILE("valueInterceptors.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    checkNoErrors(component);
    QEXPECT_FAIL("", "QT-2920", Abort);
    QVERIFY(object != 0);

    QCOMPARE(object->rect().x(), 26);

    object->setProperty("value", 99);

    QCOMPARE(object->rect().x(), 112);

    delete object;
}

// Test that you can't assign a binding to the "root" value type, and a sub-property
void tst_qmlvaluetypes::bindingConflict()
{
    QmlComponent component(&engine, TEST_FILE("bindingConflict.qml"));
    QCOMPARE(component.isError(), true);
}

#define CPP_TEST(type, v) \
{ \
    type *t = new type; \
    QVariant value(v); \
    t->setValue(value); \
    QCOMPARE(t->value(), value); \
    delete t; \
}

// Test that accessing a reference to a valuetype after the owning object is deleted
// doesn't crash
void tst_qmlvaluetypes::deletedObject()
{
    QmlComponent component(&engine, TEST_FILE("deletedObject.qml"));
    QTest::ignoreMessage(QtDebugMsg, "Test: 2");
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QObject *dObject = qvariant_cast<QObject *>(object->property("object"));
    QVERIFY(dObject != 0);
    delete dObject;

    QTest::ignoreMessage(QtDebugMsg, "Test: undefined");
    object->emitRunScript();

    delete object;
}

// Test that value types can be assigned to another value type property in a binding
void tst_qmlvaluetypes::bindingVariantCopy()
{
    QmlComponent component(&engine, TEST_FILE("bindingVariantCopy.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->rect(), QRect(19, 33, 5, 99));

    delete object;
}

// Test that value types can be assigned to another value type property in script
void tst_qmlvaluetypes::scriptVariantCopy()
{
    QmlComponent component(&engine, TEST_FILE("scriptVariantCopy.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->rect(), QRect(2, 3, 109, 102));

    object->emitRunScript();

    QCOMPARE(object->rect(), QRect(19, 33, 5, 99));

    delete object;
}


// Test that the value type classes can be used manually
void tst_qmlvaluetypes::cppClasses()
{
    CPP_TEST(QmlPointValueType, QPoint(19, 33));
    CPP_TEST(QmlPointFValueType, QPointF(33.6, -23));
    CPP_TEST(QmlSizeValueType, QSize(-100, 18));
    CPP_TEST(QmlSizeFValueType, QSizeF(-100.7, 18.2));
    CPP_TEST(QmlRectValueType, QRect(13, 39, 10928, 88));
    CPP_TEST(QmlRectFValueType, QRectF(88.2, -90.1, 103.2, 118));
    CPP_TEST(QmlVector3DValueType, QVector3D(18.2, 19.7, 1002));
    CPP_TEST(QmlFontValueType, QFont("Helvetica"));

}
QTEST_MAIN(tst_qmlvaluetypes)

#include "tst_qmlvaluetypes.moc"
