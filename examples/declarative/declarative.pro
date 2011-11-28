TEMPLATE = subdirs

# These examples contain some C++ and need to be built
SUBDIRS = \
   animation \
   cppextensions \
   modelviews \
   i18n \
   imageelements \
   keyinteraction/focus \
   positioners \
   sqllocalstorage \
   text \
   threading \
   tutorials \
   touchinteraction \
   toys \
   ui-components

# OpenGL shader examples requires opengl and they contain some C++ and need to be built
contains(QT_CONFIG, opengl): SUBDIRS += shadereffects

# plugins uses a 'Time' class that conflicts with symbian e32std.h also defining a class of the same name
symbian:SUBDIRS -= plugins

# These examples contain no C++ and can simply be copied
sources.files = \
   cppextensions \
   xml

sources.path = $$[QT_INSTALL_EXAMPLES]/declarative
INSTALLS += sources
