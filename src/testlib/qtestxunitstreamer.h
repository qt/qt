#ifndef QTESTXUNITSTREAMER_H
#define QTESTXUNITSTREAMER_H

#include "qtestbasicstreamer.h"

QT_BEGIN_NAMESPACE

class QTestLogger;

class QTestXunitStreamer: public QTestBasicStreamer
{
    public:
        QTestXunitStreamer();
        ~QTestXunitStreamer();

        void formatStart(const QTestElement *element = 0, char *formatted = 0) const;
        void formatEnd(const QTestElement *element = 0, char *formatted = 0) const;
        void formatAfterAttributes(const QTestElement *element = 0, char *formatted = 0) const;
        void formatAttributes(const QTestElementAttribute *attribute = 0, char *formatted = 0) const;
        void output(QTestElement *element) const;
        void outputElements(QTestElement *element, bool isChildElement = false) const;

    private:
        void displayXunitXmlHeader() const;
        static void indentForElement(const QTestElement* element, char* buf, int size);
};

QT_END_NAMESPACE

#endif
