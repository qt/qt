!x11:!embedded:!nacl:mac {
   LIBS_PRIVATE += -framework Carbon -lz
   *-mwerks:INCLUDEPATH += compat
}
