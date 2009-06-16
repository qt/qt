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
#include <QtCore>
#include <qtest.h>

#define ITERATION_COUNT 1e5

class tst_qvariant : public QObject
{
    Q_OBJECT
private slots:
    void doubleVariantCreation();
    void floatVariantCreation();
    void rectVariantCreation();
    void stringVariantCreation();
    void doubleVariantSetValue();
    void floatVariantSetValue();
    void rectVariantSetValue();
    void stringVariantSetValue();
    void doubleVariantAssignment();
    void floatVariantAssignment();
    void rectVariantAssignment();
    void stringVariantAssignment();
};


void tst_qvariant::doubleVariantCreation()
{
    double d = 0;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            QVariant v(d);
        }
    }
}

void tst_qvariant::floatVariantCreation()
{
    float f = 0;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            QVariant v(f);
        }
    }
}

void tst_qvariant::rectVariantCreation()
{
    QRect r(1,2,3,4);
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            QVariant v(r);
        }
    }
}

void tst_qvariant::stringVariantCreation()
{
    QString s;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            QVariant v(s);
        }
    }
}

void tst_qvariant::doubleVariantSetValue()
{
    double d = 0;
    QVariant v;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            qVariantSetValue(v, d);
        }
    }
}

void tst_qvariant::floatVariantSetValue()
{
    float f = 0;
    QVariant v;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            qVariantSetValue(v, f);
        }
    }
}

void tst_qvariant::rectVariantSetValue()
{
    QRect r;
    QVariant v;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            qVariantSetValue(v, r);
        }
    }
}

void tst_qvariant::stringVariantSetValue()
{
    QString s;
    QVariant v;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            qVariantSetValue(v, s);
        }
    }
}

void tst_qvariant::doubleVariantAssignment()
{
    double d = 0;
    QVariant v;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            v = d;
        }
    }
}

void tst_qvariant::floatVariantAssignment()
{
    float f = 0;
    QVariant v;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            v = f;
        }
    }
}

void tst_qvariant::rectVariantAssignment()
{
    QRect r;
    QVariant v;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            v = r;
        }
    }
}

void tst_qvariant::stringVariantAssignment()
{
    QString s;
    QVariant v;
    QBENCHMARK {
        for(int i = 0; i < ITERATION_COUNT; ++i) {
            v = s;
        }
    }
}

QTEST_MAIN(tst_qvariant)

#include "tst_qvariant.moc"
