
#include <QString>

struct String : QString
{
    String() {}
    String(const QString &s) : QString(s) {}
};

QT_BEGIN_NAMESPACE
uint qHash(const String &);
QT_END_NAMESPACE
