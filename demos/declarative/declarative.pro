TEMPLATE = subdirs

# These demos contain C++ and need to be compiled
SUBDIRS = \
    minehunt

# These examples contain no C++ and can simply be copied
sources.files = \
   calculator \
   flickr \
   photoviewer \
   samegame \
   snake \
   twitter \
   rssnews \
   webbrowser

sources.path = $$[QT_INSTALL_DEMOS]/declarative
INSTALLS += sources

