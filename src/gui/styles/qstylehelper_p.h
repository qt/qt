#include <QtCore/qglobal.h>
#include <QtCore/qpoint.h>
#include <QtGui/qpolygon.h>

#ifndef QSTYLEHELPER_P_H
#define QSTYLEHELPER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

class QPainter;
class QStyleOptionSlider;
class QStyleOption;

namespace QStyleHelper
{
    extern const bool UsePixmapCache;
    QString uniqueName(const QString &key, const QStyleOption *option, const QSize &size);
#ifndef QT_NO_DIAL
    qreal angle(const QPointF &p1, const QPointF &p2);
    QPolygonF calcLines(const QStyleOptionSlider *dial);
    int calcBigLineSize(int radius);
    void drawDial(const QStyleOptionSlider *dial, QPainter *painter);
#endif //QT_NO_DIAL
    void drawBorderPixmap(const QPixmap &pixmap, QPainter *painter, const QRect &rect,
                     int left = 0, int top = 0, int right = 0,
                     int bottom = 0);
}

QT_END_NAMESPACE

#endif // QSTYLEHELPER_P_H
