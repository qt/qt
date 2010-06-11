#include "qplatformwindow_lite.h"

#include <QtGui/qwidget.h>

class QPlatformWindowPrivate
{
    QWidget *tlw;
    QRect rect;
    friend class QPlatformWindow;
};

QPlatformWindow::QPlatformWindow(QWidget *tlw)
    : d_ptr(new QPlatformWindowPrivate)
{
    Q_D(QPlatformWindow);
    d->tlw = tlw;
    tlw->setPlatformWindow(this);
}

QPlatformWindow::~QPlatformWindow()
{
}

QWidget *QPlatformWindow::widget() const
{
    Q_D(const QPlatformWindow);
    return d->tlw;
}

void QPlatformWindow::setGeometry(const QRect &rect)
{
    Q_D(QPlatformWindow);
    d->rect = rect;
}

QRect QPlatformWindow::geometry() const
{
    Q_D(const QPlatformWindow);
    return d->rect;
}

/*!
Reimplemented in subclasses to show the surface if \a visible is \c true, and hide it if \a visible is \c false.
*/
void QPlatformWindow::setVisible(bool visible)
{
    Q_UNUSED(visible);
}
/*!
Requests setting the window flags of this surface to \a type. Returns the actual flags set.
*/
Qt::WindowFlags QPlatformWindow::setWindowFlags(Qt::WindowFlags flags)
{
    Q_UNUSED(flags);
    return Qt::Window;
}

/*!
  Returns the effective window flags for this surface.
*/
Qt::WindowFlags QPlatformWindow::windowFlags() const
{
    return Qt::Window;
}

WId QPlatformWindow::winId() const { return WId(0); }

void QPlatformWindow::setWindowTitle(const QString &) {}

void QPlatformWindow::raise() { qWarning("This plugin does not support raise()"); }

void QPlatformWindow::lower() { qWarning("This plugin does not support lower()"); }

void QPlatformWindow::setOpacity(qreal level)
{
    Q_UNUSED(level);
    qWarning("This plugin does not support setting window opacity");
}

QPlatformGLContext *QPlatformWindow::glContext()
{
    return 0;
}
