<Rect id="page" width="480" height="360" color='black'>
    <properties>
        <Property name="mouseGrabbed" value="false"/>
    </properties>
    <Item x="0" y="0" width="240" height="180">
        <ContactField1
            y="5"
            anchors.left="{parent.left}" anchors.leftMargin="5"
            anchors.right="{parent.right}" anchors.rightMargin="5"/>
        <ContactField2
            y="40"
            anchors.left="{parent.left}" anchors.leftMargin="5"
            anchors.right="{parent.right}" anchors.rightMargin="5"/>
        <ContactField3
            y="75"
            anchors.left="{parent.left}" anchors.leftMargin="5"
            anchors.right="{parent.right}" anchors.rightMargin="5"
            label="Phone Number"
            icon="../shared/pics/phone.png"
            value="123123"/>
    </Item>
    <Rect pen.color="gray" x="5" y="115" width="230" height="180" radius="5">
        <Contact3 anchors.fill="{parent}"
                label="Brian"
                phone="123123"
                email="brian@bigisp.com" />
    </Rect>
    <Rect pen.color="gray" x="245" y="115" width="230" height="180" radius="5">
        <Contact4 anchors.fill="{parent}"
                label="Brian"
                phone="123123"
                email="brian@bigisp.com" />
    </Rect>
</Rect>
