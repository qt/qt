TEMPLATE = subdirs
SUBDIRS += trace
!wince*{
    contains(QT_CONFIG, opengl):SUBDIRS += opengl
    contains(QT_CONFIG, openvg):contains(QT_CONFIG, egl):SUBDIRS += openvg
}else {
    # For WindowsCE only 2.x of OpenGL ES is supported by these plugins at this time
    contains(QT_CONFIG, opengl):!contains(QT_CONFIG, opengles1):!contains(QT_CONFIG, opengles1cl):SUBDIRS += opengl
    contains(QT_CONFIG, openvg):!contains(QT_CONFIG, opengles1):!contains(QT_CONFIG, opengles1cl):contains(QT_CONFIG, egl):SUBDIRS += openvg
}

contains(QT_CONFIG, shivavg) {
    # Only works under X11 at present
    !win32:!embedded:!mac:SUBDIRS += shivavg
}
