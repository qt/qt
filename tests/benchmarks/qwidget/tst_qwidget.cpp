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
#include <QtGui>

class tst_QWidget : public QObject
{
    Q_OBJECT


private slots:
    void update_data();
    void updateOpaque_data();
    void updateOpaque();
    void updateTransparent_data();
    void updateTransparent();
    void updatePartial_data();
    void updatePartial();
    void updateComplex_data();
    void updateComplex();

    void complexToplevelResize();
};

class UpdateWidget : public QWidget
{
public:
    UpdateWidget(int rows, int columns) : QWidget(0)
    {
        QGridLayout *layout = new QGridLayout;
        for (int row = 0; row < rows; ++row) {
            for (int column = 0; column < columns; ++column) {
                UpdateWidget *widget = new UpdateWidget;
                widget->setFixedSize(20, 20);
                layout->addWidget(widget, row, column);
                children.append(widget);
            }
        }
        setLayout(layout);
    }

    UpdateWidget(QWidget *parent = 0) : QWidget(parent) {}

    void paintEvent(QPaintEvent *)
    {
        static int color = Qt::black;

        QPainter painter(this);
        painter.fillRect(rect(), Qt::GlobalColor(color));

        if (++color > Qt::darkYellow)
            color = Qt::black;
    }

    QRegion updateRegion;
    QList<UpdateWidget*> children;
};

void tst_QWidget::update_data()
{
    QTest::addColumn<int>("rows");
    QTest::addColumn<int>("columns");
    QTest::addColumn<int>("numUpdates");

    QTest::newRow("10x10x1") << 10 << 10 << 1;
    QTest::newRow("10x10x10") << 10 << 10 << 10;
    QTest::newRow("25x25x1") << 25 << 25 << 1;
    QTest::newRow("25x25x10") << 25 << 25 << 10;
    QTest::newRow("25x25x100") << 25 << 25 << 100;
}

void tst_QWidget::updateOpaque_data()
{
    update_data();
}

void tst_QWidget::updateOpaque()
{
    QFETCH(int, rows);
    QFETCH(int, columns);
    QFETCH(int, numUpdates);

    UpdateWidget widget(rows, columns);
    foreach (QWidget *w, widget.children) {
        w->setAttribute(Qt::WA_OpaquePaintEvent);
    }

    widget.show();
    QApplication::processEvents();

    int i = 0;
    const int n = widget.children.size();
    QBENCHMARK {
        for (int j = 0; j < numUpdates; ++j) {
            widget.children[i]->update();
            QApplication::processEvents();
            i = (i + 1) % n;
        }
    }
}

void tst_QWidget::updateTransparent_data()
{
    update_data();
}

void tst_QWidget::updateTransparent()
{
    QFETCH(int, rows);
    QFETCH(int, columns);
    QFETCH(int, numUpdates);

    UpdateWidget widget(rows, columns);
    widget.show();
    QApplication::processEvents();

    int i = 0;
    const int n = widget.children.size();
    QBENCHMARK {
        for (int j = 0; j < numUpdates; ++j) {
            widget.children[i]->update();
            QApplication::processEvents();
            i = (i + 1) % n;
        }
    }
}

void tst_QWidget::updatePartial_data()
{
    update_data();
}

void tst_QWidget::updatePartial()
{
    QFETCH(int, rows);
    QFETCH(int, columns);
    QFETCH(int, numUpdates);

    UpdateWidget widget(rows, columns);
    widget.show();
    QApplication::processEvents();

    int i = 0;
    const int n = widget.children.size();
    QBENCHMARK {
        for (int j = 0; j < numUpdates; ++j) {
            QWidget *w = widget.children[i];
            const int x = w->width() / 2;
            const int y = w->height() / 2;
            w->update(0, 0, x, y);
            w->update(x, 0, x, y);
            w->update(0, y, x, y);
            w->update(x, y, x, y);
            QApplication::processEvents();
            i = (i + 1) % n;
        }
    }
}

void tst_QWidget::updateComplex_data()
{
    update_data();
}

