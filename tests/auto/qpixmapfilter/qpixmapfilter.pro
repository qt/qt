load(qttest_p4)
SOURCES  += tst_qpixmapfilter.cpp

wince*: {
    addFiles.sources = noise.png
    addFiles.path = .
    DEPLOYMENT += addFiles
}

