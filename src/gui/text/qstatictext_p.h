#ifndef QSTATICTEXT_P_H
#define QSTATICTEXT_P_H

QT_BEGIN_NAMESPACE

#include <private/qtextengine_p.h>

class QStaticText;
class QStaticTextPrivate
{
public:
    QStaticTextPrivate();
    void init(const QString &text, const QFont &font);

    QTextLayout *textLayout;
    
    static QStaticTextPrivate *get(const QStaticText *q);
};

QT_END_NAMESPACE

#endif // QSTATICTEXT_P_H
