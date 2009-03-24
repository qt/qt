#ifndef QTESTFILELOGGER_H
#define QTESTFILELOGGER_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QTestFileLogger
{
    public:
        QTestFileLogger();
        ~QTestFileLogger();

        void init();
        void flush(const char *msg);
};

QT_END_NAMESPACE

#endif // QTESTFILELOGGER_H
