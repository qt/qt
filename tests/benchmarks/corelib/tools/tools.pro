TEMPLATE = subdirs
SUBDIRS = \
        containers-associative \
        containers-sequential \
        qbytearray \
        qline \
        qlist \
        qrect \
        #qregexp \     # FIXME: broken
        qstring \
        qstringbuilder \
        qstringlist \
        qvector

!*g++*: SUBDIRS -= qstring
