<Rect id="Page" color="black" width="240" height="320">
    <properties>
        <Property name="listShown" value="1"/>
        <Property name="mode" value="list"/>
        <Property name="mouseGrabbed" value="false"/>
    </properties>
    <resources>
        <SqlConnection id="contactDatabase" name="qmlConnection" driver="QSQLITE" databaseName="../shared/contacts.sqlite"/>
        <SqlQuery id="contactList" connection="{contactDatabase}">
            <query>SELECT recid, label, email, phone FROM contacts WHERE lower(label) LIKE lower(:searchTerm) ORDER BY label, recid</query>
            <bindings>
                <SqlBind name=":searchTerm" value="{searchBar.text + '%' }"/>
            </bindings>
        </SqlQuery>
        <Component id="contactDelegate">
            <Item id="wrapper" x="0" width="{ListView.view.width}" height="34">
                <Text id="label" x="45" y="12" text="{model.label}" color="white" font.bold="true" width="{parent.width-30}" opacity="{listShown}"/>
                <Item id="Details" anchors.fill="{wrapper}">
                    <Bind target="{Details.qmlItem}" property="contactid" value="{model.recid}"/>
                    <Bind target="{Details.qmlItem}" property="mode" value="{wrapper.state}"/>
                    <Bind target="{Details.qmlItem}" property="label" value="{model.label}"/>
                    <Bind target="{Details.qmlItem}" property="phone" value="{model.phone}"/>
                    <Bind target="{Details.qmlItem}" property="email" value="{model.email}"/>
                </Item>
                <MouseRegion anchors.fill="{label}" opacity="{listShown}">
                    <onClicked>
                        Details.qml = 'Contact.qml';
                        wrapper.state='opened';
                    </onClicked>
                </MouseRegion>
                <states>
                    <State name='opened'>
                        <SetProperty target="{wrapper}" property="height" value="{contactListView.height}"/>
                        <SetProperty target="{contactListView}" property="yPosition" value="{wrapper.y}"/>
                        <SetProperty target="{contactListView}" property="locked" value="1"/>
                        <SetProperty target="{Details.qmlItem}" property="mode" value="opened"/>
                        <SetProperty target="{Page}" property="mode" value="editExisting"/>
                    </State>
                </states>
                <transitions>
                    <Transition>
                        <NumericAnimation duration="500" properties="yPosition,height,opacity"/>
                    </Transition>
                </transitions>
                <Connection sender="{confirmEditButton}" signal="clicked()">
                    if (Details.qmlItem.mode == 'opened' &amp;&amp; Page.mouseGrabbed != 'true') {
                        Details.qmlItem.mode = 'closed';
                        wrapper.state = "";
                        Details.qmlItem.confirm.emit();
                    }
                </Connection>
                <Connection sender="{cancelEditButton}" signal="clicked()">
                    if (Details.qmlItem.mode == 'opened' &amp;&amp; Page.mouseGrabbed != 'true') {
                        Details.qmlItem.mode = 'closed';
                        wrapper.state = "";
                        Details.qmlItem.cancel.emit();
                    }
                </Connection>
            </Item>
        </Component>
    </resources>
    <Button id="newContactButton" icon="../shared/pics/new.png"
        anchors.top="{parent.top}" anchors.topMargin="5"
        anchors.right="{parent.right}" anchors.rightMargin="5"
        onClicked="newContactItem.label = ''; newContactItem.phone = ''; newContactItem.email = ''; Page.mode = 'editNew'"
        opacity="{Page.mode == 'list' ? 1 : 0}"/>
    <Button id="cancelEditButton" icon="../shared/pics/cancel.png"
        anchors.top="{parent.top}" anchors.topMargin="5"
        anchors.right="{parent.right}" anchors.rightMargin="5"
        opacity="{Page.mode == 'list' || Page.mouseGrabbed == 'true' ? 0 : 1}"/>
    <Button id="confirmEditButton" icon="../shared/pics/ok.png"
        anchors.top="{parent.top}" anchors.topMargin="5"
        anchors.left="{parent.left}" anchors.leftMargin="5"
        opacity="{Page.mode == 'list' || Page.mouseGrabbed == 'true' ? 0 : 1}"/>
    <FocusRealm id="searchBarRealm"
        height="30"
        anchors.bottom="{parent.bottom}"
        anchors.left="{parent.left}" anchors.right="{parent.right}"
        focus="{Page.mode == 'list' ? 'true' : 'false'}">
        <SearchBar id="searchBar" anchors.fill="{parent}">
            <states>
                <State name="searchHidden" when="{searchBar.text == '' || Page.listShown == 0}">
                    <SetProperty target="{searchBarRealm}" property="anchors.bottomMargin" value="-30"/>
                </State>
            </states>
            <transitions>
                <Transition fromState="*" toState="*">
                    <NumericAnimation property="bottomMargin" duration="250"/>
                </Transition>
            </transitions>
        </SearchBar>
    </FocusRealm>
    <ListView id="contactListView" model="{contactList}" delegate="{contactDelegate}"
        anchors.top="{newContactButton.bottom}" anchors.topMargin="10"
        anchors.left="{parent.left}" anchors.right="{parent.right}"
        anchors.bottom="{searchBarRealm.top}"
        clip="true"
        focus="{Page.mode == 'list' ? 'false' : 'true'}"
        />
    <Contact id="newContactItem"
        mode="opened"
        anchors.top="{newContactButton.bottom}" anchors.topMargin="10"
        anchors.left="{parent.left}" anchors.right="{parent.right}"
        anchors.bottom="{searchBarRealm.top}"
        onClose="Page.mode='list'"
        opacity="0"
    />
    <Connection sender="{confirmEditButton}" signal="clicked()">
        if (Page.mode == 'editNew' &amp;&amp; Page.mouseGrabbed != 'true') {
            newContactItem.confirm.emit()
        }
    </Connection>
    <Connection sender="{cancelEditButton}" signal="clicked()">
        if (Page.mode == 'editNew' &amp;&amp; Page.mouseGrabbed != 'true') {
            newContactItem.cancel.emit()
        }
    </Connection>
    <states>
        <State name="editExistingState" when="{Page.mode == 'editExisting'}">
            <SetProperty target="{Page}" property="listShown" value="0"/>
        </State>
        <State name="editNewState" when="{Page.mode == 'editNew'}">
            <SetProperty target="{Page}" property="listShown" value="0"/>
            <SetProperty target="{contactListView}" property="opacity" value="0"/>
            <SetProperty target="{newContactItem}" property="opacity" value="1"/>
        </State>
    </states>
    <transitions>
        <Transition fromState="*" toState="*">
            <NumericAnimation property="opacity" duration="500"/>
        </Transition>
    </transitions>
</Rect>
