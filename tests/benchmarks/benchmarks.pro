TEMPLATE = subdirs
SUBDIRS = containers-associative \
	containers-sequential \
        qanimation \
	qbytearray \
	qpainter \
	qtestlib-simple	events \
	qiodevice \
	qpixmap \
	blendbench \
    qstring \
    qstringlist \
        qmatrix4x4 \
        qnetworkreply \
	qobject \
        qrect \
	qregexp	\
        qregion \
        qvariant \
	qwidget	\
	qtwidgets \
        qmlxmlparser \
        qfxview \

contains(QT_CONFIG, opengl): SUBDIRS += opengl

# Tests which should run in Pulse
PULSE_TESTS = \
    qfxview \
    qmlxmlparser \

