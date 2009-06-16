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
#include <QtGui>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QBoxLayout : public QObject
{
    Q_OBJECT

public:
    tst_QBoxLayout();
    virtual ~tst_QBoxLayout();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void insertSpacerItem();
    void sizeHint();
    void sizeConstraints();
    void setGeometry();
};


tst_QBoxLayout::tst_QBoxLayout()
{
}

tst_QBoxLayout::~tst_QBoxLayout()
{
}

void tst_QBoxLayout::initTestCase()
{
}

void tst_QBoxLayout::cleanupTestCase()
{
}

void tst_QBoxLayout::init()
{
}

void tst_QBoxLayout::cleanup()
{
}

void tst_QBoxLayout::insertSpacerItem()
{
    QWidget *window = new QWidget;

    QSpacerItem *spacer1 = new QSpacerItem(20, 10, QSizePolicy::Expanding, QSizePolicy::Expanding);
    QSpacerItem *spacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

    QBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(new QLineEdit("Foooooooooooooooooooooooooo"));
    layout->addSpacerItem(spacer1);
    layout->addWidget(new QLineEdit("Baaaaaaaaaaaaaaaaaaaaaaaaar"));
    layout->insertSpacerItem(0, spacer2);
    window->setLayout(layout);

    QVERIFY(layout->itemAt(0) == spacer2);
    QVERIFY(layout->itemAt(2) == spacer1);

    window->show();
}

void tst_QBoxLayout::sizeHint()
{
    QWidget *window = new QWidget;
    QHBoxLayout *lay1 = new QHBoxLayout;
    QHBoxLayout *lay2 = new QHBoxLayout;
    QLabel *label = new QLabel("widget twooooooooooooooooooooooooooooooooooooooooooooooooooooooo");
    lay2->addWidget(label);
    lay1->addLayout(lay2);
    window->setLayout(lay1);
    window->show();
    label->setText("foooooooo baaaaaaar");
    QSize sh = lay1->sizeHint();
    QApplication::processEvents();
    // Note that this is not strictly required behaviour - actually
    // the preferred behaviour would be that sizeHint returns
    // the same value regardless of what's lying in the event queue.
    // (i.e. we would check for equality here instead)
    QVERIFY(lay1->sizeHint() != sh);
}

void tst_QBoxLayout::sizeConstraints()
{
    QWidget *window = new QWidget;
    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(new QLabel("foooooooooooooooooooooooooooooooooooo"));
    lay->addWidget(new QLabel("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaar"));
    lay->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(lay);
    window->show();
    QApplication::processEvents();
    QSize sh = window->sizeHint();
    lay->takeAt(1);
    QVERIFY(sh.width() >= window->sizeHint().width() &&
            sh.height() >= window->sizeHint().height());

}

void tst_QBoxLayout::setGeometry()
{
    QWidget w;
    QVBoxLayout *lay = new QVBoxLayout;
    lay->setMargin(0);
    lay->setSpacing(0);
    QHBoxLayout *lay2 = new QHBoxLayout;
    QDial *dial = new QDial;
    lay2->addWidget(dial);
    lay2->setAlignment(Qt::AlignTop);
    lay2->setAlignment(Qt::AlignRight);
    lay->addLayout(lay2);
    w.setLayout(lay);
    w.show();
    
    QRect newGeom(0, 0, 70, 70);
    lay2->setGeometry(newGeom);
    QApplication::processEvents();
    QVERIFY2(newGeom.contains(dial->geometry()), "dial->geometry() should be smaller and within newGeom");
}

QTEST_MAIN(tst_QBoxLayout)
#include "tst_qboxlayout.moc"
