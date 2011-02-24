#ifndef QXLIBDISPLAY_H
#define QXLIBDISPLAY_H

#include "qxlibintegration.h"

class QXlibDisplay
{
public:
    QXlibDisplay(Display *display);
    ~QXlibDisplay();
    QString displayName() const;

    Display *nativeDisplay() const;

    void sync() const;
    void flush() const;
private:
    Display *mDisplay;
    QString mDisplayName;
};

#endif // QXLIBDISPLAY_H
