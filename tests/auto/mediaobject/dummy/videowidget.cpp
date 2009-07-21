#include "videowidget.h"
#include <QtCore/QEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QPalette>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QBoxLayout>
#include <QApplication>
#include "mediaobject.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Dummy
{

VideoWidget::VideoWidget(Backend *backend, QWidget *parent) :
    QWidget(parent),
    m_aspectRatio(Phonon::VideoWidget::AspectRatioAuto),
    m_brightness(0.0),
    m_hue(0.0),
    m_contrast(0.0),
    m_saturation(0.0),
    m_scaleMode(Phonon::VideoWidget::FitInView)
{
    Q_UNUSED(backend)

}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
}

void VideoWidget::setVisible(bool val)
{
    Q_UNUSED(val)
}

Phonon::VideoWidget::AspectRatio VideoWidget::aspectRatio() const
{
    return m_aspectRatio;
}

QSize VideoWidget::sizeHint() const
{
    return QSize(640, 480);
}

void VideoWidget::setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio)
{
    Q_UNUSED(aspectRatio)
}

Phonon::VideoWidget::ScaleMode VideoWidget::scaleMode() const
{
    return m_scaleMode;
}

QRect VideoWidget::scaleToAspect(QRect srcRect, int w, int h) const
{
    float width = srcRect.width();
    float height = srcRect.width() * (float(h) / float(w));
    if (height > srcRect.height()) {
        height = srcRect.height();
        width = srcRect.height() * (float(w) / float(h));
    }
    return QRect(0, 0, (int)width, (int)height);
}

/***
 * Calculates the actual rectangle the movie will be presented with
 **/
QRect VideoWidget::calculateDrawFrameRect() const
{
    QRect widgetRect = rect();
    QRect drawFrameRect;
    // Set m_drawFrameRect to be the size of the smallest possible
    // rect conforming to the aspect and containing the whole frame:
    switch (aspectRatio()) {

    case Phonon::VideoWidget::AspectRatioWidget:
        drawFrameRect = widgetRect;
        // No more calculations needed.
        return drawFrameRect;

    case Phonon::VideoWidget::AspectRatio4_3:
        drawFrameRect = scaleToAspect(widgetRect, 4, 3);
        break;

    case Phonon::VideoWidget::AspectRatio16_9:
        drawFrameRect = scaleToAspect(widgetRect, 16, 9);
        break;

    case Phonon::VideoWidget::AspectRatioAuto:
    default:
        drawFrameRect = QRect(0, 0, movieSize().width(), movieSize().height());
        break;
    }

    // Scale m_drawFrameRect to fill the widget
    // without breaking aspect:
    float widgetWidth = widgetRect.width();
    float widgetHeight = widgetRect.height();
    float frameWidth = widgetWidth;
    float frameHeight = drawFrameRect.height() * float(widgetWidth) / float(drawFrameRect.width());

    switch (scaleMode()) {
    case Phonon::VideoWidget::ScaleAndCrop:
        if (frameHeight < widgetHeight) {
            frameWidth *= float(widgetHeight) / float(frameHeight);
            frameHeight = widgetHeight;
        }
        break;
    case Phonon::VideoWidget::FitInView:
    default:
        if (frameHeight > widgetHeight) {
            frameWidth *= float(widgetHeight) / float(frameHeight);
            frameHeight = widgetHeight;
        }
        break;
    }
    drawFrameRect.setSize(QSize(int(frameWidth), int(frameHeight)));
    drawFrameRect.moveTo(int((widgetWidth - frameWidth) / 2.0f),
                           int((widgetHeight - frameHeight) / 2.0f));
    return drawFrameRect;
}

void VideoWidget::setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode)
{
    Q_UNUSED(scaleMode)
}

qreal VideoWidget::brightness() const
{
    return m_brightness;
}

qreal clampedValue(qreal val)
{
    if (val > 1.0 )
        return 1.0;
    else if (val < -1.0)
        return -1.0;
    else return val;
}

void VideoWidget::setBrightness(qreal newValue)
{
    Q_UNUSED(newValue)
}

qreal VideoWidget::contrast() const
{
    return m_contrast;
}

void VideoWidget::setContrast(qreal newValue)
{
    Q_UNUSED(newValue)
}

qreal VideoWidget::hue() const
{
    return m_hue;
}

void VideoWidget::setHue(qreal newValue)
{
    Q_UNUSED(newValue)
}

qreal VideoWidget::saturation() const
{
    return m_saturation;
}

void VideoWidget::setSaturation(qreal newValue)
{
    Q_UNUSED(newValue)
}

bool VideoWidget::event(QEvent *event)
{
    return QWidget::event(event);
}

void VideoWidget::setMovieSize(const QSize &size)
{
    m_movieSize = size;
    widget()->updateGeometry();
    widget()->update();
}

}
} //namespace Phonon::Dummy

QT_END_NAMESPACE

#include "moc_videowidget.cpp"
