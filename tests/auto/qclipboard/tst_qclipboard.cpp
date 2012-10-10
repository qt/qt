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
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif
#ifdef Q_OS_SYMBIAN
#include "private/qcore_symbian_p.h"
#include "txtetext.h"
#include <baclipb.h>
#endif
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "txtclipboard.h"
#endif

//TESTED_CLASS=
//TESTED_FILES=

class tst_QClipboard : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void copy_exit_paste();
    void capabiliyFunctions();
    void modes();
    void testSignals();
    void setMimeData();
    void clearBeforeSetText();
#ifdef Q_OS_SYMBIAN
    void pasteCopySymbian();
    void copyPasteSymbian();
    void copyCopyPasteSymbian();
#endif

private:
    bool nativeClipboardWorking();

    QString m_copier;
    QString m_paster;
};

void tst_QClipboard::initTestCase()
{
    QDir workingDirectory = QDir::current();
    QString copier = QLatin1String("copier/copier");
    QString paster = QLatin1String("paster/paster");
    // Windows: cd up to be able to locate the binary of the sub-process.
#ifdef Q_OS_WIN
    const QString suffix = QLatin1String(".exe");
    copier.append(suffix);
    paster.append(suffix);
    if (workingDirectory.absolutePath().endsWith(QLatin1String("/debug"), Qt::CaseInsensitive)
        || workingDirectory.absolutePath().endsWith(QLatin1String("/release"), Qt::CaseInsensitive)) {
        QVERIFY(workingDirectory.cdUp());
        QVERIFY(QDir::setCurrent(workingDirectory.absolutePath()));
    }
#endif
    m_copier = workingDirectory.absoluteFilePath(copier);
    m_paster = workingDirectory.absoluteFilePath(paster);
    QVERIFY2(QFileInfo(m_copier).exists(),
             qPrintable(QString::fromLatin1("Copier executable '%1' does not exist!")
                        .arg(QDir::toNativeSeparators(m_copier))));
    QVERIFY2(QFileInfo(m_paster).exists(),
             qPrintable(QString::fromLatin1("Paster executable '%1' does not exist!")
                        .arg(QDir::toNativeSeparators(m_paster))));
}

bool tst_QClipboard::nativeClipboardWorking()
{
#ifdef Q_WS_MAC
    PasteboardRef pasteboard;
    OSStatus status = PasteboardCreate(0, &pasteboard);
    if (status == noErr)
        CFRelease(pasteboard);
    return status == noErr;
#endif
    return true;
}

Q_DECLARE_METATYPE(QClipboard::Mode)

/*
    Tests that the capability functions are implemented on all
    platforms.
*/
void tst_QClipboard::capabiliyFunctions()
{
    QClipboard * const clipboard =  QApplication::clipboard();

    clipboard->supportsSelection();
    clipboard->supportsFindBuffer();
    clipboard->ownsSelection();
    clipboard->ownsClipboard();
    clipboard->ownsFindBuffer();
}

/*
    Test that text inserted into the clipboard in different modes is
    kept separate.
*/
void tst_QClipboard::modes()
{
    QClipboard * const clipboard =  QApplication::clipboard();

    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup", SkipAll);

    const QString defaultMode = "default mode text;";
    clipboard->setText(defaultMode);
    QCOMPARE(clipboard->text(), defaultMode);

    if (clipboard->supportsSelection()) {
        const QString selectionMode = "selection mode text";
        clipboard->setText(selectionMode, QClipboard::Selection);
        QCOMPARE(clipboard->text(QClipboard::Selection), selectionMode);
        QCOMPARE(clipboard->text(), defaultMode);
    }

    if (clipboard->supportsFindBuffer()) {
        const QString searchMode = "find mode text";
        clipboard->setText(searchMode, QClipboard::FindBuffer);
        QCOMPARE(clipboard->text(QClipboard::FindBuffer), searchMode);
        QCOMPARE(clipboard->text(), defaultMode);
    }
}

