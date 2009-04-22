#ifndef PropertyAddState_H
#define PropertyAddState_H

#include <qstate.h>

#include <QVariant>
#include <QList>

class PropertyAddState: public QState
{
public:
    PropertyAddState(QState *parent = 0);
    
    void addToProperty(QObject *object, const char *propertyName, const QVariant &valueToAdd);
    virtual void onEntry();

private:
    QVariant addProperties(const QVariant &current, const QVariant &toAdd) const;

    struct PropertyAdder {
        PropertyAdder(QObject *_object, const char *_propertyName, const QVariant &_valueToAdd)
            : object(_object), propertyName(_propertyName), valueToAdd(_valueToAdd)
        {
        }

        QObject *object;
        QByteArray propertyName;
        QVariant valueToAdd;
    };
    QList<PropertyAdder> m_propertyAdditions;
};

#endif // PropertyAddState_H
