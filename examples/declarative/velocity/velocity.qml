<Rect color="lightSteelBlue" width="800" height="600">
    <ListModel id="List">
        <Day>
            <name>Sunday</name>
            <dayColor>#808080</dayColor>
            <notes>
                <Note noteText="Lunch" />
                <Note noteText="Party" />
            </notes>
        </Day>
        <Day>
            <name>Monday</name>
            <dayColor>blue</dayColor>
            <notes>
                <Note noteText="Pickup kids" />
                <Note noteText="Checkout kinetic" />
                <Note noteText="Read email" />
            </notes>
        </Day>
        <Day>
            <name>Tuesday</name>
            <dayColor>yellow</dayColor>
            <notes>
                <Note noteText="Walk dog" />
                <Note noteText="Buy newspaper" />
            </notes>
        </Day>
        <Day>
            <name>Wednesday</name>
            <dayColor>purple</dayColor>
            <notes>
                <Note noteText="Cook dinner" />
                <Note noteText="Eat dinner" />
            </notes>
        </Day>
        <Day>
            <name>Thursday</name>
            <dayColor>blue</dayColor>
            <notes>
                <Note noteText="5:30pm Meeting" />
                <Note noteText="Weed garden" />
            </notes>
        </Day>
        <Day>
            <name>Friday</name>
            <dayColor>green</dayColor>
            <notes>
                <Note noteText="Still work" />
                <Note noteText="Drink" />
            </notes>
        </Day>
        <Day>
            <name>Saturday</name>
            <dayColor>orange</dayColor>
            <notes>
                <Note noteText="Drink" />
                <Note noteText="Drink" />
            </notes>
        </Day>
    </ListModel>

    <Flickable id="Flick" anchors.fill="{parent}" viewportWidth="{Lay.width}">
        <HorizontalLayout id="Lay">
            <Repeater dataSource="{List}">
                <Component>
                    <Day day="{name}" color="{dayColor}" stickies="{notes}"/>
                </Component>
            </Repeater>
            <!--
            <Day color="#808080" day="Sunday" />
            <Day color="blue" day="Monday"/>
            <Day color="yellow" day="Tuesday"/>
            <Day color="purple" day="Wednesday"/>
            <Day color="blue" day="Thursday"/>
            <Day color="green" day="Friday"/>
            <Day color="orange" day="Saturday"/>
            -->
        </HorizontalLayout>
    </Flickable>

</Rect>