/*
    Test that the appropriate signals are emitted when the cliboard
    contents is changed by calling the qt functions.
*/
void tst_QClipboard::testSignals()
{
    qRegisterMetaType<QClipboard::Mode>("QClipboard::Mode");

    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup", SkipAll);

    QClipboard * const clipboard =  QApplication::clipboard();

    QSignalSpy changedSpy(clipboard, SIGNAL(changed(QClipboard::Mode)));
    QSignalSpy dataChangedSpy(clipboard, SIGNAL(dataChanged()));
    QSignalSpy searchChangedSpy(clipboard, SIGNAL(findBufferChanged()));
    QSignalSpy selectionChangedSpy(clipboard, SIGNAL(selectionChanged()));

    const QString text = "clipboard text;";

    // Test the default mode signal.
    clipboard->setText(text);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(searchChangedSpy.count(), 0);
    QCOMPARE(selectionChangedSpy.count(), 0);
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(changedSpy.at(0).count(), 1);
    QCOMPARE(qVariantValue<QClipboard::Mode>(changedSpy.at(0).at(0)), QClipboard::Clipboard);

    changedSpy.clear();

    // Test the selection mode signal.
    if (clipboard->supportsSelection()) {
        clipboard->setText(text, QClipboard::Selection);
        QCOMPARE(selectionChangedSpy.count(), 1);
        QCOMPARE(changedSpy.count(), 1);
        QCOMPARE(changedSpy.at(0).count(), 1);
        QCOMPARE(qVariantValue<QClipboard::Mode>(changedSpy.at(0).at(0)), QClipboard::Selection);
    } else {
        QCOMPARE(selectionChangedSpy.count(), 0);
    }
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(searchChangedSpy.count(), 0);

    changedSpy.clear();

    // Test the search mode signal.
    if (clipboard->supportsFindBuffer()) {
        clipboard->setText(text, QClipboard::FindBuffer);
        QCOMPARE(searchChangedSpy.count(), 1);
        QCOMPARE(changedSpy.count(), 1);
        QCOMPARE(changedSpy.at(0).count(), 1);
        QCOMPARE(qVariantValue<QClipboard::Mode>(changedSpy.at(0).at(0)), QClipboard::FindBuffer);
    } else {
        QCOMPARE(searchChangedSpy.count(), 0);
    }
    QCOMPARE(dataChangedSpy.count(), 1);
}

/*
    Test that pasted text remain on the clipboard
    after a Qt application exits.
*/
void tst_QClipboard::copy_exit_paste()
{
#ifndef QT_NO_PROCESS
#if defined Q_WS_X11 || defined Q_WS_QWS || (!defined (Q_OS_QNX) && defined (Q_WS_QPA))
    QSKIP("This test does not make sense on X11 and embedded, copied data disappears from the clipboard when the application exits ", SkipAll);
    // ### It's still possible to test copy/paste - just keep the apps running
#elif defined (Q_OS_SYMBIAN) && defined (Q_CC_NOKIAX86)
    QSKIP("emulator cannot launch multiple processes",SkipAll);
#endif
    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup", SkipAll);
    const QStringList stringArgument = QStringList() << "Test string.";
    QCOMPARE(QProcess::execute(m_copier, stringArgument), 0);
#ifdef Q_WS_MAC
    // The Pasteboard needs a moment to breathe (at least on older Macs).
    QTest::qWait(100);
#endif
    QCOMPARE(QProcess::execute(m_paster, stringArgument), 0);
#endif
}

