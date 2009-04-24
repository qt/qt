<Rect id="removeButton"
    width="30" height="30"
    color="red"
    radius="5">
    <properties>
        <Property name="expandedWidth" value="230"/>
    </properties>
    <signals>
        <Signal name="confirmed"/>
    </signals>
    <resources>
        <Script>
            function toggle() {
                print('removeButton.toggle()');
                if (removeButton.state == 'opened') {
                    removeButton.state = '';
                    contacts.mouseGrabbed=false;
                } else {
                    if (!contacts.mouseGrabbed) {
                        removeButton.state = 'opened';
                        contacts.mouseGrabbed=true;
                    }
                }
            }
        </Script>
    </resources>
    <Image id="trashIcon"
        width="22" height="22"
        anchors.right="{parent.right}" anchors.rightMargin="4"
        anchors.verticalCenter="{parent.verticalCenter}"
        src="../shared/pics/trash.png"
        opacity="1">
        <MouseRegion
            anchors.fill="{parent}"
            onClicked="toggle()"/>
    </Image>
    <Image id="cancelIcon"
        width="22" height="22"
        anchors.right="{parent.right}" anchors.rightMargin="4"
        anchors.verticalCenter="{parent.verticalCenter}"
        src="../shared/pics/cancel.png"
        opacity="0">
        <MouseRegion
            anchors.fill="{parent}"
            onClicked="toggle()"/>
    </Image>
    <Image id="confirmIcon"
        width="22" height="22"
        anchors.left="{parent.left}" anchors.leftMargin="4"
        anchors.verticalCenter="{parent.verticalCenter}"
        src="../shared/pics/ok.png"
        opacity="0">
        <MouseRegion
            anchors.fill="{parent}"
            onClicked="toggle(); removeButton.confirmed.emit()"/>
    </Image>
    <Text id="text"
        anchors.verticalCenter="{parent.verticalCenter}"
        anchors.left="{confirmIcon.right}" anchors.leftMargin="4"
        anchors.right="{cancelIcon.left}" anchors.rightMargin="4"
        font.bold="true"
        color="white"
        hAlign="AlignHCenter"
        text="Remove"
        opacity="0"/>
    <states>
        <State name="opened">
            <SetProperty target="{removeButton}" property="width" value="{removeButton.expandedWidth}"/>
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
</Rect>
