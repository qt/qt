<Item id="contacts"
    width="240"
    height="230">
    <properties>
        <Property name="mouseGrabbed" value="false"/>
    </properties>
    <resources>
        <SqlConnection id="contactDatabase" name="qmlConnection" driver="QSQLITE" databaseName="../../shared/contacts.sqlite"/>
        <SqlQuery id="contactList" connection="{contactDatabase}">
            <query>SELECT recid, label, email, phone FROM contacts ORDER BY label, recid</query>
        </SqlQuery>
        <Component id="contactDelegate">
            <Text
                x="45" y="12"
                width="{contactListView.width-45}"
                height="30"
                color="black"
                font.bold="true"
                text="{model.label}"/>
        </Component>
    </resources>
    <ListView id="contactListView"
        anchors.fill="{parent}"
        clip="true"
        model="{contactList}"
        delegate="{contactDelegate}"
        focus="true"/>
</Item>
