TEMPLATE         = \
                 subdirs
SUBDIRS          += \
                 animatedtiles \
                 appchooser \
                 easing \
                 example \
                 moveblocks \
                 padnavigator-ng \
                 photobrowser \
                 piemenu \
                 selectbutton \
                 states \
                 stickman \
                 sub-attaq

# install
target.path = $$[QT_INSTALL_EXAMPLES]/animation
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS animation.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/animation
INSTALLS += target sources
