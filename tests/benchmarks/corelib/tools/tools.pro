TEMPLATE = subdirs
SUBDIRS = \
        containers-associative \
        containers-sequential \
        qbytearray \
        qcontiguouscache \
        qline \
        qlist \
        qrect \
        #qregexp \     # FIXME: broken
        qstring \
        qstringbuilder \
        qstringlist \
        qvector

!*g++*: SUBDIRS -= qstring
