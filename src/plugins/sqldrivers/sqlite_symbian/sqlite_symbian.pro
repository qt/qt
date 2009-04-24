# Use subdirs template to suppress generation of unnecessary files
TEMPLATE = subdirs

# We just want to extract the zip file containing sqlite binaries for Symbian
!exists($${EPOCROOT}epoc32/release/armv5/urel/sqlite3.dll): {
    BLD_INF_RULES.prj_exports +=  ":zip SQLite3_v9.2.zip"
}

