import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

import com.sunrain.playlist 1.0
import com.sunrain.qmlplugin 1.0

ApplicationWindow {
    visible: true
    width: 360
    height: 960


    Connections {
        target: musicLibraryManager
        onSearching: {
            console.log("searching " + path + " " + file);
            searchText.text = path + "/" + file;
        }
        onPlayingSongChanged: {
            songtitle.text
                    = musicLibraryManager
            .querySongMetaElementByIndex(Common.E_FileName,
                                         musicLibraryManager.playingSongHash(),
                                         true);
            var lyrics = musicLibraryManager
            .querySongMetaElementByIndex(Common.E_Lyrics,
                                         musicLibraryManager.playingSongHash(),
                                         true)[0];
            lyricsModel.setLyricsStr(lyrics);
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

        songtitle.text = musicLibraryManager.querySongMetaElementByIndex(Common.E_FileName, musicLibraryManager.playingSongHash(), true);
    }
    Connections {
        target: musicPlayer
        onPlayModeChanged: {
            var m = mode;
            searchText.text = "play mode "+m;
        }
        onPlayStateChanged: {
            console.log("Play state change to " + state);
        }
        // playTickActual(quint64 sec);
        onPlayTickActual: {
            console.log("====== onPlayTickActual to " + sec);
            lyricsModel.findIndex(sec);
        }
        // playTickPercent(int percent);
        onPlayTickPercent: {
            console.log("====== onPlayTickPercent to " + percent);
            playProgress.value = percent;
        }
    }

    ListModel {id: libraryModel}
    ListModel {id: playListModel}

    LyricsModel {
        id: lyricsModel
        onCurrentIndexChanged: {
            console.log("=== qml onCurrentIndexChanged " + lyricsModel.currentIndex)
            lyricsView.currentIndex = lyricsModel.currentIndex;
        }
    }

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
        onClicked: {
            musicPlayer.togglePlayPause();
        }
    }

    Button {
        id: next
        x: 112
        y: 117
        text: qsTr("Next")
        onClicked: {musicLibraryManager.nextSong();}
    }

    Button {
        id: pre
        x: 237
        y: 78
        text: qsTr("Pre")
        onClicked: {musicLibraryManager.preSong();}
    }

    Button {
        id: radom
        x: 242
        y: 117
        text: qsTr("Radom")
        onClicked: {musicLibraryManager.randomSong();}
    }

    Text {
        id: songtitle
        x: 36
        y: 11
        width: 163
        height: 32
        font.pixelSize: 12
    }

    ListView {
        id: libraryList
        x: 17
        y: 162
        width: 300
        height: 195
        delegate: Item {
            x: 5
            width: parent.width
            height: column.height
            Column {
                id:column
                width: parent.width
                spacing: 3
                Text {
                    id: text
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
                Rectangle {
                    width: parent.width
                    height: 5
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    musicLibraryManager.setPlayingSongHash(Hash);
                }
            }

            Component.onCompleted: {
                var method = Common.E_SongTitle;
//                console.log("==== " + Hash)
                var p = musicLibraryManager.querySongMetaElementByIndex(method, Hash, true);
                if (p == "" || p == undefined) {
                    method = Common.E_FileName;
                    p = musicLibraryManager.querySongMetaElementByIndex(method, Hash, true);
                }
                text.text = p;
            }
        }
        model: libraryModel
    }

    ListView {
        id: playList
        x: 17
        y: 477
        width: 305
        height: 88
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
    GroupBox {
        x: 7
        y: 629
        title: "PlayMode"

        Column {
            ExclusiveGroup { id: playModeGroup }
            RadioButton {
                text: "ModeOrder"
                checked: true
                exclusiveGroup: playModeGroup
                onClicked: {
                    musicPlayer.setPlayModeInt(Common.ModeOrder);
                }
            }
            RadioButton {
                text: "ModeRepeatAll"
                exclusiveGroup: playModeGroup
                onClicked: {
                    musicPlayer.setPlayModeInt(Common.ModeRepeatAll);
                }
            }
            RadioButton {
                text: "ModeRepeatCurrent"
                exclusiveGroup: playModeGroup
                onClicked: {
                    musicPlayer.setPlayModeInt(Common.ModeRepeatCurrent);
                }
            }
            RadioButton {
                text: "ModeShuffle"
                exclusiveGroup: playModeGroup
                onClicked: {
                    musicPlayer.setPlayModeInt(Common.ModeShuffle);
                }
            }
        }
    }

    Button {
        id: lyricBtn
        x: 187
        y: 637
        text: qsTr("Button")
        onClicked: {
            musicPlayer.lookupLyric("");
        }
    }

    Slider {
        id: playProgress
        x: 29
        y: 55
        width: 288
        height: 22
        maximumValue: 100.0
        stepSize: 1.0

        property int jumpValue: 0
        onValueChanged: {
            console.log("playProgress value to " + value);
            if (playProgress.pressed)
                jumpValue = value

        }
        onPressedChanged: {
            console.log("playProgress onPressedChanged is pressed "
                        + playProgress.pressed);
            if (!playProgress.pressed && jumpValue >0) {
                console.log("playProgress jump to value " + jumpValue)
                playProgress.value = jumpValue;
                musicPlayer.setPosition(jumpValue, true)
                jumpValue = 0;
            }

        }

        //        style: SliderStyle {
        //            groove: Rectangle {
        //                width: control.width
        //                height: 1
        //                color: "#909090"
        //                Rectangle {
        //                    width: styleData.handlePosition
        //                    height: 1
        //                    color: "#e82828"
        //                }
        //            }
        //            handle: Image {
        //                source: "qrc:/resource/handle.png"
        //            }
        //        }
    }

    ListView {
        id: lyricsView
        x: 17
        y: 376
        width: 300
        height: 78
        delegate: Rectangle {
            width: parent.width
            height: 15
            color: Qt.rgba(0,0,0,0)
            Text {
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignHCenter
                text: lyricsText
                color: "#4c4c4c"
                font.pointSize: 10
                font.bold: parent.ListView.isCurrentItem
            }
        }
        model: lyricsModel
        clip: true
        spacing: 3
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlight: Rectangle {
            color: Qt.rgba(0,0,0,0)
            Behavior on y {
                SmoothedAnimation {
                    duration: 300
                }
            }
        }

    }

}
