#include "QtMultimediaBackend.h"

#include <QMediaPlayer>

namespace PhoenixPlayer {
namespace PlayBackend {
namespace QtMultimediaBackend {

QtMultimediaBackend::QtMultimediaBackend(QObject *parent)
    : IPlayBackend(parent)
{
    mPlayBackendState = Common::PlayBackendStopped;
    mPlayer = 0;
}

QtMultimediaBackend::~QtMultimediaBackend()
{
    if (mPlayer) {
        mPlayer->deleteLater ();
        mPlayer = 0;
    }
}

Common::PlayBackendState QtMultimediaBackend::getPlayBackendState()
{
    mPlayBackendState = Common::PlayBackendStopped;
    switch (mPlayer->state ()) {
    case mPlayer->StoppedState:
    default:
        mPlayBackendState = Common::PlayBackendStopped;
        break;
    case mPlayer->PlayingState:
        mPlayBackendState = Common::PlayBackendPlaying;
        break;
    case mPlayer->PausedState:
        mPlayBackendState = Common::PlayBackendPaused;
        break;
    }
    return mPlayBackendState;
}

void QtMultimediaBackend::init()
{
    if (!mPlayer) {
        mPlayer = new QMediaPlayer(this);
        mPlayer->setMuted (false);
        mPlayer->setVolume (50);
    }
    connect (mPlayer, &QMediaPlayer::positionChanged,
             [this](qint64 position) {
        emit tick (position/1000);
    });

    connect (mPlayer, &QMediaPlayer::volumeChanged,
             [this](int volume) {
        emit volumeChanged (volume);
    });

    connect (mPlayer, &QMediaPlayer::mediaStatusChanged,
             [this](QMediaPlayer::MediaStatus status) {

        qDebug()<<__FUNCTION__<<" mediaStatusChanged TO "<<status;

        if (status == QMediaPlayer::EndOfMedia) {
            emit finished ();
        } else if (status == QMediaPlayer::UnknownMediaStatus
                  || status == QMediaPlayer::NoMedia
                  || status == QMediaPlayer::InvalidMedia) {
            emit failed ();
        }
    });

    connect (mPlayer, &QMediaPlayer::stateChanged,
             [this](QMediaPlayer::State state) {
        Common::PlayBackendState pState;
        switch (state) {
        case QMediaPlayer::StoppedState:
        default:
            pState = Common::PlayBackendStopped;
            break;
        case QMediaPlayer::PlayingState:
            pState = Common::PlayBackendPlaying;
            break;
        case QMediaPlayer::PausedState:
            pState = Common::PlayBackendPaused;
            break;
        }
        emit stateChanged (pState);
    });

}

void QtMultimediaBackend::play(quint64 startSec)
{
    mPlayer->play ();
    if (startSec > 0)
        mPlayer->setPosition (startSec * 1000);
}

void QtMultimediaBackend::stop()
{
    mPlayer->stop ();
}

void QtMultimediaBackend::pause()
{
    mPlayer->pause ();
}

void QtMultimediaBackend::setVolume(int vol)
{
    mPlayer->setVolume (vol);
}

void QtMultimediaBackend::setPosition(quint64 sec)
{
    mPlayer->setPosition (sec * 1000);
}

void QtMultimediaBackend::changeMedia(BaseMediaObject *obj, quint64 startSec, bool startPlay)
{
    mPlayer->stop ();
    QString uri;
    if (!obj->filePath ().isEmpty () && !obj->fileName ().isEmpty ()) {
        uri = QString("%1/%2").arg (obj->filePath ()).arg (obj->fileName ());
    } else {
        uri = obj->filePath ().isEmpty () ? obj->fileName () : obj->filePath ();
    }

    if (obj->mediaType () == Common::MediaTypeLocalFile) {
        mPlayer->setMedia (QUrl::fromLocalFile (uri));
    } else if (uri.toLower ().startsWith ("http://")
               || uri.toLower ().startsWith ("https://")
               || uri.toLower ().startsWith ("ftp://")) {
        mPlayer->setMedia (QUrl(uri));
    }
    if (startPlay) {
        this->play (startSec);
    } else {
        this->setPosition (startSec);
    }
}

} //QtMultimediaBackend
} //PlayBackend
} //PhoenixPlayer
