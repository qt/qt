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

#ifndef ABSTRACTTESTSUITE_H
#define ABSTRACTTESTSUITE_H

#include <QtCore/qobject.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qdir.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvector.h>
#include <QtCore/qtextstream.h>

class TestMetaObjectBuilder;

namespace TestConfig {
enum Mode {
    Skip,
    ExpectFail
};
}

// For receiving callbacks from the config parser.
class TestConfigClientInterface
{
public:
    virtual ~TestConfigClientInterface() {}
    virtual void configData(TestConfig::Mode mode,
                            const QStringList &parts) = 0;
    virtual void configError(const QString &path,
                             const QString &message,
                             int lineNumber) = 0;
};

class AbstractTestSuite : public QObject,
                          public TestConfigClientInterface
{
// No Q_OBJECT macro, we implement the meta-object ourselves.
public:
    AbstractTestSuite(const QByteArray &className,
                      const QString &defaultTestsPath,
                      const QString &defaultConfigPath);
    virtual ~AbstractTestSuite();

    static QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **argv);

    static QString readFile(const QString &);
    static QString escape(const QString &);

protected:
    enum DataFunctionCreation {
        DontCreateDataFunction,
        CreateDataFunction
    };

    void addTestFunction(const QString &,
                         DataFunctionCreation = DontCreateDataFunction);
    void finalizeMetaObject();

    virtual void initTestCase();
    virtual void cleanupTestCase();

    virtual void writeSkipConfigFile(QTextStream &) = 0;
    virtual void writeExpectFailConfigFile(QTextStream &) = 0;

    virtual void runTestFunction(int index) = 0;

    virtual void configError(const QString &path, const QString &message, int lineNumber);

    QDir testsDir;
    bool shouldGenerateExpectedFailures;

private:
    TestMetaObjectBuilder *metaBuilder;
    QString skipConfigPath, expectFailConfigPath;

private:
    void createSkipConfigFile();
    void createExpectFailConfigFile();
};

#endif
