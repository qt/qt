TEMPLATE = subdirs

# These demos contain C++ and need to be compiled
SUBDIRS = \
   calculator \
   flickr \
   photoviewer \
   minehunt \
   samegame \
   snake \
   rssnews \
   twitter

contains(QT_CONFIG, webkit):SUBDIRS += webbrowser

