#ifndef QTESTXMLSTREAMER_H
#define QTESXMLSTREAMER_H

#include "qtestbasicstreamer.h"

QT_BEGIN_NAMESPACE

class QTestElement;
class QTestElementAttribute;

class QTestXmlStreamer: public QTestBasicStreamer
{
    public:
        QTestXmlStreamer();
        ~QTestXmlStreamer();

        void formatStart(const QTestElement *element = 0, char *formatted = 0) const;
        void formatEnd(const QTestElement *element = 0, char *formatted = 0) const;
        void formatBeforeAttributes(const QTestElement *element = 0, char *formatted = 0) const;
        void output(QTestElement *element) const;
};

QT_END_NAMESPACE

#endif
