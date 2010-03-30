TEMPLATE = subdirs

# These examples contain some C++ and need to be built
SUBDIRS = \
   extending \
   imageprovider \
   objectlistmodel \
   plugins \
   widgets

# These examples contain no C++ and can simply be copied
sources.files = \
   animations \
   aspectratio \
   behaviours \
   border-image \
   clocks \
   connections \
   dial \
   dynamic \
   effects \
   fillmode \
   focus \
   focusscope \
   fonts \
   gridview \
   layouts \
   listview \
   mousearea \
   package \
   parallax \
   progressbar \
   scrollbar \
   searchbox \
   slideswitch \
   sql \
   states \
   tabwidget \
   tic-tac-toe \
   tutorials \
   tvtennis \
   velocity \
   webview \
   workerlistmodel \
   workerscript \
   xmldata \
   xmlhttprequest
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative
INSTALLS += sources
