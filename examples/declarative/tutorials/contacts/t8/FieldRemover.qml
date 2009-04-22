<Item height="30" width="30" id="fieldRemover" clip="true">
    <properties>
        <Property name="open" value="false"/>
    </properties>
    <signals>
        <Signal name="confirm"/>
    </signals>
    <resources>
        <Script>
            function toggle() {
                if (fieldRemover.state=='opened') {
                    fieldRemover.state='';
                    open='false';
                    Page.mouseGrabbed='false';
                } else {
                    if (Page.mouseGrabbed != 'true') {
                        fieldRemover.state='opened';
                        open='true';
                        Page.mouseGrabbed='true';
                    }
                }
            }
        </Script>
    </resources>
    <Rect id="border" anchors.fill="{parent}" color="red" radius="5"/>
    <Image id="trashIcon" src="../shared/pics/trash.png"
        width="22" height="22"
        anchors.right="{parent.right}" anchors.rightMargin="4"
        anchors.verticalCenter="{parent.verticalCenter}"/>
    <Image id="cancelIcon" src="../shared/pics/cancel.png"
        width="22" height="22" opacity="0"
        anchors.right="{parent.right}" anchors.rightMargin="4"
        anchors.verticalCenter="{parent.verticalCenter}"/>
    <Image id="confirmIcon" src="../shared/pics/ok.png"
        width="22" height="22" opacity="0"
        anchors.left="{parent.left}" anchors.leftMargin="4"
        anchors.verticalCenter="{parent.verticalCenter}"/>
    <Text id="text" opacity="0" text="Remove" font.bold="true" color="white" hAlign="AlignHCenter"
        anchors.verticalCenter="{parent.verticalCenter}"
        anchors.left="{confirmIcon.right}" anchors.leftMargin="4"
        anchors.right="{cancelIcon.left}" anchors.rightMargin="4"/>
    <MouseRegion anchors.fill="{confirmIcon}" onClicked="toggle(); fieldRemover.confirm.emit()"/>
    <MouseRegion anchors.fill="{trashIcon}" onClicked="toggle()"/>
    <states>
        <State name="opened">
            <SetProperty target="{text}" property="opacity" value="1"/>
            <SetProperty target="{confirmIcon}" property="opacity" value="1"/>
            <SetProperty target="{cancelIcon}" property="opacity" value="1"/>
            <SetProperty target="{trashIcon}" property="opacity" value="0"/>
        </State>
    </states>
    <transitions>
        <Transition fromState="*" toState="opened" reversible="true">
            <NumericAnimation properties="opacity,x,width" duration="200"/>
        </Transition>
    </transitions>
</Item>
