
/*
配置文件，需要根据平台不同自行修改相关内容
*/
import QtQuick 2.2
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0 as ListItem
import com.sunrain.phoenixplayer.qmlplugin 1.0

//             #define IMAGE_SIZE_REGEXP_LARGE "large"
//             #define IMAGE_SIZE_REGEXP_MEDIA "medium"
//             #define IMAGE_SIZE_REGEXP_SMALL "small"
//             #define IMAGE_SIZE_REGEXP_EXTRA_LARGE "extralarge"
//             #define IMAGE_SIZE_REGEXP_MEGA "mega"

ListItem.ItemSelector {
    id: selector
    function getSelectedIndex() {
        var s = settings.getConfig("LastFM_Image_Size", "medium");
        if (s == "small")
            return 0;
        if (s == "medium")
            return 1;
        if (s == "large")
            return 2;
        if (s == "extralarge")
            return 3;
        if (s == "mega")
            return 4;
    }

    function setSelectedIndex(index) {
        if (index == 0)
            settings.setConfig("LastFM_Image_Size", "small");
        else if (index == 1)
            settings.setConfig("LastFM_Image_Size", "medium");
        else if (index == 2)
            settings.setConfig("LastFM_Image_Size", "large");
        else if (index == 3)
            settings.setConfig("LastFM_Image_Size", "extralarge");
        else if (index == 4)
            settings.setConfig("LastFM_Image_Size", "mega");
        else
            settings.setConfig("LastFM_Image_Size", "medium");
    }

    text: qsTr("ImageSize")
    expanded: true
    model: [qsTr("small"),
        qsTr("medium"),
        qsTr("large"),
        qsTr("extralarge"),
        qsTr("mega")]
    selectedIndex: getSelectedIndex();

    onDelegateClicked: {
        setSelectedIndex(index)
    }
}
