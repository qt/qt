<Item id="Container">
    <properties>
        <Property name="flickableArea"/>
    </properties>

    <Rect radius="5" color="black" opacity="0.3" pen.color="white" pen.width="2" x="0" y="{flickableArea.pageYPosition * Container.height}"
          width="{parent.width}" height="{flickableArea.pageHeight * Container.height}"/>
    <states>
        <State name="show" when="{flickableArea.moving}">
            <SetProperties target="{Container}" opacity="1" />
        </State>
    </states>
    <transitions>
        <Transition fromState="*" toState="*">
            <NumericAnimation target="{Container}" properties="opacity" duration="400"/>
        </Transition>
    </transitions>
</Item>
