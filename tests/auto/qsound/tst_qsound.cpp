/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <QtGui>

#if defined(Q_OS_SYMBIAN)
#define SRCDIR ""
#endif

class tst_QSound : public QObject
{
    Q_OBJECT

public:
    tst_QSound( QObject* parent=0) : QObject(parent) {}

private slots:
    void checkFinished();

    // Manual tests
    void staticPlay();
};

void tst_QSound::checkFinished()
{
    QSound sound(SRCDIR"4.wav");
    sound.setLoops(3);
    sound.play();
    QTest::qWait(5000);

#if defined(Q_WS_QWS)
    QEXPECT_FAIL("", "QSound buggy on embedded (task QTBUG-157)", Abort);
#endif
    QVERIFY(sound.isFinished() );
}

void tst_QSound::staticPlay()
{
    QSKIP("Test disabled -- only for manual purposes", SkipAll);

    // Check that you hear sound with static play also.
    QSound::play(SRCDIR"4.wav");
    QTest::qWait(2000);
}

QTEST_MAIN(tst_QSound);
#include "tst_qsound.moc"
