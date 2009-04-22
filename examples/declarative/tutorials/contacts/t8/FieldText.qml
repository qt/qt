<Item height="30" id="fieldText">
    <properties>
        <Property name="open" value="false"/>
        <Property name="text" value="" onValueChanged="setText(fieldText.text)"/>
        <Property name="label" value=""/>
    </properties>
    <signals>
        <Signal name="textEdited"/>
    </signals>
    <resources>
        <Script>
            function start() {
                if (Page.mouseGrabbed != 'true') {
                    fieldText.state='editing';
                    open='true';
                    Page.mouseGrabbed='true';
                }
            }
            function confirm() {
                fieldText.text = textEdit.text;
                fieldText.state='';
                open='false';
                Page.mouseGrabbed='false';
                fieldText.textEdited.emit();
            }
            function cancel() {
                textEdit.text = fieldText.text;
                fieldText.state='';
                open='false';
                Page.mouseGrabbed='false';
            }
            function setText(value) {
                if (textEdit.text != value) {
                    fieldText.state='';
                    open='false';
                    textEdit.text = value;
                }
            }
        </Script>
    </resources>
    <Rect id="border" radius="5" anchors.fill="{parent}" color="{field.editable == 1 ? '#202020' : '#000000'}">
        <TextEdit id="textEdit" vAlign="AlignVCenter" text=""
            readOnly="true" font.bold="true" wrap="false" color="white"
            x="5" width="{parent.width-10}"
            anchors.verticalCenter="{parent.verticalCenter}"
            />
        <Text id="textLabel" vAlign="AlignVCenter" hAlign="AlignHCenter"
            color="#505050" font.italic="true"
            anchors.fill="{border}" text="{fieldText.label}"
            opacity="{textEdit.text == '' ? 1 : 0}">
            <opacity>
                <Behaviour>
                    <NumericAnimation target="{textLabel}" property="opacity" duration="250"/>
                </Behaviour>
            </opacity>
        </Text>
        <Image id="cancelIcon" src="../shared/pics/cancel.png"
            width="22" height="22" opacity="0"
            anchors.right="{parent.right}" anchors.rightMargin="4"
            anchors.verticalCenter="{parent.verticalCenter}"/>
        <Image id="confirmIcon" src="../shared/pics/ok.png"
            width="22" height="22" opacity="0"
            anchors.left="{parent.left}" anchors.leftMargin="4"
            anchors.verticalCenter="{parent.verticalCenter}"/>
        <MouseRegion anchors.fill="{cancelIcon}" onClicked="cancel()"/>
        <MouseRegion anchors.fill="{confirmIcon}" onClicked="confirm()"/>
        <MouseRegion id="editRegion" anchors.fill="{textEdit}" onClicked="start()"/>
    </Rect>
    <states>
        <State name="editing">
            <SetProperty target="{confirmIcon}" property="opacity" value="1"/>
            <SetProperty target="{cancelIcon}" property="opacity" value="1"/>
            <SetProperty target="{border}" property="color" value="white"/>
            <SetProperty target="{textEdit}" property="color" value="black"/>
            <SetProperty target="{textEdit}" property="readOnly" value="false"/>
            <SetProperty target="{textEdit}" property="focus" value="true"/>
            <SetProperty target="{textEdit}" property="x" value="35"/>
            <SetProperty target="{editRegion}" property="opacity" value="0"/>
        </State>
    </states>
    <transitions>
        <Transition fromState='' toState="*" reversible="true">
            <NumericAnimation target="{textEdit}" properties="x" duration="200"/>
            <NumericAnimation target="{confirmIcon}" properties="opacity" duration="200"/>
            <NumericAnimation target="{cancelIcon}" properties="opacity" duration="200"/>
            <ColorAnimation duration="150"/>
        </Transition>
    </transitions>
</Item>
