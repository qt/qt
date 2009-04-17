#include "PropertyAddState.h"
#include "timeperiod.h"

#include <QDateTime>
#include <QDebug>

PropertyAddState::PropertyAddState(QState *parent)
    : QState(parent)
{
}

void PropertyAddState::addToProperty(QObject *object, const char *propertyName, 
                                      const QVariant &valueToAdd)
{
    m_propertyAdditions.append(PropertyAdder(object, propertyName, valueToAdd));
}

QVariant PropertyAddState::addProperties(const QVariant &current, const QVariant &toAdd) const
{
    QVariant result;
    switch (current.type()) {
    case QVariant::DateTime:
        result = current.toDateTime() + qvariant_cast<TimePeriod>(toAdd);
        break;                                    
    case QVariant::Time:
        result = current.toTime() + qvariant_cast<TimePeriod>(toAdd);
        break;
    default:
        qWarning("PropertyAddState::addProperties: QVariant type '%s' not supported", 
                 current.typeName());
    };

    return result;
}

void PropertyAddState::onEntry()
{
    foreach (PropertyAdder propertyAdder, m_propertyAdditions) {
        QObject *object = propertyAdder.object;
        QByteArray propertyName = propertyAdder.propertyName;
        QVariant toAdd = propertyAdder.valueToAdd;
        QVariant current = object->property(propertyName);

        object->setProperty(propertyName, addProperties(current, toAdd));
    }
}