void tst_QClipboard::setMimeData()
{
    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup", SkipAll);
    QMimeData *mimeData = new QMimeData;
    const QString TestName(QLatin1String("tst_QClipboard::setMimeData() mimeData"));
    mimeData->setObjectName(TestName);
#if defined(Q_OS_WINCE)
    // need to set text on CE
    mimeData->setText(QLatin1String("Qt/CE foo"));
#endif

    QApplication::clipboard()->setMimeData(mimeData);
    QCOMPARE(QApplication::clipboard()->mimeData(), (const QMimeData *)mimeData);
    QCOMPARE(QApplication::clipboard()->mimeData()->objectName(), TestName);

    // set it to the same data again, it shouldn't delete mimeData (and crash as a result)
    QApplication::clipboard()->setMimeData(mimeData);
    QCOMPARE(QApplication::clipboard()->mimeData(), (const QMimeData *)mimeData);
    QCOMPARE(QApplication::clipboard()->mimeData()->objectName(), TestName);
    QApplication::clipboard()->clear();
    const QMimeData *appMimeData = QApplication::clipboard()->mimeData();
    QVERIFY(appMimeData != mimeData || appMimeData->objectName() != TestName);

    // check for crash when using the same mimedata object on several clipboards
    QMimeData *data = new QMimeData;
    data->setText("foo");

    QApplication::clipboard()->setMimeData(data, QClipboard::Clipboard);
    QApplication::clipboard()->setMimeData(data, QClipboard::Selection);
    QApplication::clipboard()->setMimeData(data, QClipboard::FindBuffer);

    QSignalSpy spySelection(QApplication::clipboard(), SIGNAL(selectionChanged()));
    QSignalSpy spyData(QApplication::clipboard(), SIGNAL(dataChanged()));
    QSignalSpy spyFindBuffer(QApplication::clipboard(), SIGNAL(findBufferChanged()));

    QApplication::clipboard()->clear(QClipboard::Clipboard);
    QApplication::clipboard()->clear(QClipboard::Selection); // used to crash on X11
    QApplication::clipboard()->clear(QClipboard::FindBuffer);

#if defined(Q_WS_X11)
    QCOMPARE(spySelection.count(), 1);
    QCOMPARE(spyData.count(), 1);
    QCOMPARE(spyFindBuffer.count(), 0);
#elif defined(Q_WS_MAC)
    QCOMPARE(spySelection.count(), 0);
    QCOMPARE(spyData.count(), 1);
    QCOMPARE(spyFindBuffer.count(), 1);
#elif defined(Q_WS_WIN)
    QCOMPARE(spySelection.count(), 0);
    QCOMPARE(spyData.count(), 1);
    QCOMPARE(spyFindBuffer.count(), 0);
#endif

    // an other crash test
    data = new QMimeData;
    data->setText("foo");

    QApplication::clipboard()->setMimeData(data, QClipboard::Clipboard);
    QApplication::clipboard()->setMimeData(data, QClipboard::Selection);
    QApplication::clipboard()->setMimeData(data, QClipboard::FindBuffer);

    QMimeData *newData = new QMimeData;
    newData->setText("bar");

    spySelection.clear();
    spyData.clear();
    spyFindBuffer.clear();

    QApplication::clipboard()->setMimeData(newData, QClipboard::Clipboard);
    QApplication::clipboard()->setMimeData(newData, QClipboard::Selection); // used to crash on X11
    QApplication::clipboard()->setMimeData(newData, QClipboard::FindBuffer);

#if defined(Q_WS_X11)
    QCOMPARE(spySelection.count(), 1);
    QCOMPARE(spyData.count(), 1);
    QCOMPARE(spyFindBuffer.count(), 0);
#elif defined(Q_WS_MAC)
    QCOMPARE(spySelection.count(), 0);
    QCOMPARE(spyData.count(), 1);
    QCOMPARE(spyFindBuffer.count(), 1);
#elif defined(Q_WS_WIN)
    QCOMPARE(spySelection.count(), 0);
    QCOMPARE(spyData.count(), 1);
    QCOMPARE(spyFindBuffer.count(), 0);
#endif
}

void tst_QClipboard::clearBeforeSetText()
{
    QApplication::processEvents();

    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup", SkipAll);

    const QString text = "tst_QClipboard::clearBeforeSetText()";

    // setText() should work after processEvents()
    QApplication::clipboard()->setText(text);
    QCOMPARE(QApplication::clipboard()->text(), text);
    QApplication::processEvents();
    QCOMPARE(QApplication::clipboard()->text(), text);

    // same with clear()
    QApplication::clipboard()->clear();
    QVERIFY(QApplication::clipboard()->text().isEmpty());
    QApplication::processEvents();
    QVERIFY(QApplication::clipboard()->text().isEmpty());

    // setText() again
    QApplication::clipboard()->setText(text);
    QCOMPARE(QApplication::clipboard()->text(), text);
    QApplication::processEvents();
    QCOMPARE(QApplication::clipboard()->text(), text);

    // clear() immediately followed by setText() should still return the text
    QApplication::clipboard()->clear();
    QVERIFY(QApplication::clipboard()->text().isEmpty());
    QApplication::clipboard()->setText(text);
    QCOMPARE(QApplication::clipboard()->text(), text);
    QApplication::processEvents();
    QCOMPARE(QApplication::clipboard()->text(), text);
}

