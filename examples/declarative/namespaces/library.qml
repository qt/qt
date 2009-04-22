<?qtfx namespacepath:http://nokia.com/qml=lib ?>

<Rect id="obj" width="200" height="200" xmlns:Chronos="http://nokia.com/qml/Chronos">
 <properties>
     <Property name="time_sec"/>
 </properties>
 <time_sec> <!-- simple animation, not bound to the real time -->
    <NumericAnimation from="0" to="43200" duration="43200000" running="true" repeat="true"/>
 </time_sec>
 <Chronos:Clock x="10" y="10" width="{parent.width-20}" height="{parent.height-20}"
    hours="{Math.floor(time_sec/3600)}" minutes="{Math.floor(time_sec/60)%60}" seconds="{time_sec%60}"/>
</Rect>
