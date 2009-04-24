/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include "../../shared/util.h"

#include <qinputcontext.h>
#include <qlineedit.h>
#include <qplaintextedit.h>

class tst_QInputContext : public QObject
{
Q_OBJECT

public:
    tst_QInputContext() {}
    virtual ~tst_QInputContext() {}

public slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void init() {}
    void cleanup() {}
private slots:
    void maximumTextLength();
    void filterMouseEvents();
};

void tst_QInputContext::maximumTextLength()
{
    QLineEdit le;

    le.setMaxLength(15);
    QVariant variant = le.inputMethodQuery(Qt::ImMaximumTextLength);
    QVERIFY(variant.isValid());
    QCOMPARE(variant.toInt(), 15);

    QPlainTextEdit pte;
    // For BC/historical reasons, QPlainTextEdit::inputMethodQuery is protected.
    variant = static_cast<QWidget *>(&pte)->inputMethodQuery(Qt::ImMaximumTextLength);
    QVERIFY(!variant.isValid());
}

class QFilterInputContext : public QInputContext
{
public:
    QFilterInputContext() : successful(false) {}
    ~QFilterInputContext() {}

    QString identifierName() { return QString(); }
    QString language() { return QString(); }

    void reset() {}

    bool isComposing() const { return false; }

    bool filterEvent( const QEvent *event )
    {
        successful = event->type() == QEvent::MouseButtonRelease;
    }

public:
    bool successful;
};

void tst_QInputContext::filterMouseEvents()
{
    QLineEdit le;
    le.show();

    QFilterInputContext *ic = new QFilterInputContext;
    le.setInputContext(ic);
    QTest::mouseClick(&le, Qt::LeftButton);

    QVERIFY(ic->successful);

    le.setInputContext(0);
}

QTEST_MAIN(tst_QInputContext)
#include "tst_qinputcontext.moc"
