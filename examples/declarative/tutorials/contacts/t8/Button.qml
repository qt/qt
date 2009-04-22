<Item width="30" height="30" id="button">
    <properties>
        <Property name="icon"/>
    </properties>
    <signals>
        <Signal name="clicked"/>
    </signals>
    <Rect id="buttonRect" color="lightgreen" anchors.fill="{parent}" radius="5">
        <Image id="iconImage"
            src="{button.icon}"
            anchors.horizontalCenter="{buttonRect.horizontalCenter}"
            anchors.verticalCenter="{buttonRect.verticalCenter}"/>
        <MouseRegion id="buttonMouseRegion" anchors.fill="{buttonRect}" onClicked="button.clicked.emit()"/>
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
