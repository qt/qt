/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#ifdef Q_WS_QWS

//TESTED_CLASS=
//TESTED_FILES=

#include <QWSEmbedWidget>
#include <QLabel>

class tst_QWSEmbedWidget : public QObject
{
    Q_OBJECT

public:
    tst_QWSEmbedWidget() {}
    ~tst_QWSEmbedWidget() {}

private slots:
    void embedWidget();
};

void tst_QWSEmbedWidget::embedWidget()
{
    QLabel embedded("hello");
    embedded.show();
    QApplication::processEvents();
    QVERIFY(embedded.isVisible());

    {
        QWSEmbedWidget embedder(embedded.winId());
        embedder.show();
        QApplication::processEvents();
        QVERIFY(embedded.isVisible());
    }
    QApplication::processEvents();
    QVERIFY(!embedded.isVisible());

    {
        QWidget w;
        embedded.setWindowFlags(Qt::FramelessWindowHint);
        QWSEmbedWidget embedder(embedded.winId(), &w);

        const QRect geometry(100, 100, 100, 100);
        embedder.setGeometry(geometry);
        w.show();

        QApplication::processEvents();

        const QPoint offset = w.mapToGlobal(QPoint(0, 0));
        QCOMPARE(embedded.geometry(), geometry.translated(offset));
        QVERIFY(embedded.isVisible());
    }
    QApplication::processEvents();
    QVERIFY(!embedded.isVisible());
}

QTEST_MAIN(tst_QWSEmbedWidget)

#include "tst_qwsembedwidget.moc"

#else // Q_WS_QWS
QTEST_NOOP_MAIN
#endif
