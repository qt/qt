#ifndef QDECLARATIVETEXTLAYOUT_P_H
#define QDECLARATIVETEXTLAYOUT_P_H

#include <QtGui/qtextlayout.h>

class QDeclarativeTextLayoutPrivate;
class QDeclarativeTextLayout : public QTextLayout
{
public:
    QDeclarativeTextLayout();
    QDeclarativeTextLayout(const QString &);
    ~QDeclarativeTextLayout();

    void beginLayout();

    void prepare();
    void draw(QPainter *, const QPointF & = QPointF());

private:
    QDeclarativeTextLayoutPrivate *d;
};

#endif // QDECLARATIVETEXTLAYOUT_P_H
