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

class tst_qmlmetatype : public QObject
{
    Q_OBJECT
public:
    tst_qmlmetatype() {}

private slots:
    void copy();
};

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
    QT_COPY_TEST(QMatrix, QMatrix().translate(10, 10));
    QT_COPY_TEST(QTransform, QTransform().translate(10, 10));
    QT_COPY_TEST(QMatrix4x4, QMatrix4x4().translate(10, 10));
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
}

QTEST_MAIN(tst_qmlmetatype)

#include "tst_qmlmetatype.moc"
