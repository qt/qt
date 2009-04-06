#ifndef QTESTLIGHTXMLSTREAMER_H
#define QTESTLIGHTXMLSTREAMER_H

#include "qtestbasicstreamer.h"

QT_BEGIN_NAMESPACE

class QTestElement;
class QTestElementAttribute;

class QTestLightXmlStreamer: public QTestBasicStreamer
{
    public:
        QTestLightXmlStreamer();
        ~QTestLightXmlStreamer();

        void formatStart(const QTestElement *element = 0, char *formatted = 0) const;
        void formatEnd(const QTestElement *element = 0, char *formatted = 0) const;
        void formatBeforeAttributes(const QTestElement *element = 0, char *formatted = 0) const;
        void output(QTestElement *element) const;
};

QT_END_NAMESPACE

#endif
