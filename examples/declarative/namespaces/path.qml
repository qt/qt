<!-- Empty Namespaces paths allow unqualified Types to be found
     in other locations. For file URLs, multiple paths can be
     given, forming a file search path. -->

<?qtfx namespacepath:=lib/Chronos ?> <!-- Clock will be found here -->
<?qtfx namespacepath:=lib/Path ?> <!-- PathLabel will be found here -->

<Rect id="obj" width="200" height="200">
 <properties>
     <Property name="time_sec"/>
 </properties>
 <time_sec> <!-- simple animation, not bound to the real time -->
    <NumericAnimation from="0" to="43200" duration="43200000" running="true" repeat="true"/>
 </time_sec>
 <Clock x="10" y="10" width="{parent.width-20}" height="{parent.height-20}"
    hours="{Math.floor(time_sec/3600)}" minutes="{Math.floor(time_sec/60)%60}" seconds="{time_sec%60}"/>
 <PathLabel text="This is a clock"/>
</Rect>
