TEMPLATE = app
LANGUAGE = C++

FORMS += project.ui

TRANSLATIONS        = project.ts

# Copy the ts to a temp file because:
# 1. The depot file is usually read-only
# 2. We don't want to modify the original file, since then it won't be possible to run the test twice
#    without reverting the original file again.

win32: system(copy /Y project.ts.before $$TRANSLATIONS)
unix: system(cp -f project.ts.before $$TRANSLATIONS && chmod a+w $$TRANSLATIONS)
