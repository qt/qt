#ifndef OLDNORMALIZEOBJECT_H
#define OLDNORMALIZEOBJECT_H

#include <QObject>

struct Struct;
class Class;
template <typename T> class Template;

// An object with old moc output that incorrectly normalizes 'T<C> const &' in the function
// signatures
class OldNormalizeObject : public QObject
{
    /* tmake ignore Q_OBJECT */
    Q_OBJECT

signals:
    void typeRefSignal(Template<Class &> &ref);
    void constTypeRefSignal(const Template<const Class &> &ref);
    void typeConstRefSignal(Template<Class const &> const &ref);

public slots:
    void typeRefSlot(Template<Class &> &) {}
    void constTypeRefSlot(const Template<const Class &> &) {}
    void typeConstRefSlot(Template<Class const &> const &) {}
};

#endif // OLDNORMALIZEOBJECT_H
