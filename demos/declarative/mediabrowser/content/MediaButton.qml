<Item id="Container">
    <signals>
        <Signal name="clicked"/>
    </signals>

    <properties>
        <Property name="text"/>
    </properties>

    <Image id="Image" source="pics/button.png"/>
    <Image id="Pressed" source="pics/button-pressed.png" opacity="0"/>
    <MouseRegion id="MouseRegion" anchors.fill="{Image}" onClicked="Container.clicked.emit();"/>
    <Text font.bold="true" color="white" anchors.centeredIn="{Image}" text="{Container.text}"/>
    <width>{Image.width}</width>

    <states>
        <State name="Pressed" when="{MouseRegion.pressed == true}">
            <SetProperties target="{Pressed}" opacity="1"/>
        </State>
    </states>
</Item>
