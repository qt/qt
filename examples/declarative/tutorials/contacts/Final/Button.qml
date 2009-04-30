<Item id="button" width="30" height="30">
    <properties>
        <Property name="icon"/>
    </properties>
    <signals>
        <Signal name="clicked"/>
    </signals>
    <Rect id="buttonRect"
        anchors.fill="{parent}"
        color="lightgreen"
        radius="5">
        <Image id="iconImage"
            source="{button.icon}"
            anchors.horizontalCenter="{buttonRect.horizontalCenter}"
            anchors.verticalCenter="{buttonRect.verticalCenter}"/>
        <MouseRegion id="buttonMouseRegion"
            anchors.fill="{buttonRect}"
            onClicked="button.clicked.emit()"/>
        <states>
            <State name="pressed" when="{buttonMouseRegion.pressed == true}">
                <SetProperty target="{buttonRect}" property="color" value="green"/>
            </State>
        </states>
        <transitions>
            <Transition fromState="*" toState="pressed">
                <ColorAnimation duration="200"/>
            </Transition>
            <Transition fromState="pressed" toState="*">
                <ColorAnimation duration="1000"/>
            </Transition>
        </transitions>
    </Rect>
    <opacity>
        <Behaviour>
            <NumericAnimation property="opacity" duration="250"/>
        </Behaviour>
    </opacity>
</Item>
