//! [0]
MyGraphicsOpacityEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    // Fully opaque; draw directly without going through a pixmap.
    if (qFuzzyCompare(m_opacity, 1)) {
        source->draw(painter);
        return;
    }
    ...
}
//! [0]

//! [1]
MyGraphicsEffect::draw(QPainter *painter, QGraphicsEffectSource *source)
{
    ...
    QPoint offset;
    if (source->isPixmap()) {
        // No point in drawing in device coordinates (pixmap will be scaled anyways).
        const QPixmap pixmap = source->pixmap(Qt::LogicalCoordinates, &offset);
        ...
        painter->drawPixmap(offset, pixmap);
    } else {
        // Draw pixmap in device coordinates to avoid pixmap scaling;
        const QPixmap pixmap = source->pixmap(Qt::DeviceCoordinates, &offset);
        painter->setWorldTransform(QTransform());
        ...
        painter->drawPixmap(offset, pixmap);
    }
    ...
}
//! [1]

//! [2]
...
QLinearGradient alphaGradient(rect.topLeft(), rect.bottomLeft());
alphaGradient.setColorAt(0.0, Qt::transparent);
alphaGradient.setColorAt(0.5, Qt::black);
alphaGradient.setColorAt(1.0, Qt::transparent);
QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect;
effect->setOpacityMask(alphaGradient);
...
//! [2]
