!symbian: error(Only include this file for Symbian platforms)

RSS_RULES ="group_name=\"QtDemos\";"

vendorinfo = \
    "; Localised Vendor name" \
    "%{\"Nokia, Qt\"}" \
    " " \
    "; Unique Vendor name" \
    ":\"Nokia, Qt\"" \
    " "

demos_deployment.pkg_prerules += vendorinfo
DEPLOYMENT += demos_deployment

isEmpty(ICON):ICON = $$QT_SOURCE_TREE/src/s60installs/qt.svg
