#ifndef QTESTBASICSTREAMER_H
#define QTESTBASICSTREAMER_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QTestElement;
class QTestElementAttribute;
class QTestLogger;

class QTestBasicStreamer
{
    public:
        QTestBasicStreamer();
        virtual ~QTestBasicStreamer();

        virtual void output(QTestElement *element) const;

        void outputString(const char *msg) const;
        bool isTtyOutput();
        void startStreaming();
        void stopStreaming();

        void setLogger(const QTestLogger *tstLogger);
        const QTestLogger *logger() const;

    protected:
        virtual void formatStart(const QTestElement *element = 0, char *formatted = 0) const;
        virtual void formatEnd(const QTestElement *element = 0, char *formatted = 0) const;
        virtual void formatBeforeAttributes(const QTestElement *element = 0, char *formatted = 0) const;
        virtual void formatAfterAttributes(const QTestElement *element = 0, char *formatted = 0) const;
        virtual void formatAttributes(const QTestElement *element = 0, const QTestElementAttribute *attribute = 0, char *formatted = 0) const;
        virtual void outputElements(QTestElement *element, bool isChildElement = false) const;
        virtual void outputElementAttributes(const QTestElement *element, QTestElementAttribute *attribute) const;

    private:
        const QTestLogger *testLogger;
};

QT_END_NAMESPACE

#endif
