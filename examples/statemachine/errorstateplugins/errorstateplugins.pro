TEMPLATE      = subdirs
SUBDIRS       = random_ai \
                spin_ai_with_error \
                spin_ai

# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine/errorstateplugins
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS errorstateplugins.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine/errorstateplugins
INSTALLS += target sources
