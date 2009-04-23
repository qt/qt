<Item id="groupBox" width="{Math.max(270, subItem.width+40)}" height="{Math.max(70, subItem.height+40)}">
    <properties>
        <Property name="contents"/>
        <Property name="label"/>
    </properties>
    <Rect id="wrapper" x="5" y="10" radius="10"
        width="{groupBox.width-20}" height="{groupBox.height-20}"
        color="white" pen.color="black">
        <Item id="subItem" qml="{groupBox.contents}"
            anchors.top="{parent.top}" anchors.topMargin="10"
            anchors.right="{parent.right}" anchors.rightMargin="10"
            width="{qmlItem.width}" height="{qmlItem.height}"/>
    </Rect>
    <Rect x="20" y="0" height="{text.height}" width="{text.width+10}" color="white">
        <Text x="5" id="text" text="{label}" font.bold="true"/>
    </Rect>
</Item>
