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
#include <QtCore>

class tst_QtIcoImageFormat : public QObject
{
    Q_OBJECT

public:
    tst_QtIcoImageFormat();
    virtual ~tst_QtIcoImageFormat();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void format();
    void canRead_data();
    void canRead();
    void SequentialFile_data();
    void SequentialFile();
    void imageCount_data();
    void imageCount();
    void jumpToNextImage_data();
    void jumpToNextImage();
    void loopCount_data();
    void loopCount();
    void nextImageDelay_data();
    void nextImageDelay();

private:
    QString m_IconPath;
};


tst_QtIcoImageFormat::tst_QtIcoImageFormat()
{
    m_IconPath = QLatin1String(SRCDIR) + "/icons";
    qDebug() << m_IconPath;
}

tst_QtIcoImageFormat::~tst_QtIcoImageFormat()
{

}

void tst_QtIcoImageFormat::init()
{

}

void tst_QtIcoImageFormat::cleanup()
{

}

void tst_QtIcoImageFormat::initTestCase()
{

}

void tst_QtIcoImageFormat::cleanupTestCase()
{

}

void tst_QtIcoImageFormat::format()
{
    QImageReader reader(m_IconPath + "/valid/35FLOPPY.ICO", "ico");
    QByteArray fmt = reader.format();
    QCOMPARE(const_cast<const char*>(fmt.data()), "ico" );
}

void tst_QtIcoImageFormat::canRead_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("isValid");

    QTest::newRow("floppy (16px,32px - 16 colors)") << "valid/35FLOPPY.ICO" << 1;
    QTest::newRow("16px,32px,48px - 256,16M colors") << "valid/abcardWindow.ico" << 1;
    QTest::newRow("16px - 16 colors") << "valid/App.ico" << 1;
    QTest::newRow("16px,32px,48px - 16,256,16M colors") << "valid/Obj_N2_Internal_Mem.ico" << 1;
    QTest::newRow("16px - 16,256,16M colors") << "valid/Status_Play.ico" << 1;
    QTest::newRow("16px,32px - 16 colors") << "valid/TIMER01.ICO" << 1;
    QTest::newRow("16px16c, 32px32c, 32px256c") << "valid/WORLD.ico" << 1;
    QTest::newRow("16px16c, 32px32c, 32px256c") << "valid/WORLDH.ico" << 1;
    QTest::newRow("invalid floppy (first 8 bytes = 0xff)") << "invalid/35floppy.ico" << 0;
    QTest::newRow("103x16px, 24BPP") << "valid/trolltechlogo_tiny.ico" << 1;
    QTest::newRow("includes 32BPP w/alpha") << "valid/semitransparent.ico" << 1;
}

void tst_QtIcoImageFormat::canRead()
{
    QFETCH(QString, fileName);
    QFETCH(int, isValid);

    QImageReader reader(m_IconPath + "/" + fileName);
    QCOMPARE(reader.canRead(), (isValid == 0 ? false : true));
}

class QSequentialFile : public QFile
{
public:
    QSequentialFile(const QString &name) : QFile(name) {}

    virtual ~QSequentialFile() {}

    virtual bool isSequential() const {
        return true;
    }

};

void tst_QtIcoImageFormat::SequentialFile_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("isValid");

    QTest::newRow("floppy (16,32 pixels - 16 colors)") << "valid/35FLOPPY.ICO" << 1;

    QTest::newRow("invalid floppy (first 8 bytes = 0xff)") << "invalid/35floppy.ico" << 0;


}

void tst_QtIcoImageFormat::SequentialFile()
{
    QFETCH(QString, fileName);
    QFETCH(int, isValid);

    QSequentialFile *file = new QSequentialFile(m_IconPath + "/" + fileName);
    QVERIFY(file);
    QVERIFY(file->open(QFile::ReadOnly));
    QImageReader reader(file);

    // Perform the check twice. If canRead() does not restore the sequential device back to its original state,
    // it will fail on the second try.
    QCOMPARE(reader.canRead(), (isValid == 0 ? false : true));
    QCOMPARE(reader.canRead(), (isValid == 0 ? false : true));
    file->close();
}


