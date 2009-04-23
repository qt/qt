#ifndef TOUCHWIDGET_H
#define TOUCHWIDGET_H

#include <QWidget>

class TouchWidget : public QWidget
{
    Q_OBJECT

public:
    bool acceptTouchBegin, acceptTouchUpdate, acceptTouchEnd;
    bool seenTouchBegin, seenTouchUpdate, seenTouchEnd;
    bool acceptMousePress, acceptMouseMove, acceptMouseRelease;
    bool seenMousePress, seenMouseMove, seenMouseRelease;

    inline TouchWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
        reset();
    }

    void reset()
    {
        acceptTouchBegin
            = acceptTouchUpdate
            = acceptTouchEnd
            = seenTouchBegin
            = seenTouchUpdate
            = seenTouchEnd
            = acceptMousePress
            = acceptMouseMove
            = acceptMouseRelease
            = seenMousePress
            = seenMouseMove
            = seenMouseRelease
            = false;
    }

    bool event(QEvent *event);
};

#endif // TOUCHWIDGET_H
