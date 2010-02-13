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
#include <QmlMetaType>
#include <QLocale>
#include <QPixmap>
#include <QBitmap>
#include <QPen>
#include <QTextLength>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#include <qml.h>

class tst_qmlmetatype : public QObject
{
    Q_OBJECT
public:
    tst_qmlmetatype() {}

private slots:
    void copy();

    void qmlParserStatusCast();
    void qmlPropertyValueSourceCast();
    void qmlPropertyValueInterceptorCast();

    void isList();
    void isQmlList();

    void listCount();
    void listAt();

    void defaultObject();
};

class TestType : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int foo READ foo);

    Q_CLASSINFO("DefaultProperty", "foo");
public:
    int foo() { return 0; }
};
QML_DECLARE_TYPE(TestType);
QML_DEFINE_TYPE(Test, 1, 0, TestType, TestType);

class ParserStatusTestType : public QObject, public QmlParserStatus
{
    Q_OBJECT
    Q_CLASSINFO("DefaultProperty", "foo"); // Missing default property
};
QML_DECLARE_TYPE(ParserStatusTestType);
QML_DEFINE_TYPE(Test, 1, 0, ParserStatusTestType, ParserStatusTestType);

class ValueSourceTestType : public QObject, public QmlPropertyValueSource
{
    Q_OBJECT
    Q_INTERFACES(QmlPropertyValueSource)
public:
    virtual void setTarget(const QmlMetaProperty &) {}
};
QML_DECLARE_TYPE(ValueSourceTestType);
QML_DEFINE_TYPE(Test, 1, 0, ValueSourceTestType, ValueSourceTestType);

class ValueInterceptorTestType : public QObject, public QmlPropertyValueInterceptor
{
    Q_OBJECT
    Q_INTERFACES(QmlPropertyValueInterceptor)
public:
    virtual void setTarget(const QmlMetaProperty &) {}
    virtual void write(const QVariant &) {}
};
QML_DECLARE_TYPE(ValueInterceptorTestType);
QML_DEFINE_TYPE(Test, 1, 0, ValueInterceptorTestType, ValueInterceptorTestType);


#define COPY_TEST(cpptype, metatype, value, defaultvalue) \
{ \
    cpptype v = (value); cpptype v2 = (value); \
    QVERIFY(QmlMetaType::copy(QMetaType:: metatype, &v, 0)); \
    QVERIFY(v == (defaultvalue)); \
    QVERIFY(QmlMetaType::copy(QMetaType:: metatype, &v, &v2)); \
    QVERIFY(v == (value)); \
}

#define QT_COPY_TEST(type, value) \
{ \
    type v = (value); type v2 = (value); \
    QVERIFY(QmlMetaType::copy(QMetaType:: type, &v, 0)); \
    QVERIFY(v == (type ())); \
    QVERIFY(QmlMetaType::copy(QMetaType:: type, &v, &v2)); \
    QVERIFY(v == (value)); \
}

