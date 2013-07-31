TEMPLATE=subdirs
SUBDIRS=\
           qdbusabstractadaptor \
           qdbusabstractinterface \
           qdbusconnection \
           qdbuscontext \
           qdbusinterface \
           qdbuslocalcalls \
           qdbusmarshall \
           qdbusmetaobject \
           qdbusmetatype \
           qdbuspendingcall \
           qdbuspendingreply \
           qdbusreply \
           qdbusservicewatcher \
           qdbustype \
           qdbusthreading \
           qdbusxmlparser \
           qdbusxmlgenerator \

!contains(QT_CONFIG, private_tests): SUBDIRS -= \
           qdbusmarshall \
