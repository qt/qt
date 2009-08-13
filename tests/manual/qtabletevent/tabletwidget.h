#ifndef TABLETWIDGET_H
#define TABLETWIDGET_H

#include <QWidget>
#include <QTabletEvent>

// a widget showing the information of the last tablet event
class TabletWidget : public QWidget
{
public:
    TabletWidget();
protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    void tabletEvent(QTabletEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    void resetAttributes() {
        mType = mDev = mPointerType = mXT = mYT = mZ = 0;
        mPress = mTangential = mRot = 0.0;
        mPos = mGPos = QPoint();
        mHiResGlobalPos = QPointF();
        mUnique = 0;
    }
    int mType;
    QPoint mPos, mGPos;
    QPointF mHiResGlobalPos;
    int mDev, mPointerType, mXT, mYT, mZ;
    qreal mPress, mTangential, mRot;
    qint64 mUnique;
};

#endif // TABLETWIDGET_H
