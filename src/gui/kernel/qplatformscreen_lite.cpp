#include "qplatformscreen_lite.h"

QWidget *QPlatformScreen::topLevelAt(const QPoint & pos) const
{
    QWidgetList list = QApplication::topLevelWidgets();
    for (int i = list.size()-1; i >= 0; --i) {
        QWidget *w = list[i];
        //### mask is ignored
        if (w != QApplication::desktop() && w->isVisible() && w->geometry().contains(pos))
            return w;
    }

    return 0;
}

