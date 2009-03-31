#include <QtGui/qgraphicslayout.h>

class FlowLayout : public QGraphicsLayout
{
public:
    FlowLayout();
    inline void addItem(QGraphicsLayoutItem *item);
    void insertItem(int index, QGraphicsLayoutItem *item);
    void setSpacing(Qt::Orientations o, qreal spacing);
    qreal spacing(Qt::Orientation o) const;

    // inherited functions
    void setGeometry(const QRectF &geom);

    int count() const;
    QGraphicsLayoutItem *itemAt(int index) const;
    void removeAt(int index);

protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

private:
    qreal doLayout(const QRectF &geom, bool applyNewGeometry);
    QSizeF minSize(const QSizeF &constraint) const;
    QSizeF prefSize() const;
    QSizeF maxSize() const;

    QList<QGraphicsLayoutItem*> m_items;
    qreal m_spacing[2];
};


inline void FlowLayout::addItem(QGraphicsLayoutItem *item)
{
    insertItem(-1, item);
}
