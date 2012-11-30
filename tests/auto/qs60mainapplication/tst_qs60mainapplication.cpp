/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** All rights reserved.
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
#include <QtGui/QS60MainApplication>
#include <QtGui/QS60MainDocument>
#include <QtGui/QS60MainAppUi>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QS60MainApplication : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void customQS60MainApplication();
};

void tst_QS60MainApplication::initTestCase()
{
}

void tst_QS60MainApplication::cleanupTestCase()
{
}

void tst_QS60MainApplication::init()
{
}

void tst_QS60MainApplication::cleanup()
{
}

#ifdef Q_WS_S60
bool appUiConstructed = false;

class CustomMainAppUi : public QS60MainAppUi
{
public:
    CustomMainAppUi()
    {
        appUiConstructed = true;
    }
};

class CustomMainDocument : public QS60MainDocument
{
public:
    CustomMainDocument(CEikApplication &eikApp)
        : QS60MainDocument(eikApp)
    {
    }
    CEikAppUi *CreateAppUiL()
    {
        return new (ELeave) CustomMainAppUi;
    }
};

class CustomMainApplication : public QS60MainApplication
{
protected:
    CApaDocument *CreateDocumentL()
    {
        return new (ELeave) CustomMainDocument(*this);
    }
};

CApaApplication *factory()
{
    return new (ELeave) CustomMainApplication;
}
#endif // Q_WS_S60

void tst_QS60MainApplication::customQS60MainApplication()
{
#ifndef Q_WS_S60
    QSKIP("This is an S60-only test", SkipAll);
#else
    int argc = 1;
    char *argv = "tst_qs60mainapplication";
    QApplication app(factory, argc, &argv);
    QVERIFY(appUiConstructed);
#endif
}

QTEST_APPLESS_MAIN(tst_QS60MainApplication)
#include "tst_qs60mainapplication.moc"
