TEMPLATE = subdirs
SUBDIRS = containers-associative \
	containers-sequential \
	qbytearray \
	qpainter \
	qtestlib-simple	events \
	qiodevice \
	qpixmap \
	blendbench \
	qstringlist \
	qobject \
        qrect \
	qregexp	\
        qregion \
        qvariant \
	qwidget	\
	qtwidgets

contains(QT_CONFIG, opengl): SUBDIRS += opengl
