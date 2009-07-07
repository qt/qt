#ifndef PHONON_DUMMY_VIDEOWIDGET_H
#define PHONON_DUMMY_VIDEOWIDGET_H

#include <phonon/videowidget.h>
#include <phonon/videowidgetinterface.h>

#include "backend.h"

QT_BEGIN_NAMESPACE

class QString;

namespace Phonon
{
namespace Dummy
{

class VideoWidget : public QWidget, public Phonon::VideoWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(Phonon::VideoWidgetInterface)
public:
    VideoWidget(Backend *backend, QWidget *parent = 0);
    ~VideoWidget();

    void paintEvent(QPaintEvent *event);
    void setVisible(bool);

    Phonon::VideoWidget::AspectRatio aspectRatio() const;
    void setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio);
    Phonon::VideoWidget::ScaleMode scaleMode() const;
    void setScaleMode(Phonon::VideoWidget::ScaleMode);
    qreal brightness() const;
    void setBrightness(qreal);
    qreal contrast() const;
    void setContrast(qreal);
    qreal hue() const;
    void setHue(qreal);
    qreal saturation() const;
    void setSaturation(qreal);
    void setMovieSize(const QSize &size);
    QSize sizeHint() const;
    QRect scaleToAspect(QRect srcRect, int w, int h) const;
    QRect calculateDrawFrameRect() const;

    QSize movieSize() const {
        return m_movieSize;
    }

    bool event(QEvent *);

    QWidget *widget() {
        return this;
    }

protected:
    QSize m_movieSize;

private:
    Phonon::VideoWidget::AspectRatio m_aspectRatio;
    qreal m_brightness, m_hue, m_contrast, m_saturation;
    Phonon::VideoWidget::ScaleMode m_scaleMode;
};

}
} //namespace Phonon::Dummy

QT_END_NAMESPACE

#endif // PHONON_DUMMY_VIDEOWIDGET_H
