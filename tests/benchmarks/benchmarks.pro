TEMPLATE = subdirs
SUBDIRS = containers-associative \
	containers-sequential \
        qanimation \
	qbytearray \
        qfileinfo \
        qfile_vs_qnetworkaccessmanager \
        qhostinfo \
	qpainter \
	qtestlib-simple	events \
        qtext \
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
	qapplication \
	qdir \
	qdiriterator \
	qgraphicsanchorlayout \
	qgraphicsitem \
	qgraphicswidget \
	qmetaobject \
	qpixmapcache \
	qquaternion \
	qscriptclass \
	qscriptengine \
	qscriptvalue \
	qstringbuilder \
	qstylesheetstyle \
	qsvgrenderer \
        qtcpserver \
	qtableview \
	qthreadstorage



contains(QT_CONFIG, opengl): SUBDIRS += opengl
