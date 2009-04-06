#ifndef QTESTELEMENT_H
#define QTESTELEMENT_H

#include "qtestcoreelement.h"

QT_BEGIN_NAMESPACE

class QTestElement: public QTestCoreElement<QTestElement>
{
    public:
        QTestElement(int type = -1);
        ~QTestElement();

        bool addLogElement(QTestElement *element);
        QTestElement *childElements() const;

        const QTestElement *parentElement() const;
        void setParent(const QTestElement *p);

    private:
        QTestElement *listOfChildren;
        const QTestElement * parent;

};

QT_END_NAMESPACE

#endif
