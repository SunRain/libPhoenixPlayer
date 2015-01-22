import QtQuick 2.2
import QtQuick.Window 2.1

import com.sunrain.playlist 1.0

/*Window*/Item {
    visible: true
    width: 360
    height: 360

    Connections {
        target: musicLibraryManager
        //searching (QString path, QString file, qint64 size);
        onSearching: {
            infoText.text = path + "/" + file;
        }
        onPlayingSongChanged: {
            infoText.text = musicLibraryManager.playingSong();
        }
        onSearchingFinished: {
            infoText.text = "搜索完成, 当前音乐 " + musicLibraryManager.playingSong();
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }

        MouseArea {
            id: mouseArea1
            x: 34
            y: 70
            width: 62
            height: 35
            Text {
                text: qsTr("搜索")
            }
            onClicked: {
                console.log("=== 搜索")
                musicLibraryManager.scanLocalMusic();
            }
        }

        MouseArea {
            id: mouseArea2
            x: 124
            y: 70
            width: 52
            height: 42
            Text {
                text: qsTr("前一首")
            }
            onClicked: {
                musicLibraryManager.preSong();
            }
        }

        MouseArea {
            id: mouseArea3
            x: 199
            y: 66
            width: 58
            height: 51
            Text {
                text: qsTr("下一首")
            }
            onClicked: {
                musicLibraryManager.nextSong();
            }
        }

        MouseArea {
            id: mouseArea4
            x: 281
            y: 66
            width: 55
            height: 56
            Text {
                text: qsTr("随机")
            }
            onClicked: {
                musicLibraryManager.randomSong();
            }
        }
        MouseArea {
            id: mouseArea5
            x: 34
            y: 146
            width: 55
            height: 56
            Text {
                x: -22
                y: 8
                text: qsTr("测试音乐库query")
            }
            onClicked: {
                //querySongMetaElement(Common::MusicLibraryElement targetColumn = Common::E_NULLElement,
                //const QString &hash = "", bool skipDuplicates = true);
                var method = Common.E_FileName;
                musicLibraryManager.querySongMetaElementByIndex(method, "" , true);
            }
        }

        MouseArea {
            id: mouseArea6
            x: 137
            y: 138
            width: 136
            height: 25
            Text {
                text: qsTr("测试播放列表query")
            }
            onClicked: {
                //querySongMetaElement(Common::MusicLibraryElement targetColumn = Common::E_NULLElement,
                //const QString &hash = "", bool skipDuplicates = true);
                var method = Common.E_PlayListName;
                console.log("=== method is " + method );
                musicLibraryManager.queryPlayListElementByIndex(method, "");
            }
        }

        MouseArea {
            id: mouseArea7
            x: 26
            y: 223
            width: 85
            height: 75
            Text {
                text: qsTr("添加播放列表")
            }
            onClicked: {
                //querySongMetaElement(Common::MusicLibraryElement targetColumn = Common::E_NULLElement,
                //const QString &hash = "", bool skipDuplicates = true);
                musicLibraryManager.createPlayList("测试列表");
            }
        }

        MouseArea {
            id: mouseArea8
            x: 157
            y: 223
            width: 116
            height: 75
            Text {
                text: qsTr("删除播放列表")
            }
            onClicked: {
                //querySongMetaElement(Common::MusicLibraryElement targetColumn = Common::E_NULLElement,
                //const QString &hash = "", bool skipDuplicates = true);
//                musicLibraryManager.createPlayList("测试列表");
            }
        }
    }

    Text {
        id: infoText
        text: qsTr("Hello World")
        anchors.verticalCenterOffset: -138
        anchors.horizontalCenterOffset: -96
        anchors.centerIn: parent
    }

}
