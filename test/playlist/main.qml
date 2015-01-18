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
                text: qsTr("测试query")
            }
            onClicked: {
                //querySongMetaElement(Common::MusicLibraryElement targetColumn = Common::E_NULLElement,
                //const QString &hash = "", bool skipDuplicates = true);
                var method = Common.E_FileName;
                musicLibraryManager.querySongMetaElement(method, "" , true);
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
