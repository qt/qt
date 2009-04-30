<?qtfx namespacepath:http://nokia.com/qml/Chronos=. ?>

<Image id="clock" source="pics/clockface.png" xmlns:This="http://nokia.com/qml/Chronos">
    <properties>
        <Property name="hours" value="0"/>
        <Property name="minutes" value="0"/>
        <Property name="seconds" value="0"/>
    </properties>
    <DateTimeFormatter id="format" time="{clock.time}"/>
    <Item x="{clock.width/2}" y="{clock.height/2}">
        <This:Hand id="hour" length="{clock.height/4}" rotation="{clock.hours*30+clock.minutes/2+clock.seconds/120}"/>
        <This:Hand id="minute" length="{clock.height/3}" thickness="3" rotation="{clock.minutes*6+clock.seconds/10}"/>
        <This:Hand id="second" length="{clock.height/2.5}" thickness="1" rotation="{clock.seconds*6}"/>
    </Item>
</Image>
