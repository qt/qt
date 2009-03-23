#ifndef QTESTFILELOGGER_H
#define QTESTFILELOGGER_H

class QTestFileLogger
{
    public:
        QTestFileLogger();
        ~QTestFileLogger();

        void init();
        void flush(const char *msg);
};

#endif // QTESTFILELOGGER_H
