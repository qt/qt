symbian {
    RSS_RULES ="group_name=\"QtExamples\";"

    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia, Qt\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia, Qt\"" \
        " "
    default_deployment.pkg_prerules += vendorinfo
}
