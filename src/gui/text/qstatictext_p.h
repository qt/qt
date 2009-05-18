#ifndef QSTATICTEXT_P_H
#define QSTATICTEXT_P_H

QT_BEGIN_NAMESPACE

#include <private/qtextengine_p.h>

class QStaticText;
class QStaticTextPrivate
{
public:
    void init(const QString &text, const QFont &font);

    QTextEngine engine;
    QVarLengthArray<int> visualOrder;
    
    static QStaticTextPrivate *get(const QStaticText *q);
};

QT_END_NAMESPACE

#endif // QSTATICTEXT_P_H
