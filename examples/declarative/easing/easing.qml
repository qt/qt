<Rect id="Window" width="640" height="{Layout.height}" color="white">

    <resources>
        <ListModel id="EasingTypes">
            <ListItem><type>easeLinear</type></ListItem>
            <ListItem><type>easeInQuad</type></ListItem>
            <ListItem><type>easeOutQuad</type></ListItem>
            <ListItem><type>easeInOutQuad</type></ListItem>
            <ListItem><type>easeOutInQuad</type></ListItem>
            <ListItem><type>easeInCubic</type></ListItem>
            <ListItem><type>easeOutCubic</type></ListItem>
            <ListItem><type>easeInOutCubic</type></ListItem>
            <ListItem><type>easeOutInCubic</type></ListItem>
            <ListItem><type>easeInQuart</type></ListItem>
            <ListItem><type>easeOutQuart</type></ListItem>
            <ListItem><type>easeInOutQuart</type></ListItem>
            <ListItem><type>easeOutInQuart</type></ListItem>
            <ListItem><type>easeInQuint</type></ListItem>
            <ListItem><type>easeOutQuint</type></ListItem>
            <ListItem><type>easeInOutQuint</type></ListItem>
            <ListItem><type>easeOutInQuint</type></ListItem>
            <ListItem><type>easeInSine</type></ListItem>
            <ListItem><type>easeOutSine</type></ListItem>
            <ListItem><type>easeInOutSine</type></ListItem>
            <ListItem><type>easeOutInSine</type></ListItem>
            <ListItem><type>easeInExpo</type></ListItem>
            <ListItem><type>easeOutExpo</type></ListItem>
            <ListItem><type>easeInOutExpo</type></ListItem>
            <ListItem><type>easeOutInExpo</type></ListItem>
            <ListItem><type>easeInCirc</type></ListItem>
            <ListItem><type>easeOutCirc</type></ListItem>
            <ListItem><type>easeInOutCirc</type></ListItem>
            <ListItem><type>easeOutInCirc</type></ListItem>
            <ListItem><type>easeInElastic</type></ListItem>
            <ListItem><type>easeOutElastic</type></ListItem>
            <ListItem><type>easeInOutElastic</type></ListItem>
            <ListItem><type>easeOutInElastic</type></ListItem>
            <ListItem><type>easeInBack</type></ListItem>
            <ListItem><type>easeOutBack</type></ListItem>
            <ListItem><type>easeInOutBack</type></ListItem>
            <ListItem><type>easeOutInBack</type></ListItem>
            <ListItem><type>easeOutBounce</type></ListItem>
            <ListItem><type>easeInBounce</type></ListItem>
            <ListItem><type>easeInOutBounce</type></ListItem>
            <ListItem><type>easeOutInBounce</type></ListItem>
        </ListModel>
    </resources>

    <VerticalLayout id="Layout" anchors.left="{Window.left}" anchors.right="{Window.right}">
        <Repeater dataSource="{EasingTypes}">
            <Component>
                <Text id="Text" text="{type}" height="18" font.italic="true">
                    <x>
                        <SequentialAnimation id="Anim">
                            <NumericAnimation from="0" to="{Window.width / 2}" easing="{type}" duration="1000"/>
                            <PauseAnimation duration="300"/>
                            <NumericAnimation to="0" from="{Window.width / 2}" easing="{type}" duration="1000"/>
                        </SequentialAnimation>
                    </x>
                    <children>
                        <MouseRegion onClicked="Anim.running=true" anchors.fill="{parent}"/>
                    </children>
                </Text>
            </Component>
        </Repeater>
    </VerticalLayout>
</Rect>