#ifdef Q_OS_SYMBIAN
/*
    Test that text copied from qt application
    can be pasted with symbian clipboard
*/
// ### This test case only makes sense in symbian
void tst_QClipboard::pasteCopySymbian()
{
    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup", SkipAll);
    const QString string("Test string qt->symbian.");
    QApplication::clipboard()->setText(string);

    const TInt KPlainTextBegin = 0;
    RFs fs = qt_s60GetRFs();
    CClipboard* cb = CClipboard::NewForReadingLC(fs);

    CPlainText* text = CPlainText::NewL();
    CleanupStack::PushL(text);
    TInt dataLength = text->PasteFromStoreL(cb->Store(), cb->StreamDictionary(),
                                            KPlainTextBegin);
    if (dataLength == 0) {
        User::Leave(KErrNotFound);
    }
    HBufC* hBuf = HBufC::NewL(dataLength);
    TPtr buf = hBuf->Des();
    text->Extract(buf, KPlainTextBegin, dataLength);

    QString storeString = qt_TDesC2QString(buf);
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(cb);

    QCOMPARE(string, storeString);
}

static void nativeCopyHelper(const QString &string)
{
    const TInt KPlainTextBegin = 0;

    RFs fs = qt_s60GetRFs();
    CClipboard* cb = CClipboard::NewForWritingLC(fs);
    CStreamStore& store = cb->Store();
    CStreamDictionary& dict = cb->StreamDictionary();
    RStoreWriteStream symbianStream;
    TStreamId symbianStId = symbianStream.CreateLC(cb->Store());

    CPlainText* text = CPlainText::NewL();
    CleanupStack::PushL(text);
    TPtrC textPtr(qt_QString2TPtrC(string));
    text->InsertL(KPlainTextBegin, textPtr);
    text->CopyToStoreL(store, dict, KPlainTextBegin, textPtr.Length());
    CleanupStack::PopAndDestroy(text);
    (cb->StreamDictionary()).AssignL(KClipboardUidTypePlainText, symbianStId);
    cb->CommitL();
    CleanupStack::PopAndDestroy(2, cb);
}

/*
    Test that text copied to symbian clipboard
    can be pasted to qt clipboard
*/
// ### This test case only makes sense in symbian
void tst_QClipboard::copyPasteSymbian()
{
    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup", SkipAll);
    const QString string("Test string symbian->qt.");

    nativeCopyHelper(string);

    QCOMPARE(QApplication::clipboard()->text(), string);
}

/*
    Test that text copied to symbian clipboard
    can be pasted to qt clipboard, even if Qt
    clipboard already had copied formatted text
*/
// ### This test case only makes sense in symbian
void tst_QClipboard::copyCopyPasteSymbian()
{
    if (!nativeClipboardWorking())
        QSKIP("Native clipboard not working in this setup", SkipAll);

    //first copy some mime data with text/html and text/plain representations
    QMimeData *mimeData = new QMimeData;
    const QString preCopy(QLatin1String("qt_symbian"));
    mimeData->setText(preCopy);
    mimeData->setHtml(preCopy);
    QApplication::clipboard()->setMimeData(mimeData);

    //check both representations are pastable
    QCOMPARE(QApplication::clipboard()->mimeData()->html(), preCopy);
    QCOMPARE(QApplication::clipboard()->mimeData()->text(), preCopy);

    //native copy some plain text
    const QString string("symbian_qt");
    nativeCopyHelper(string);

    //check text/plain is pastable
    QCOMPARE(QApplication::clipboard()->text(), string);
    QCOMPARE(QApplication::clipboard()->mimeData()->text(), string);
    //check text/html is cleared
    QVERIFY(QApplication::clipboard()->mimeData()->html().isEmpty());
}
#endif

QTEST_MAIN(tst_QClipboard)

#include "tst_qclipboard.moc"
