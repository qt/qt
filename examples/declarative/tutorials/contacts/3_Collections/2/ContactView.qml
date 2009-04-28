<Item id="contacts"
    width="240"
    height="230">
    <properties>
        <Property name="mouseGrabbed" value="false"/>
    </properties>
    <resources>
        <SqlConnection id="contactDatabase" name="qmlConnection" driver="QSQLITE" databaseName="../../shared/contacts.sqlite"/>
        <SqlQuery id="contactList" connection="{contactDatabase}">
            <query>SELECT recid AS contactid, label, email, phone FROM contacts ORDER BY label, recid</query>
        </SqlQuery>
        <Component id="contactDelegate">
            <Item id="wrapper"
                x="0"
                width="{ListView.view.width}" height="34">
                <Text id="label"
                    x="45" y="12"
                    width="{parent.width-45}" 
                    text="{model.label}"
                    color="black"
                    font.bold="true">
                </Text>
                <MouseRegion
                    anchors.fill="{label}"
                    onClicked="wrapper.state='opened'"/>
                <Contact id="details"
                    anchors.fill="{parent}"
                    contactid="{model.contactid}"
                    label="{model.label}"
                    email="{model.email}"
                    phone="{model.phone}"
                    opacity="0"/>
                <states>
                    <State name='opened'>
                        <SetProperty target="{wrapper}" property="height" value="{contactListView.height}"/>
                        <SetProperty target="{contactListView}" property="yPosition" value="{wrapper.y}"/>
                        <SetProperty target="{contactListView}" property="locked" value="1"/>
                        <SetProperty target="{label}" property="opacity" value="0"/>
                        <SetProperty target="{details}" property="opacity" value="1"/>
                    </State>
                </states>
                <transitions>
                    <Transition>
                        <NumericAnimation duration="500" properties="yPosition,height,opacity"/>
                    </Transition>
                </transitions>
                <Connection sender="{cancelEditButton}" signal="clicked()">
                    if (wrapper.state == 'opened') {
                        wrapper.state = '';
                    }
                </Connection>
            </Item>
        </Component>
    </resources>
    <Button id="cancelEditButton"
        anchors.top="{parent.top}" anchors.topMargin="5"
        anchors.right="{parent.right}" anchors.rightMargin="5"
        icon="../../shared/pics/cancel.png"
        opacity="{mouseGrabbed ? 0 : 1}"/>
    <ListView id="contactListView"
        anchors.left="{parent.left}"
        anchors.right="{parent.right}"
        anchors.top="{cancelEditButton.bottom}"
        anchors.bottom="{parent.bottom}"
        clip="true"
        model="{contactList}"
        delegate="{contactDelegate}"
        focus="true"/>
</Item>
