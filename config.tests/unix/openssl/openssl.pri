!cross_compile {
    TRY_INCLUDEPATHS = /include /usr/include /usr/local/include $$QMAKE_INCDIR $$INCLUDEPATH
    # LSB doesn't allow using headers from /include or /usr/include
    linux-lsb-g++:TRY_INCLUDEPATHS = $$QMAKE_INCDIR $$INCLUDEPATH
    for(p, TRY_INCLUDEPATHS) {
        pp = $$join(p, "", "", "/openssl")
        exists($$pp):INCLUDEPATH *= $$p
    }
}

symbian{
    TRY_INCLUDEPATHS = $${EPOCROOT}epoc32 $${EPOCROOT}epoc32/include $${EPOCROOT}epoc32/include/stdapis $${EPOCROOT}epoc32/include/stdapis/sys $$OS_LAYER_LIBC_SYSTEMINCLUDE $$QMAKE_INCDIR $$INCLUDEPATH 
    for(p, TRY_INCLUDEPATHS) {
        pp = $$join(p, "", "", "/openssl")
        exists($$pp):INCLUDEPATH *= $$pp
    } 
}
