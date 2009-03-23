TEMPLATE      = subdirs
SUBDIRS       = analogclock \
                calculator \
                calendarwidget \
                charactermap \
                codeeditor \
                digitalclock \
                groupbox \
                icons \
                imageviewer \
                lineedits \
                movie \
                scribble \
                shapedclock \
                sliders \
                spinboxes \
                stylesheet \
                tablet \
                tetrix \
                tooltips \
                validators \
                wiggly \
                windowflags

contains(styles, motif): SUBDIRS += styles

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS widgets.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets
INSTALLS += target sources