void tst_QWidget::updateComplex()
{
    QFETCH(int, rows);
    QFETCH(int, columns);
    QFETCH(int, numUpdates);

    UpdateWidget widget(rows, columns);
    widget.show();
    QApplication::processEvents();

    int i = 0;
    const int n = widget.children.size();
    QBENCHMARK {
        for (int j = 0; j < numUpdates; ++j) {
            QWidget *w = widget.children[i];
            const int x = w->width() / 2;
            const int y = w->height() / 2;
            w->update(QRegion(0, 0, x, y, QRegion::Ellipse));
            w->update(QRegion(x, y, x, y, QRegion::Ellipse));
            QApplication::processEvents();
            i = (i + 1) % n;
        }
    }
}

class ResizeWidget : public QWidget
{
public:
    ResizeWidget();
};

ResizeWidget::ResizeWidget() : QWidget(0)
{
    QBoxLayout *topLayout = new QVBoxLayout;

    QMenuBar *menubar = new QMenuBar;
    QMenu* popup = menubar->addMenu("&File");
    popup->addAction("&Quit", qApp, SLOT(quit()));
    topLayout->setMenuBar(menubar);

    QBoxLayout *buttons = new QHBoxLayout;
    buttons->setMargin(5);
    buttons->addStretch(10);
    for (int i = 1; i <= 4; i++ ) {
        QPushButton* button = new QPushButton;
        button->setText(QString("Button %1").arg(i));
        buttons->addWidget(button);
    }
    topLayout->addLayout(buttons);

    buttons = new QHBoxLayout;
    buttons->addStretch(10);
    for (int i = 11; i <= 16; i++) {
        QPushButton* button = new QPushButton;
        button->setText(QString("Button %1").arg(i));
        buttons->addWidget(button);
    }
    topLayout->addLayout(buttons);

    QBoxLayout *buttons2 = new QHBoxLayout;
    buttons2->addStretch(10);
    topLayout->addLayout(buttons2);

    QPushButton *button = new QPushButton;
    button->setText("Button five");
    buttons2->addWidget(button);

    button = new QPushButton;
    button->setText("Button 6");
    buttons2->addWidget(button);

    QTextEdit *bigWidget = new QTextEdit;
    bigWidget->setText("This widget will get all the remaining space");
    bigWidget->setFrameStyle(QFrame::Panel | QFrame::Plain);
    topLayout->addWidget(bigWidget);

    const int numRows = 6;
    const int labelCol = 0;
    const int linedCol = 1;
    const int multiCol = 2;

    QGridLayout *grid = new QGridLayout;
    for (int row = 0; row < numRows; row++) {
        QLineEdit *lineEdit = new QLineEdit;
        grid->addWidget(lineEdit, row, linedCol);
        QLabel *label = new QLabel(QString("Line &%1").arg(row + 1));
        grid->addWidget(label, row, labelCol);
    }
    topLayout->addLayout(grid);

    QTextEdit *multiLineEdit = new QTextEdit;
    grid->addWidget(multiLineEdit, 0, labelCol + 1, multiCol, multiCol);

    grid->setColumnStretch(linedCol, 10);
    grid->setColumnStretch(multiCol, 20);

    QLabel* statusBar = new QLabel;
    statusBar->setText("Let's pretend this is a status bar");
    statusBar->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusBar->setFixedHeight(statusBar->sizeHint().height());
    statusBar->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    topLayout->addWidget(statusBar);

    topLayout->activate();
    setLayout(topLayout);
}

void tst_QWidget::complexToplevelResize()
{
    ResizeWidget w;
    w.show();

    QApplication::processEvents();

    const int minSize = 100;
    const int maxSize = 800;
    int size = minSize;

    QBENCHMARK {
        w.resize(size, size);
        size = qMax(minSize, (size + 10) % maxSize);
        QApplication::processEvents();
        QApplication::processEvents();
    }
}

QTEST_MAIN(tst_QWidget)

#include "tst_qwidget.moc"
