/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qtest.h>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDebug>
#include <private/qdeclarativevaluetype_p.h>
#include "testtypes.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

QT_BEGIN_NAMESPACE
extern Q_GUI_EXPORT int qt_defaultDpi();
QT_END_NAMESPACE

class tst_qdeclarativevaluetypes : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativevaluetypes() {}

private slots:
    void initTestCase();

    void point();
    void pointf();
    void size();
    void sizef();
    void sizereadonly();
    void rect();
    void rectf();
    void vector2d();
    void vector3d();
    void vector4d();
    void quaternion();
    void matrix4x4();
    void font();
    void variant();

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
    void enums();
    void conflictingBindings();
    void returnValues();
    void varAssignment();
    void bindingsSpliceCorrectly();

private:
    QDeclarativeEngine engine;
};

void tst_qdeclarativevaluetypes::initTestCase()
{
    registerTypes();
}

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

void tst_qdeclarativevaluetypes::point()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("point_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("p_x").toInt(), 10);
        QCOMPARE(object->property("p_y").toInt(), 4);
        QCOMPARE(object->property("copy"), QVariant(QPoint(10, 4)));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("point_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->point(), QPoint(11, 12));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::pointf()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("pointf_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(float(object->property("p_x").toDouble()), float(11.3));
        QCOMPARE(float(object->property("p_y").toDouble()), float(-10.9));
        QCOMPARE(object->property("copy"), QVariant(QPointF(11.3, -10.9)));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("pointf_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->pointf(), QPointF(6.8, 9.3));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::size()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("size_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("s_width").toInt(), 1912);
        QCOMPARE(object->property("s_height").toInt(), 1913);
        QCOMPARE(object->property("copy"), QVariant(QSize(1912, 1913)));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("size_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->size(), QSize(13, 88));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::sizef()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("sizef_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(float(object->property("s_width").toDouble()), float(0.1));
        QCOMPARE(float(object->property("s_height").toDouble()), float(100923.2));
        QCOMPARE(object->property("copy"), QVariant(QSizeF(0.1, 100923.2)));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("sizef_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->sizef(), QSizeF(44.3, 92.8));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::variant()
{
    QDeclarativeComponent component(&engine, TEST_FILE("variant_read.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(float(object->property("s_width").toDouble()), float(0.1));
    QCOMPARE(float(object->property("s_height").toDouble()), float(100923.2));
    QCOMPARE(object->property("copy"), QVariant(QSizeF(0.1, 100923.2)));

    delete object;
}

void tst_qdeclarativevaluetypes::sizereadonly()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("sizereadonly_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("s_width").toInt(), 1912);
        QCOMPARE(object->property("s_height").toInt(), 1913);
        QCOMPARE(object->property("copy"), QVariant(QSize(1912, 1913)));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("sizereadonly_writeerror.qml"));
        QVERIFY(component.isError());
        QCOMPARE(component.errors().at(0).description(), QLatin1String("Invalid property assignment: \"sizereadonly\" is a read-only property"));
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("sizereadonly_writeerror2.qml"));
        QVERIFY(component.isError());
        QCOMPARE(component.errors().at(0).description(), QLatin1String("Invalid property assignment: \"sizereadonly\" is a read-only property"));
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("sizereadonly_writeerror3.qml"));
        QVERIFY(component.isError());
        QCOMPARE(component.errors().at(0).description(), QLatin1String("Invalid property assignment: \"sizereadonly\" is a read-only property"));
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("sizereadonly_writeerror4.qml"));

        QObject *object = component.create();
        QVERIFY(object);

        QCOMPARE(object->property("sizereadonly").toSize(), QSize(1912, 1913));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::rect()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("rect_read.qml"));
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
        QDeclarativeComponent component(&engine, TEST_FILE("rect_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->rect(), QRect(1234, 7, 56, 63));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::rectf()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("rectf_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(float(object->property("r_x").toDouble()), float(103.8));
        QCOMPARE(float(object->property("r_y").toDouble()), float(99.2));
        QCOMPARE(float(object->property("r_width").toDouble()), float(88.1));
        QCOMPARE(float(object->property("r_height").toDouble()), float(77.6));
        QCOMPARE(object->property("copy"), QVariant(QRectF(103.8, 99.2, 88.1, 77.6)));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("rectf_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->rectf(), QRectF(70.1, -113.2, 80924.8, 99.2));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::vector2d()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("vector2d_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE((float)object->property("v_x").toDouble(), (float)32.88);
        QCOMPARE((float)object->property("v_y").toDouble(), (float)1.3);
        QCOMPARE(object->property("copy"), QVariant(QVector2D(32.88, 1.3)));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("vector2d_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->vector2(), QVector2D(-0.3, -12.9));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::vector3d()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("vector3d_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE((float)object->property("v_x").toDouble(), (float)23.88);
        QCOMPARE((float)object->property("v_y").toDouble(), (float)3.1);
        QCOMPARE((float)object->property("v_z").toDouble(), (float)4.3);
        QCOMPARE(object->property("copy"), QVariant(QVector3D(23.88, 3.1, 4.3)));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("vector3d_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->vector(), QVector3D(-0.3, -12.9, 907.4));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::vector4d()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("vector4d_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE((float)object->property("v_x").toDouble(), (float)54.2);
        QCOMPARE((float)object->property("v_y").toDouble(), (float)23.88);
        QCOMPARE((float)object->property("v_z").toDouble(), (float)3.1);
        QCOMPARE((float)object->property("v_w").toDouble(), (float)4.3);
        QCOMPARE(object->property("copy"), QVariant(QVector4D(54.2, 23.88, 3.1, 4.3)));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("vector4d_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->vector4(), QVector4D(-0.3, -12.9, 907.4, 88.5));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::quaternion()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("quaternion_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE((float)object->property("v_scalar").toDouble(), (float)4.3);
        QCOMPARE((float)object->property("v_x").toDouble(), (float)54.2);
        QCOMPARE((float)object->property("v_y").toDouble(), (float)23.88);
        QCOMPARE((float)object->property("v_z").toDouble(), (float)3.1);
        QCOMPARE(object->property("copy"), QVariant(QQuaternion(4.3, 54.2, 23.88, 3.1)));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("quaternion_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->quaternion(), QQuaternion(88.5, -0.3, -12.9, 907.4));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::matrix4x4()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("matrix4x4_read.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE((float)object->property("v_m11").toDouble(), (float)1);
        QCOMPARE((float)object->property("v_m12").toDouble(), (float)2);
        QCOMPARE((float)object->property("v_m13").toDouble(), (float)3);
        QCOMPARE((float)object->property("v_m14").toDouble(), (float)4);
        QCOMPARE((float)object->property("v_m21").toDouble(), (float)5);
        QCOMPARE((float)object->property("v_m22").toDouble(), (float)6);
        QCOMPARE((float)object->property("v_m23").toDouble(), (float)7);
        QCOMPARE((float)object->property("v_m24").toDouble(), (float)8);
        QCOMPARE((float)object->property("v_m31").toDouble(), (float)9);
        QCOMPARE((float)object->property("v_m32").toDouble(), (float)10);
        QCOMPARE((float)object->property("v_m33").toDouble(), (float)11);
        QCOMPARE((float)object->property("v_m34").toDouble(), (float)12);
        QCOMPARE((float)object->property("v_m41").toDouble(), (float)13);
        QCOMPARE((float)object->property("v_m42").toDouble(), (float)14);
        QCOMPARE((float)object->property("v_m43").toDouble(), (float)15);
        QCOMPARE((float)object->property("v_m44").toDouble(), (float)16);
        QCOMPARE(object->property("copy"),
                 QVariant(QMatrix4x4(1, 2, 3, 4,
                                     5, 6, 7, 8,
                                     9, 10, 11, 12,
                                     13, 14, 15, 16)));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("matrix4x4_write.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->matrix(), QMatrix4x4(11, 12, 13, 14,
                                              21, 22, 23, 24,
                                              31, 32, 33, 34,
                                              41, 42, 43, 44));

        delete object;
    }
}

void tst_qdeclarativevaluetypes::font()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("font_read.qml"));
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
        QCOMPARE(object->property("f_pixelSize").toInt(), int((object->font().pointSizeF() * qt_defaultDpi()) / qreal(72.)));
        QCOMPARE(object->property("f_capitalization").toInt(), (int)object->font().capitalization());
        QCOMPARE(object->property("f_letterSpacing").toDouble(), object->font().letterSpacing());
        QCOMPARE(object->property("f_wordSpacing").toDouble(), object->font().wordSpacing());

        QCOMPARE(object->property("copy"), QVariant(object->font()));

        delete object;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("font_write.qml"));
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

        QFont f = object->font();
        QCOMPARE(f.family(), font.family());
        QCOMPARE(f.bold(), font.bold());
        QCOMPARE(f.weight(), font.weight());
        QCOMPARE(f.italic(), font.italic());
        QCOMPARE(f.underline(), font.underline());
        QCOMPARE(f.strikeOut(), font.strikeOut());
        QCOMPARE(f.pointSize(), font.pointSize());
        QCOMPARE(f.capitalization(), font.capitalization());
        QCOMPARE(f.letterSpacing(), font.letterSpacing());
        QCOMPARE(f.wordSpacing(), font.wordSpacing());

        delete object;
    }

    // Test pixelSize
    {
        QDeclarativeComponent component(&engine, TEST_FILE("font_write.2.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->font().pixelSize(), 10);

        delete object;
    }

    // Test pixelSize and pointSize
    {
        QDeclarativeComponent component(&engine, TEST_FILE("font_write.3.qml"));
        QTest::ignoreMessage(QtWarningMsg, "Both point size and pixel size set. Using pixel size. ");
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->font().pixelSize(), 10);

        delete object;
    }
    {
        QDeclarativeComponent component(&engine, TEST_FILE("font_write.4.qml"));
        QTest::ignoreMessage(QtWarningMsg, "Both point size and pixel size set. Using pixel size. ");
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->font().pixelSize(), 10);

        delete object;
    }
    {
        QDeclarativeComponent component(&engine, TEST_FILE("font_write.5.qml"));
        QObject *object = qobject_cast<QObject *>(component.create());
        QVERIFY(object != 0);
        MyTypeObject *object1 = object->findChild<MyTypeObject *>("object1");
        QVERIFY(object1 != 0);
        MyTypeObject *object2 = object->findChild<MyTypeObject *>("object2");
        QVERIFY(object2 != 0);

        QCOMPARE(object1->font().pixelSize(), 19);
        QCOMPARE(object2->font().pointSize(), 14);

        delete object;
    }
}

// Test bindings can write to value types
void tst_qdeclarativevaluetypes::bindingAssignment()
{
    QDeclarativeComponent component(&engine, TEST_FILE("bindingAssignment.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->rect().x(), 10);

    object->setProperty("value", QVariant(92));

    QCOMPARE(object->rect().x(), 92);

    delete object;
}

// Test bindings can read from value types
void tst_qdeclarativevaluetypes::bindingRead()
{
    QDeclarativeComponent component(&engine, TEST_FILE("bindingRead.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->property("value").toInt(), 2);

    object->setRect(QRect(19, 3, 88, 2));

    QCOMPARE(object->property("value").toInt(), 19);

    delete object;
}

// Test static values can assign to value types
void tst_qdeclarativevaluetypes::staticAssignment()
{
    QDeclarativeComponent component(&engine, TEST_FILE("staticAssignment.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->rect().x(), 9);

    delete object;
}

// Test scripts can read/write value types
void tst_qdeclarativevaluetypes::scriptAccess()
{
    QDeclarativeComponent component(&engine, TEST_FILE("scriptAccess.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->property("valuePre").toInt(), 2);
    QCOMPARE(object->rect().x(), 19);
    QCOMPARE(object->property("valuePost").toInt(), 19);

    delete object;
}

// Test that assigning a constant from script removes any binding
void tst_qdeclarativevaluetypes::autoBindingRemoval()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("autoBindingRemoval.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->rect().x(), 10);

        object->setProperty("value", QVariant(13));

        QCOMPARE(object->rect().x(), 13);

        object->emitRunScript();

        QCOMPARE(object->rect().x(), 42);

        object->setProperty("value", QVariant(92));

        QCOMPARE(object->rect().x(), 42);

        delete object;
    }

    /*
    {
        QDeclarativeComponent component(&engine, TEST_FILE("autoBindingRemoval.2.qml"));
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
        QDeclarativeComponent component(&engine, TEST_FILE("autoBindingRemoval.3.qml"));
        MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
        QVERIFY(object != 0);

        object->setProperty("value", QVariant(QRect(9, 22, 33, 44)));

        QCOMPARE(object->rect(), QRect(9, 22, 33, 44));

        object->emitRunScript();

        QCOMPARE(object->rect(), QRect(44, 22, 33, 44));

        object->setProperty("value", QVariant(QRect(19, 3, 4, 8)));

        QCOMPARE(object->rect(), QRect(44, 22, 33, 44));

        delete object;
    }
*/
}

// Test that property value sources assign to value types
void tst_qdeclarativevaluetypes::valueSources()
{
    QDeclarativeComponent component(&engine, TEST_FILE("valueSources.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->rect().x(), 3345);

    delete object;
}

static void checkNoErrors(QDeclarativeComponent& component)
{
    QList<QDeclarativeError> errors = component.errors();
    if (errors.isEmpty())
        return;
    for (int ii = 0; ii < errors.count(); ++ii) {
        const QDeclarativeError &error = errors.at(ii);
        qWarning("%d:%d:%s",error.line(),error.column(),error.description().toUtf8().constData());
    }
}

// Test that property value interceptors can be applied to value types
void tst_qdeclarativevaluetypes::valueInterceptors()
{
    QDeclarativeComponent component(&engine, TEST_FILE("valueInterceptors.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    checkNoErrors(component);
    QVERIFY(object != 0);

    QCOMPARE(object->rect().x(), 13);

    object->setProperty("value", 99);

    QCOMPARE(object->rect().x(), 112);

    delete object;
}

// Test that you can't assign a binding to the "root" value type, and a sub-property
void tst_qdeclarativevaluetypes::bindingConflict()
{
    QDeclarativeComponent component(&engine, TEST_FILE("bindingConflict.qml"));
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
void tst_qdeclarativevaluetypes::deletedObject()
{
    QDeclarativeComponent component(&engine, TEST_FILE("deletedObject.qml"));
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
void tst_qdeclarativevaluetypes::bindingVariantCopy()
{
    QDeclarativeComponent component(&engine, TEST_FILE("bindingVariantCopy.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->rect(), QRect(19, 33, 5, 99));

    delete object;
}

// Test that value types can be assigned to another value type property in script
void tst_qdeclarativevaluetypes::scriptVariantCopy()
{
    QDeclarativeComponent component(&engine, TEST_FILE("scriptVariantCopy.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->rect(), QRect(2, 3, 109, 102));

    object->emitRunScript();

    QCOMPARE(object->rect(), QRect(19, 33, 5, 99));

    delete object;
}


// Test that the value type classes can be used manually
void tst_qdeclarativevaluetypes::cppClasses()
{
    CPP_TEST(QDeclarativePointValueType, QPoint(19, 33));
    CPP_TEST(QDeclarativePointFValueType, QPointF(33.6, -23));
    CPP_TEST(QDeclarativeSizeValueType, QSize(-100, 18));
    CPP_TEST(QDeclarativeSizeFValueType, QSizeF(-100.7, 18.2));
    CPP_TEST(QDeclarativeRectValueType, QRect(13, 39, 10928, 88));
    CPP_TEST(QDeclarativeRectFValueType, QRectF(88.2, -90.1, 103.2, 118));
    CPP_TEST(QDeclarativeVector2DValueType, QVector2D(19.7, 1002));
    CPP_TEST(QDeclarativeVector3DValueType, QVector3D(18.2, 19.7, 1002));
    CPP_TEST(QDeclarativeVector4DValueType, QVector4D(18.2, 19.7, 1002, 54));
    CPP_TEST(QDeclarativeQuaternionValueType, QQuaternion(18.2, 19.7, 1002, 54));
    CPP_TEST(QDeclarativeMatrix4x4ValueType,
             QMatrix4x4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    CPP_TEST(QDeclarativeFontValueType, QFont("Helvetica"));

}

void tst_qdeclarativevaluetypes::enums()
{
    {
    QDeclarativeComponent component(&engine, TEST_FILE("enums.1.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QVERIFY(object->font().capitalization() == QFont::AllUppercase);
    delete object;
    }

    {
    QDeclarativeComponent component(&engine, TEST_FILE("enums.2.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QVERIFY(object->font().capitalization() == QFont::AllUppercase);
    delete object;
    }

    {
    QDeclarativeComponent component(&engine, TEST_FILE("enums.3.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QVERIFY(object->font().capitalization() == QFont::AllUppercase);
    delete object;
    }

    {
    QDeclarativeComponent component(&engine, TEST_FILE("enums.4.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QVERIFY(object->font().capitalization() == QFont::AllUppercase);
    delete object;
    }

    {
    QDeclarativeComponent component(&engine, TEST_FILE("enums.5.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QVERIFY(object->font().capitalization() == QFont::AllUppercase);
    delete object;
    }
}

// Tests switching between "conflicting" bindings (eg. a binding on the core
// property, to a binding on the value-type sub-property)
void tst_qdeclarativevaluetypes::conflictingBindings()
{
    {
    QDeclarativeComponent component(&engine, TEST_FILE("conflicting.1.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<QFont>(object->property("font")).pixelSize(), 12);

    QMetaObject::invokeMethod(object, "toggle");

    QCOMPARE(qvariant_cast<QFont>(object->property("font")).pixelSize(), 6);

    QMetaObject::invokeMethod(object, "toggle");

    QCOMPARE(qvariant_cast<QFont>(object->property("font")).pixelSize(), 12);

    delete object;
    }

    {
    QDeclarativeComponent component(&engine, TEST_FILE("conflicting.2.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<QFont>(object->property("font")).pixelSize(), 6);

    QMetaObject::invokeMethod(object, "toggle");

    QCOMPARE(qvariant_cast<QFont>(object->property("font")).pixelSize(), 12);

    QMetaObject::invokeMethod(object, "toggle");

    QCOMPARE(qvariant_cast<QFont>(object->property("font")).pixelSize(), 6);

    delete object;
    }

    {
    QDeclarativeComponent component(&engine, TEST_FILE("conflicting.3.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<QFont>(object->property("font")).pixelSize(), 12);

    QMetaObject::invokeMethod(object, "toggle");

    QCOMPARE(qvariant_cast<QFont>(object->property("font")).pixelSize(), 24);

    QMetaObject::invokeMethod(object, "toggle");

    QCOMPARE(qvariant_cast<QFont>(object->property("font")).pixelSize(), 12);

    delete object;
    }
}

void tst_qdeclarativevaluetypes::returnValues()
{
    QDeclarativeComponent component(&engine, TEST_FILE("returnValues.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test1").toBool(), true);
    QCOMPARE(object->property("test2").toBool(), true);
    QCOMPARE(object->property("size").toSize(), QSize(13, 14));

    delete object;
}

void tst_qdeclarativevaluetypes::varAssignment()
{
    QDeclarativeComponent component(&engine, TEST_FILE("varAssignment.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("x").toInt(), 1);
    QCOMPARE(object->property("y").toInt(), 2);
    QCOMPARE(object->property("z").toInt(), 3);

    delete object;
}

// Test bindings splice together correctly
void tst_qdeclarativevaluetypes::bindingsSpliceCorrectly()
{
    {
    QDeclarativeComponent component(&engine, TEST_FILE("bindingsSpliceCorrectly.1.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);

    delete object;
    }

    {
    QDeclarativeComponent component(&engine, TEST_FILE("bindingsSpliceCorrectly.2.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);

    delete object;
    }


    {
    QDeclarativeComponent component(&engine, TEST_FILE("bindingsSpliceCorrectly.3.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);

    delete object;
    }

    {
    QDeclarativeComponent component(&engine, TEST_FILE("bindingsSpliceCorrectly.4.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);

    delete object;
    }

    {
    QDeclarativeComponent component(&engine, TEST_FILE("bindingsSpliceCorrectly.5.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);

    delete object;
    }
}

QTEST_MAIN(tst_qdeclarativevaluetypes)

#include "tst_qdeclarativevaluetypes.moc"
