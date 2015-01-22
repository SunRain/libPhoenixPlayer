import QtQuick 2.2
import QtQuick.Controls 1.1

import com.sunrain.playlist 1.0

ApplicationWindow {
    visible: true
    width: 360
    height: 640


    Connections {
        target: musicLibraryManager
        onSearching: {
            console.log("searching " + path + " " + file);
            searchText.text = path + "/" + file;
        }
        onPlayingSongChanged: {
            songtitle.text = musicLibraryManager.playingSong();
        }
        onSearchingFinished: {
              searchText.text = "搜索完成, 当前音乐 " + musicLibraryManager.playingSong();
        }
    }
    Component.onCompleted: {
        var method = Common.E_Hash;
        var result = musicLibraryManager.querySongMetaElementByIndex(method, "" , true);
//        console.log("result " + result);
        for (var i=0; i<result.length; i++) {
            libraryModel.append({"Hash":result[i]});
        }
    }

    ListModel {id: libraryModel}
    ListModel {id: playListModel}

    Image {
        id: image
        x: 28
        y: 78
        width: 63
        height: 57
        source: "qrc:/qtquickplugin/images/template_image.png"
    }

    Button {
        id: play
        x: 112
        y: 78
        text: qsTr("Play")
    }

    Button {
        id: next
        x: 112
        y: 117
        text: qsTr("Next")
    }

    Button {
        id: pre
        x: 237
        y: 78
        text: qsTr("Pre")
    }

    Button {
        id: radom
        x: 242
        y: 117
        text: qsTr("Radom")
    }

    Text {
        id: songtitle
        x: 64
        y: 23
        width: 203
        height: 24
        text: qsTr("SongTitle")
        font.pixelSize: 12
    }

    ListView {
        id: libraryList
        x: 17
        y: 171
        width: 300
        height: 186
        delegate: Item {
            x: 5
            width: parent.width
            height: column.height
            Column {
                id:column
                width: parent.width
                spacing: 3
                Text {
                    id: path
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
                Text {
                    text: Hash
                    font.bold: true
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
                Rectangle {
                    width: parent.width
                    height: 1
                }
            }
            Component.onCompleted: {
                var method = Common.E_FilePath;
                console.log("==== " + Hash)
                var p = musicLibraryManager.querySongMetaElementByIndex(method, Hash, true);
                method = Common.E_FileName;
                p = p + "/" +
                        musicLibraryManager.querySongMetaElementByIndex(method, Hash, true);
                path.text = p;
            }
        }
        model: libraryModel
    }

    ListView {
        id: playList
        x: 17
        y: 383
        width: 305
        height: 182
        delegate: Item {
            x: 5
            width: 80
            height: 40
            Row {
                id: row2
                Rectangle {
                    width: 40
                    height: 40
                    color: colorCode
                }

                Text {
                    text: name
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }
                spacing: 10
            }
        }
        model: playListModel
    }

    Button {
        id: searchMusic
        x: 36
        y: 589
        text: qsTr("Scan")
        MouseArea {
            anchors.fill: parent
            onClicked: {
                musicLibraryManager.scanLocalMusic();
            }
        }
    }

    Text {
        id: searchText
        x: 147
        y: 583
        width: 185
        height: 26
        text: qsTr("Searching ...")
        font.pixelSize: 12
    }

}
