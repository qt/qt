# Use subdirs template to suppress generation of unnecessary files
TEMPLATE = subdirs

# We just want to extract the zip file containing sqlite binaries for Symbian
BLD_INF_RULES.prj_exports +=  ":zip SQLite3_v9.2.zip"
