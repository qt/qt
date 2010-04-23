import Qt 4.7

Item { id: wrapper
    property variant model: xmlModel
    property string tags : ""
    property string authName : ""
    property string authPass : ""
    property string mode : "everyone"
    property int status: xmlModel.status
    function reload() { xmlModel.reload(); }
XmlListModel {
    id: xmlModel

    source:{ 
            if (wrapper.authName == ""){
                ""; //Avoid worthless calls to twitter servers
            }else if(wrapper.mode == 'user'){
                "https://"+ ((wrapper.authName!="" && wrapper.authPass!="")? (wrapper.authName+":"+wrapper.authPass+"@") : "" )+"twitter.com/statuses/user_timeline.xml?screen_name="+wrapper.tags;
            }else if(wrapper.mode == 'self'){
                "https://"+ ((wrapper.authName!="" && wrapper.authPass!="")? (wrapper.authName+":"+wrapper.authPass+"@") : "" )+"twitter.com/statuses/friends_timeline.xml";
            }else{//everyone/public
                "http://twitter.com/statuses/public_timeline.xml";
            }
    }
    query: "/statuses/status"

    XmlRole { name: "statusText"; query: "text/string()" }
    XmlRole { name: "timestamp"; query: "created_at/string()" }
    XmlRole { name: "source"; query: "source/string()" }
    XmlRole { name: "userName"; query: "user/name/string()" }
    XmlRole { name: "userScreenName"; query: "user/screen_name/string()" }
    XmlRole { name: "userImage"; query: "user/profile_image_url/string()" }
    XmlRole { name: "userLocation"; query: "user/location/string()" }
    XmlRole { name: "userDescription"; query: "user/description/string()" }
    XmlRole { name: "userFollowers"; query: "user/followers_count/string()" }
    XmlRole { name: "userStatuses"; query: "user/statuses_count/string()" }
    //TODO: Could also get the user's color scheme, timezone and a few other things
}
Binding {
    property: "mode"
    target: wrapper
    value: {if(wrapper.tags==''){"everyone";}else if(wrapper.tags=='my timeline'){"self";}else{"user";}}
}
}
