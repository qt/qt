TEMPLATE = subdirs
SUBDIRS = \
        corelib \
        gui \
        network \
        script \
        svg
contains(QT_CONFIG, opengl): SUBDIRS += opengl
contains(QT_CONFIG, declarative): SUBDIRS += declarative
