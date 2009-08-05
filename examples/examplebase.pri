symbian {
    RSS_RULES ="group_name=\"QtExamples\";"

    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia, Qt Software\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia, Qt Software\"" \
        " "
    default_deployment.pkg_prerules += vendorinfo
}
