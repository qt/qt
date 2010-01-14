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

#include <QtCore>
#include <QtGui/QPixmap>
#include <qtest.h>

#define ITERATION_COUNT 1e5

class tst_qvariant : public QObject
{
    Q_OBJECT
private slots:
    void testBound();

    void doubleVariantCreation();
    void floatVariantCreation();
    void rectVariantCreation();
    void stringVariantCreation();
    void pixmapVariantCreation();

    void doubleVariantSetValue();
    void floatVariantSetValue();
    void rectVariantSetValue();
    void stringVariantSetValue();

    void doubleVariantAssignment();
    void floatVariantAssignment();
    void rectVariantAssignment();
    void stringVariantAssignment();
};

void tst_qvariant::testBound()
{
    qreal d = qreal(.5);
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            d = qBound<qreal>(0, d, 1);
        }
    }
}

template <typename T>
static void variantCreation(T val)
{
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            QVariant v(val);
        }
    }
}

void tst_qvariant::doubleVariantCreation()
{
    variantCreation<double>(0.0);
}

void tst_qvariant::floatVariantCreation()
{
    variantCreation<float>(0.0f);
}

void tst_qvariant::rectVariantCreation()
{
    variantCreation<QRect>(QRect(1, 2, 3, 4));
}

void tst_qvariant::stringVariantCreation()
{
    variantCreation<QString>(QString());
}

void tst_qvariant::pixmapVariantCreation()
{
    variantCreation<QPixmap>(QPixmap());
}

template <typename T>
static void variantSetValue(T d)
{
    QVariant v;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            qVariantSetValue(v, d);
        }
    }
}

void tst_qvariant::doubleVariantSetValue()
{
    variantSetValue<double>(0.0);
}

void tst_qvariant::floatVariantSetValue()
{
    variantSetValue<float>(0.0f);
}

void tst_qvariant::rectVariantSetValue()
{
    variantSetValue<QRect>(QRect());
}

void tst_qvariant::stringVariantSetValue()
{
    variantSetValue<QString>(QString());
}

template <typename T>
static void variantAssignment(T d)
{
    QVariant v;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            v = d;
        }
    }
}

void tst_qvariant::doubleVariantAssignment()
{
    variantAssignment<double>(0.0);
}

void tst_qvariant::floatVariantAssignment()
{
    variantAssignment<float>(0.0f);
}

void tst_qvariant::rectVariantAssignment()
{
    variantAssignment<QRect>(QRect());
}

void tst_qvariant::stringVariantAssignment()
{
    variantAssignment<QString>(QString());
}

QTEST_MAIN(tst_qvariant)

#include "tst_qvariant.moc"
