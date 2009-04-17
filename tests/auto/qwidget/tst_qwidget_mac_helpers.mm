#include "tst_qwidget_mac_helpers.h"
#include <private/qt_mac_p.h>
#include <private/qt_cocoa_helpers_mac_p.h>


QString nativeWindowTitle(QWidget *window, Qt::WindowState state)
{
    OSWindowRef windowRef = qt_mac_window_for(window);
    QCFString macTitle;
    if (state == Qt::WindowMinimized) {
#ifndef QT_MAC_USE_COCOA
        CopyWindowAlternateTitle(windowRef, &macTitle);
#else
        macTitle = reinterpret_cast<CFStringRef>([[windowRef miniwindowTitle] retain]);
#endif
    } else {
#ifndef QT_MAC_USE_COCOA
        CopyWindowTitleAsCFString(windowRef, &macTitle);
#else
        macTitle = reinterpret_cast<CFStringRef>([[windowRef title] retain]);
#endif
    }
    return macTitle;
}

bool nativeWindowModified(QWidget *widget)
{
#ifndef QT_MAC_USE_COCOA
    return IsWindowModified(qt_mac_window_for(widget));
#else
    return [qt_mac_window_for(widget) isDocumentEdited];
#endif
}
