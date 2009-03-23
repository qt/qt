#ifndef QTESTLOGGER_P_H
#define QTESTLOGGER_P_H

#include <QtTest/private/qabstracttestlogger_p.h>

class QTestBasicStreamer;
class QTestElement;
class QTestFileLogger;

QT_BEGIN_NAMESPACE

class QTestLogger : public QAbstractTestLogger
{
    public:
        QTestLogger(int fm = 0);
        ~QTestLogger();

        enum TestLoggerFormat
        {
            TLF_XML = 0,
            TLF_LightXml = 1,
            TLF_XunitXml = 2
        };

        void startLogging();
        void stopLogging();

        void enterTestFunction(const char *function);
        void leaveTestFunction();

        void addIncident(IncidentTypes type, const char *description,
                     const char *file = 0, int line = 0);
        void addBenchmarkResult(const QBenchmarkResult &result);

        void addMessage(MessageTypes type, const char *message,
                    const char *file = 0, int line = 0);

        void setLogFormat(TestLoggerFormat fm);
        TestLoggerFormat logFormat();

        int passCount() const;
        int failureCount() const;
        int errorCount() const;
        int warningCount() const;
        int skipCount() const;
        int systemCount() const;
        int qdebugCount() const;
        int qwarnCount() const;
        int qfatalCount() const;
        int infoCount() const;

    private:
        QTestElement *listOfTestcases;
        QTestElement *currentLogElement;
        QTestBasicStreamer *logFormatter;
        TestLoggerFormat format;
        QTestFileLogger *filelogger;

        int testCounter;
        int passCounter;
        int failureCounter;
        int errorCounter;
        int warningCounter;
        int skipCounter;
        int systemCounter;
        int qdebugCounter;
        int qwarnCounter;
        int qfatalCounter;
        int infoCounter;
};

QT_END_NAMESPACE

#endif // QTESTLOGGER_P_H
