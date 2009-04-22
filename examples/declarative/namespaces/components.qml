<Item id="Root">
    <properties>
        <Property name="period" value="1000"/>
        <Property name="color" value="green"/>
    </properties>

    <Component id="SpinSquare">
        <Item>
            <Rect color="{Root.color}" width="{Root.width}" height="{width}" x="{-width/2}" y="{-height/2}"/>
            <rotation>
                <NumericAnimation from="0" to="360" repeat="true" running="true" duration="{Root.period}"/>
            </rotation>
        </Item>
    </Component>

    <ComponentInstance component="{SpinSquare}" x="{Root.width/2}" y="{Root.height/2}"/>
</Item>
