#include "qtestfilelogger.h"
#include "qtestassert.h"
#include "QtTest/private/qtestlog_p.h"
#include "QtTest/private/qtestresult_p.h"

#include <stdlib.h>
#include <stdio.h>

namespace QTest
{
    static FILE *stream = 0;
}

QTestFileLogger::QTestFileLogger()
{
}

QTestFileLogger::~QTestFileLogger()
{
    if(QTest::stream)
        fclose(QTest::stream);

    QTest::stream = 0;
}

void QTestFileLogger::init()
{
    char filename[100];
    QTest::qt_snprintf(filename, sizeof(filename), "%s.log",
                QTestResult::currentTestObjectName());

    #if defined(_MSC_VER) && _MSC_VER >= 1400 && !defined(Q_OS_WINCE)
    if (::fopen_s(&QTest::stream, filename, "wt")) {
        #else
        QTest::stream = ::fopen(filename, "wt");
        if (!QTest::stream) {
            #endif
            printf("Unable to open file for simple logging: %s", filename);
            ::exit(1);
        }
}

void QTestFileLogger::flush(const char *msg)
{
    QTEST_ASSERT(QTest::stream);

    ::fputs(msg, QTest::stream);
    ::fflush(QTest::stream);
}