void tst_qmlmetatype::copy()
{
    QVERIFY(QmlMetaType::copy(QMetaType::Void, 0, 0));

    COPY_TEST(bool, Bool, true, false);
    COPY_TEST(int, Int, 10, 0);
    COPY_TEST(unsigned int, UInt, 10, 0);
    COPY_TEST(long long, LongLong, 10, 0);
    COPY_TEST(unsigned long long, ULongLong, 10, 0);
    COPY_TEST(double, Double, 19.2, 0);

    QT_COPY_TEST(QChar, QChar('a'));

    QVariantMap variantMap;
    variantMap.insert("Hello World!", QVariant(10));
    QT_COPY_TEST(QVariantMap, variantMap);

    QT_COPY_TEST(QVariantList, QVariantList() << QVariant(19.2));
    QT_COPY_TEST(QString, QString("QML Rocks!"));
    QT_COPY_TEST(QStringList, QStringList() << "QML" << "Rocks");
    QT_COPY_TEST(QByteArray, QByteArray("0x1102DDD"));
    QT_COPY_TEST(QBitArray, QBitArray(102, true));
    QT_COPY_TEST(QDate, QDate::currentDate());
    QT_COPY_TEST(QTime, QTime::currentTime());
    QT_COPY_TEST(QDateTime, QDateTime::currentDateTime());
    QT_COPY_TEST(QUrl, QUrl("http://www.nokia.com"));
    QT_COPY_TEST(QLocale, QLocale(QLocale::English, QLocale::Australia));
    QT_COPY_TEST(QRect, QRect(-10, 10, 102, 99));
    QT_COPY_TEST(QRectF, QRectF(-10.2, 1.2, 102, 99.6));
    QT_COPY_TEST(QSize, QSize(100, 2));
    QT_COPY_TEST(QSizeF, QSizeF(20.2, -100234.2)); 
    QT_COPY_TEST(QLine, QLine(0, 0, 100, 100));
    QT_COPY_TEST(QLineF, QLineF(-10.2, 0, 103, 1));
    QT_COPY_TEST(QPoint, QPoint(-1912, 1613));
    QT_COPY_TEST(QPointF, QPointF(-908.1, 1612));
    QT_COPY_TEST(QRegExp, QRegExp("(\\d+)(?:\\s*)(cm|inch)"));

    QVariantHash variantHash;
    variantHash.insert("Hello World!", QVariant(19));
    QT_COPY_TEST(QVariantHash, variantHash);

#ifdef QT3_SUPPORT
    QT_COPY_TEST(QColorGroup, QColorGroup(Qt::red, Qt::red, Qt::red, Qt::red, Qt::red, Qt::red, Qt::red));
#endif

    QT_COPY_TEST(QFont, QFont("Helvetica", 1024));

    {
        QPixmap v = QPixmap(100, 100); QPixmap v2 = QPixmap(100, 100);
        QVERIFY(QmlMetaType::copy(QMetaType::QPixmap, &v, 0)); 
        QVERIFY(v.size() == QPixmap().size());
        QVERIFY(QmlMetaType::copy(QMetaType::QPixmap , &v, &v2)); 
        QVERIFY(v.size() == QPixmap(100,100).size());
    }

    QT_COPY_TEST(QBrush, QBrush(Qt::blue));
    QT_COPY_TEST(QColor, QColor("lightsteelblue"));
    QT_COPY_TEST(QPalette, QPalette(Qt::green));
    
    {
        QPixmap icon(100, 100);

        QIcon v = QIcon(icon); QIcon v2 = QIcon(icon);
        QVERIFY(QmlMetaType::copy(QMetaType::QIcon, &v, 0)); 
        QVERIFY(v.isNull() == QIcon().isNull());
        QVERIFY(QmlMetaType::copy(QMetaType::QIcon , &v, &v2)); 
        QVERIFY(v.isNull() == QIcon(icon).isNull());
    }

    {
        QImage v = QImage(100, 100, QImage::Format_RGB32); 
        QImage v2 = QImage(100, 100, QImage::Format_RGB32);
        QVERIFY(QmlMetaType::copy(QMetaType::QImage, &v, 0)); 
        QVERIFY(v.size() == QImage().size());
        QVERIFY(QmlMetaType::copy(QMetaType::QImage , &v, &v2)); 
        QVERIFY(v.size() == QImage(100,100, QImage::Format_RGB32).size());
    }

    QT_COPY_TEST(QPolygon, QPolygon(QRect(100, 100, 200, 103)));
    QT_COPY_TEST(QRegion, QRegion(QRect(0, 10, 99, 87)));

    {
        QBitmap v = QBitmap(100, 100); QBitmap v2 = QBitmap(100, 100);
        QVERIFY(QmlMetaType::copy(QMetaType::QBitmap, &v, 0)); 
        QVERIFY(v.size() == QBitmap().size());
        QVERIFY(QmlMetaType::copy(QMetaType::QBitmap , &v, &v2)); 
        QVERIFY(v.size() == QBitmap(100,100).size());
    }

    {
        QCursor v = QCursor(Qt::SizeFDiagCursor); QCursor v2 = QCursor(Qt::SizeFDiagCursor);
        QVERIFY(QmlMetaType::copy(QMetaType::QCursor, &v, 0)); 
        QVERIFY(v.shape() == QCursor().shape());
        QVERIFY(QmlMetaType::copy(QMetaType::QCursor , &v, &v2)); 
        QVERIFY(v.shape() == QCursor(Qt::SizeFDiagCursor).shape());
    }

    QT_COPY_TEST(QSizePolicy, QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    QT_COPY_TEST(QKeySequence, QKeySequence("Ctrl+O"));
    QT_COPY_TEST(QPen, QPen(Qt::red));
    QT_COPY_TEST(QTextLength, QTextLength(QTextLength::FixedLength, 10.2));
    QT_COPY_TEST(QTextFormat, QTextFormat(QTextFormat::ListFormat));
    QT_COPY_TEST(QMatrix, QMatrix().translate(10, 10));
    QT_COPY_TEST(QTransform, QTransform().translate(10, 10));
    QT_COPY_TEST(QMatrix4x4, QMatrix4x4(1,0,2,3,0,1,0,0,9,0,1,0,0,0,10,1));
    QT_COPY_TEST(QVector2D, QVector2D(10.2, 1));
    QT_COPY_TEST(QVector3D, QVector3D(10.2, 1, -2));
    QT_COPY_TEST(QVector4D, QVector4D(10.2, 1, -2, 1.2));
    QT_COPY_TEST(QQuaternion, QQuaternion(1.0, 10.2, 1, -2));

    int voidValue;
    COPY_TEST(void *, VoidStar, (void *)&voidValue, (void *)0);
    COPY_TEST(long, Long, 10, 0);
    COPY_TEST(short, Short, 10, 0);
    COPY_TEST(char, Char, 'a', 0);
    COPY_TEST(unsigned long, ULong, 10, 0);
    COPY_TEST(unsigned short, UShort, 10, 0);
    COPY_TEST(unsigned char, UChar, 'a', 0);
    COPY_TEST(float, Float, 10.5, 0);

    QObject objectValue;
    QWidget widgetValue;
    COPY_TEST(QObject *, QObjectStar, &objectValue, 0);
    COPY_TEST(QWidget *, QWidgetStar, &widgetValue, 0);
    COPY_TEST(qreal, QReal, 10.2, 0);

    {
        QVariant tv = QVariant::fromValue(QVariant(10));
        QVariant v(tv); QVariant v2(tv);
        QVERIFY(QmlMetaType::copy(qMetaTypeId<QVariant>(), &v, 0)); 
        QVERIFY(v == QVariant());
        QVERIFY(QmlMetaType::copy(qMetaTypeId<QVariant>(), &v, &v2)); 
        QVERIFY(v == tv);
    }

    {
        TestType t;  QVariant tv = QVariant::fromValue(&t);

        QVariant v(tv); QVariant v2(tv);
        QVERIFY(QmlMetaType::copy(qMetaTypeId<TestType *>(), &v, 0)); 
        QVERIFY(v == QVariant::fromValue((TestType *)0));
        QVERIFY(QmlMetaType::copy(qMetaTypeId<TestType *>(), &v, &v2)); 
        QVERIFY(v == tv);
    }
}

void tst_qmlmetatype::qmlParserStatusCast()
{
    QVERIFY(QmlMetaType::qmlType(QVariant::Int) == 0);
    QVERIFY(QmlMetaType::qmlType(qMetaTypeId<TestType *>()) != 0);
    QCOMPARE(QmlMetaType::qmlType(qMetaTypeId<TestType *>())->parserStatusCast(), -1);
    QVERIFY(QmlMetaType::qmlType(qMetaTypeId<ValueSourceTestType *>()) != 0);
    QCOMPARE(QmlMetaType::qmlType(qMetaTypeId<ValueSourceTestType *>())->parserStatusCast(), -1);
            
    QVERIFY(QmlMetaType::qmlType(qMetaTypeId<ParserStatusTestType *>()) != 0);
    int cast = QmlMetaType::qmlType(qMetaTypeId<ParserStatusTestType *>())->parserStatusCast();
    QVERIFY(cast != -1);
    QVERIFY(cast != 0);

    ParserStatusTestType t;
    QVERIFY(reinterpret_cast<char *>((QObject *)&t) != reinterpret_cast<char *>((QmlParserStatus *)&t));

    QmlParserStatus *status = reinterpret_cast<QmlParserStatus *>(reinterpret_cast<char *>((QObject *)&t) + cast);
    QCOMPARE(status, &t);
}

void tst_qmlmetatype::qmlPropertyValueSourceCast()
{
    QVERIFY(QmlMetaType::qmlType(QVariant::Int) == 0);
    QVERIFY(QmlMetaType::qmlType(qMetaTypeId<TestType *>()) != 0);
    QCOMPARE(QmlMetaType::qmlType(qMetaTypeId<TestType *>())->propertyValueSourceCast(), -1);
    QVERIFY(QmlMetaType::qmlType(qMetaTypeId<ParserStatusTestType *>()) != 0);
    QCOMPARE(QmlMetaType::qmlType(qMetaTypeId<ParserStatusTestType *>())->propertyValueSourceCast(), -1);
            
    QVERIFY(QmlMetaType::qmlType(qMetaTypeId<ValueSourceTestType *>()) != 0);
    int cast = QmlMetaType::qmlType(qMetaTypeId<ValueSourceTestType *>())->propertyValueSourceCast();
    QVERIFY(cast != -1);
    QVERIFY(cast != 0);

    ValueSourceTestType t;
    QVERIFY(reinterpret_cast<char *>((QObject *)&t) != reinterpret_cast<char *>((QmlPropertyValueSource *)&t));

    QmlPropertyValueSource *source = reinterpret_cast<QmlPropertyValueSource *>(reinterpret_cast<char *>((QObject *)&t) + cast);
    QCOMPARE(source, &t);
}

void tst_qmlmetatype::qmlPropertyValueInterceptorCast()
{
    QVERIFY(QmlMetaType::qmlType(QVariant::Int) == 0);
    QVERIFY(QmlMetaType::qmlType(qMetaTypeId<TestType *>()) != 0);
    QCOMPARE(QmlMetaType::qmlType(qMetaTypeId<TestType *>())->propertyValueInterceptorCast(), -1);
    QVERIFY(QmlMetaType::qmlType(qMetaTypeId<ParserStatusTestType *>()) != 0);
    QCOMPARE(QmlMetaType::qmlType(qMetaTypeId<ParserStatusTestType *>())->propertyValueInterceptorCast(), -1);
            
    QVERIFY(QmlMetaType::qmlType(qMetaTypeId<ValueInterceptorTestType *>()) != 0);
    int cast = QmlMetaType::qmlType(qMetaTypeId<ValueInterceptorTestType *>())->propertyValueInterceptorCast();
    QVERIFY(cast != -1);
    QVERIFY(cast != 0);

    ValueInterceptorTestType t;
    QVERIFY(reinterpret_cast<char *>((QObject *)&t) != reinterpret_cast<char *>((QmlPropertyValueInterceptor *)&t));

    QmlPropertyValueInterceptor *interceptor = reinterpret_cast<QmlPropertyValueInterceptor *>(reinterpret_cast<char *>((QObject *)&t) + cast);
    QCOMPARE(interceptor, &t);
}

void tst_qmlmetatype::isList()
{
    QCOMPARE(QmlMetaType::isList(QVariant()), false);
    QCOMPARE(QmlMetaType::isList(QVariant::Invalid), false);
    QCOMPARE(QmlMetaType::isList(QVariant::Int), false);
    QCOMPARE(QmlMetaType::isList(QVariant(10)), false);

    QList<TestType *> list;
    QmlConcreteList<TestType *> qmllist;

    QCOMPARE(QmlMetaType::isList(qMetaTypeId<QList<TestType *>*>()), true);
    QCOMPARE(QmlMetaType::isList(QVariant::fromValue(&list)), true);
    QCOMPARE(QmlMetaType::isList(qMetaTypeId<QmlList<TestType *>*>()), false);
    QCOMPARE(QmlMetaType::isList(QVariant::fromValue((QmlList<TestType *>*)&qmllist)), false);
}

void tst_qmlmetatype::isQmlList()
{
    QCOMPARE(QmlMetaType::isQmlList(QVariant::Invalid), false);
    QCOMPARE(QmlMetaType::isQmlList(QVariant::Int), false);

    QCOMPARE(QmlMetaType::isQmlList(qMetaTypeId<QList<TestType *>*>()), false);
    QCOMPARE(QmlMetaType::isQmlList(qMetaTypeId<QmlList<TestType *>*>()), true);
}

void tst_qmlmetatype::listCount()
{
    QCOMPARE(QmlMetaType::listCount(QVariant()), 0);
    QCOMPARE(QmlMetaType::listCount(QVariant(10)), 0);

    QList<TestType *> list; 
    QVariant listVar = QVariant::fromValue(&list);
    QmlConcreteList<TestType *> qmllist;
    QVariant qmllistVar = QVariant::fromValue((QmlList<TestType *>*)&qmllist);

    QCOMPARE(QmlMetaType::listCount(listVar), 0);
    QCOMPARE(QmlMetaType::listCount(qmllistVar), 0);

    list.append(0); list.append(0); list.append(0);
    qmllist.append(0); qmllist.append(0); qmllist.append(0);

    QCOMPARE(QmlMetaType::listCount(listVar), 3);
    QCOMPARE(QmlMetaType::listCount(qmllistVar), 0);
}

void tst_qmlmetatype::listAt()
{
    QCOMPARE(QmlMetaType::listAt(QVariant(), 0), QVariant());
    QCOMPARE(QmlMetaType::listAt(QVariant(10), 0), QVariant());
    QCOMPARE(QmlMetaType::listAt(QVariant(), 10), QVariant());
    QCOMPARE(QmlMetaType::listAt(QVariant(10), 10), QVariant());
    QCOMPARE(QmlMetaType::listAt(QVariant(), -10), QVariant());
    QCOMPARE(QmlMetaType::listAt(QVariant(10), -10), QVariant());

    QList<TestType *> list; 
    QVariant listVar = QVariant::fromValue(&list);
    QmlConcreteList<TestType *> qmllist;
    QVariant qmllistVar = QVariant::fromValue((QmlList<TestType *>*)&qmllist);

    QCOMPARE(QmlMetaType::listAt(listVar, 0), QVariant());
    QCOMPARE(QmlMetaType::listAt(listVar, 2), QVariant());
    QCOMPARE(QmlMetaType::listAt(listVar, -1), QVariant());

    QCOMPARE(QmlMetaType::listAt(qmllistVar, 0), QVariant());
    QCOMPARE(QmlMetaType::listAt(qmllistVar, 2), QVariant());
    QCOMPARE(QmlMetaType::listAt(qmllistVar, -1), QVariant());

    TestType ttype;
    QVariant ttypeVar = QVariant::fromValue(&ttype);
    QVariant nullttypeVar = QVariant::fromValue((TestType *)0);

    list.append(0); list.append(&ttype); list.append(0);
    qmllist.append(0); qmllist.append(&ttype); qmllist.append(0);

    QCOMPARE(QmlMetaType::listAt(listVar, 0), nullttypeVar);
    QCOMPARE(QmlMetaType::listAt(listVar, 1), ttypeVar);
    QCOMPARE(QmlMetaType::listAt(listVar, -1), QVariant());

    QCOMPARE(QmlMetaType::listAt(qmllistVar, 0), QVariant());
    QCOMPARE(QmlMetaType::listAt(qmllistVar, 2), QVariant());
    QCOMPARE(QmlMetaType::listAt(qmllistVar, -1), QVariant());
}

void tst_qmlmetatype::defaultObject()
{
    QVERIFY(QmlMetaType::defaultProperty(&QObject::staticMetaObject).name() == 0);
    QVERIFY(QmlMetaType::defaultProperty(&ParserStatusTestType::staticMetaObject).name() == 0);
    QCOMPARE(QString(QmlMetaType::defaultProperty(&TestType::staticMetaObject).name()), QString("foo"));

    QObject o;
    TestType t;
    ParserStatusTestType p;

    QVERIFY(QmlMetaType::defaultProperty((QObject *)0).name() == 0);
    QVERIFY(QmlMetaType::defaultProperty(&o).name() == 0);
    QVERIFY(QmlMetaType::defaultProperty(&p).name() == 0);
    QCOMPARE(QString(QmlMetaType::defaultProperty(&t).name()), QString("foo"));
}

QTEST_MAIN(tst_qmlmetatype)

#include "tst_qmlmetatype.moc"
