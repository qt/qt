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
#include <QtCore/QDate>
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtGui>
#include <windows.h>
#include <ddhelper.h>



class tst_WindowsMobile : public QObject
{
    Q_OBJECT
public:
    tst_WindowsMobile()
    {
#ifdef Q_OS_WINCE_WM
        q_initDD();
#endif
    }

#ifdef Q_OS_WINCE_WM
    private slots:
        void testMainWindowAndMenuBar();
        void testSimpleWidget();
#endif
};

#ifdef Q_OS_WINCE_WM

bool qt_wince_is_platform(const QString &platformString) {
    TCHAR tszPlatform[64];
    if (SystemParametersInfo(SPI_GETPLATFORMTYPE,
                             sizeof(tszPlatform)/sizeof(*tszPlatform),tszPlatform,0))
      if (0 == _tcsicmp(reinterpret_cast<const wchar_t *> (platformString.utf16()), tszPlatform))
            return true;
    return false;
}

bool qt_wince_is_smartphone() {
       return qt_wince_is_platform(QString::fromLatin1("Smartphone"));
}

void openMenu()
{
    ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,450,630,0,0);
    ::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,450,630,0,0);
    QTest::qWait(2000);
    ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,65535,65535,0,0);
    ::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,65535,65535,0,0);
    QTest::qWait(2000);
    ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,55535,55535,0,0);
    ::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,55535,55535,0,0);
    QTest::qWait(2000);
    ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,55535,58535,0,0);
    ::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,55535,58535,0,0);
    QTest::qWait(2000);
    ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,40535,55535,0,0);
    ::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,40535,55535,0,0);
    QTest::qWait(2000);
    ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,32535,55535,0,0);
    ::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,32535,55535,0,0);
    QTest::qWait(2000);
    ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,65535,65535,0,0);
    ::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,65535,65535,0,0);
    QTest::qWait(2000);
    ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,55535,50535,0,0);
    ::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,55535,50535,0,0);
    QTest::qWait(2000);
    ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,55535,40535,0,0);
    ::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,55535,40535,0,0);
    QTest::qWait(2000);
    ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,48535,45535,0,0);
    QTest::qWait(2000);
    ::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,48535,45535,0,0);
}

void compareScreenshots(const QString &image1, const QString &image2)
{
    if (qt_wince_is_smartphone())
        QSKIP("This test is only for Windows Mobile", SkipAll);
    QImage screenShot(image1);
    QImage original(image2);

    //ignore the clock
    QPainter p1(&screenShot);
    QPainter p2(&original);
    p1.fillRect(310, 6, 400, 34, Qt::black);
    p2.fillRect(310, 6, 400, 34, Qt::black);

    QVERIFY(original == screenShot);
}

void takeScreenShot(const QString filename)
{
    q_lock();
    QImage image = QImage(( uchar *) q_frameBuffer(), q_screenWidth(),
        q_screenHeight(), q_screenWidth() * q_screenDepth() / 8, QImage::Format_RGB16);
    image.save(filename, "PNG");
    q_unlock();
}

void tst_WindowsMobile::testMainWindowAndMenuBar()
{
    QProcess process;
    process.start("testQMenuBar.exe");
    QCOMPARE(process.state(), QProcess::Running);
    QTest::qWait(6000);
    openMenu();
    QTest::qWait(1000);
    takeScreenShot("testQMenuBar_current.png");
    process.close();
    compareScreenshots("testQMenuBar_current.png", ":/testQMenuBar_current.png");
}

void tst_WindowsMobile::testSimpleWidget()
{
    QMenuBar menubar;
    menubar.show();
    QWidget maximized;
    QPalette pal = maximized.palette();
    pal.setColor(QPalette::Background, Qt::red);
    maximized.setPalette(pal);
    maximized.showMaximized();
    QWidget widget;
    widget.setGeometry(100, 100, 200, 200);
    widget.setWindowTitle("Widget");
    widget.show();
    qApp->processEvents();
    QTest::qWait(1000);

    QWidget widget2;
    widget2.setGeometry(100, 380, 300, 200);
    widget2.setWindowTitle("Widget 2");
    widget2.setWindowFlags(Qt::Popup);
    widget2.show();

    qApp->processEvents();
    QTest::qWait(1000);
    takeScreenShot("testSimpleWidget_current.png");
    compareScreenshots("testSimpleWidget_current.png", ":/testSimpleWidget_current.png");
}


#endif //Q_OS_WINCE_WM


QTEST_MAIN(tst_WindowsMobile)
#include "tst_windowsmobile.moc"