void tst_QtIcoImageFormat::imageCount_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("count");

    QTest::newRow("floppy (16px,32px - 16 colors)") << "valid/35FLOPPY.ICO" << 2;
    QTest::newRow("16px,32px,48px - 256,16M colors") << "valid/abcardWindow.ico" << 6;
    QTest::newRow("16px - 16 colors") << "valid/App.ico" << 1;
    QTest::newRow("16px,32px,48px - 16,256,16M colors") << "valid/Obj_N2_Internal_Mem.ico" << 9;
    QTest::newRow("16px - 16,256,16M colors") << "valid/Status_Play.ico" << 3;
    QTest::newRow("16px,32px - 16 colors") << "valid/TIMER01.ICO" << 2;
    QTest::newRow("16px16c, 32px32c, 32px256c") << "valid/WORLD.ico" << 3;
    QTest::newRow("16px16c, 32px32c, 32px256c") << "valid/WORLDH.ico" << 3;
    QTest::newRow("invalid floppy (first 8 bytes = 0xff)") << "invalid/35floppy.ico" << 0;
    QTest::newRow("includes 32BPP w/alpha") << "valid/semitransparent.ico" << 9;

}

void tst_QtIcoImageFormat::imageCount()
{
    QFETCH(QString, fileName);
    QFETCH(int, count);

    QImageReader reader(m_IconPath + "/" + fileName);
    QCOMPARE(reader.imageCount(), count);

}

void tst_QtIcoImageFormat::jumpToNextImage_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("count");

    QTest::newRow("floppy (16px,32px - 16 colors)") << "valid/35FLOPPY.ICO" << 2;
    QTest::newRow("16px,32px,48px - 256,16M colors") << "valid/abcardWindow.ico" << 6;
    QTest::newRow("16px - 16 colors") << "valid/App.ico" << 1;
    QTest::newRow("16px,32px,48px - 16,256,16M colors") << "valid/Obj_N2_Internal_Mem.ico" << 9;
    QTest::newRow("16px - 16,256,16M colors") << "valid/Status_Play.ico" << 3;
    QTest::newRow("16px,32px - 16 colors") << "valid/TIMER01.ICO" << 2;
    QTest::newRow("16px16c, 32px32c, 32px256c") << "valid/WORLD.ico" << 3;
    QTest::newRow("16px16c, 32px32c, 32px256c") << "valid/WORLDH.ico" << 3;
    QTest::newRow("includes 32BPP w/alpha") << "valid/semitransparent.ico" << 9;
}

void tst_QtIcoImageFormat::jumpToNextImage()
{
    QFETCH(QString, fileName);
    QFETCH(int, count);

    QImageReader reader(m_IconPath + "/" + fileName);
    bool bJumped = reader.jumpToImage(0);
    while (bJumped) {
        count--;
        bJumped = reader.jumpToNextImage();
    }
    QCOMPARE(count, 0);
}

void tst_QtIcoImageFormat::loopCount_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("count");

    QTest::newRow("floppy (16px,32px - 16 colors)") << "valid/35FLOPPY.ICO" << 0;
    QTest::newRow("invalid floppy (first 8 bytes = 0xff)") << "invalid/35floppy.ico" << 0;
}

void tst_QtIcoImageFormat::loopCount()
{
    QFETCH(QString, fileName);
    QFETCH(int, count);

    QImageReader reader(m_IconPath + "/" + fileName);
    QCOMPARE(reader.loopCount(), count);
}

void tst_QtIcoImageFormat::nextImageDelay_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("count");

    QTest::newRow("floppy (16px,32px - 16 colors)") << "valid/35FLOPPY.ICO" << 2;
    QTest::newRow("16px,32px,48px - 256,16M colors") << "valid/abcardWindow.ico" << 6;
    QTest::newRow("16px - 16 colors") << "valid/App.ico" << 1;
    QTest::newRow("16px,32px,48px - 16,256,16M colors") << "valid/Obj_N2_Internal_Mem.ico" << 9;
    QTest::newRow("16px - 16,256,16M colors") << "valid/Status_Play.ico" << 3;
    QTest::newRow("16px,32px - 16 colors") << "valid/TIMER01.ICO" << 2;
    QTest::newRow("16px16c, 32px32c, 32px256c") << "valid/WORLD.ico" << 3;
    QTest::newRow("16px16c, 32px32c, 32px256c") << "valid/WORLDH.ico" << 3;
    QTest::newRow("invalid floppy (first 8 bytes = 0xff)") << "invalid/35floppy.ico" << -1;
    QTest::newRow("includes 32BPP w/alpha") << "valid/semitransparent.ico" << 9;
}

void tst_QtIcoImageFormat::nextImageDelay()
{
    QFETCH(QString, fileName);
    QFETCH(int, count);

    QImageReader reader(m_IconPath + "/" + fileName);
    if (count == -1) {
        QCOMPARE(reader.nextImageDelay(), 0);
    } else {
        int i;
        for (i = 0; i < count; i++) {
            QVERIFY(reader.jumpToImage(i));
            QCOMPARE(reader.nextImageDelay(), 0);
        }
    }
}

QTEST_MAIN(tst_QtIcoImageFormat)
#include "tst_qticoimageformat.moc"
