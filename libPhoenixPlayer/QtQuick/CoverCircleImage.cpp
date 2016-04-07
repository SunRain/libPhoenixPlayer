
#include "CoverCircleImage.h"

#include <QUrl>

#include "SingletonPointer.h"
#include "PlayerCore/PlayerCore.h"
#include "AudioMetaObject.h"

namespace PhoenixPlayer {
using namespace MusicLibrary;
namespace QmlPlugin {

CoverCircleImage::CoverCircleImage(CircleImage *parent)
    :CircleImage(parent)
{
    m_playCore = phoenixPlayerLib->playerCore ();//PlayerCore::instance ();
    m_autoChange = false;

//    SongMetaData *d = m_playCore->curTrackMetadata ();

//    if (d) {

//    }

    //Fuck Qt ......
    //lambda in Qml model making memory leak!!!!!
//    connect(mMusicLibraryManager,
//            &MusicLibraryManager::playingSongChanged,
//            [this]() {
//        if (autoChange()) {
//            qDebug()<<__FUNCTION__<<"playingSongHash()";
//            if (!mMusicLibraryManager) {
//                qDebug()<<__FUNCTION__<<"mMusicLibraryManager is null";
//            }
//            mSongHash = mMusicLibraryManager->playingSongHash();
//            setImage(mSongHash);
//        }
//    });
    QObject::connect (m_playCore, &PlayerCore::trackChanged, this, &CoverCircleImage::drawImage);

    drawImage ();
}

CoverCircleImage::~CoverCircleImage()
{
}

QUrl CoverCircleImage::defaultSource() const
{
    return m_defaultSource;
}

inline bool CoverCircleImage::autoChange() const
{
    return m_autoChange;
}

void CoverCircleImage::setDefaultSource(QUrl arg)
{
    m_defaultSource = arg;
}

void CoverCircleImage::setAutoChange(bool arg)
{
    m_autoChange = arg;
}

//QUrl CoverCircleImage::defaultSource() const
//{
//    return mDefaultSource;
//}

//bool CoverCircleImage::autoChange() const
//{
//    return mAutoChange;
//}

//QString CoverCircleImage::songHash() const
//{
//    return mSongHash;
//}

//void CoverCircleImage::setDefaultSource(const QUrl &source)
//{
//    if (source.isEmpty() || !source.isValid() || mDefaultSource == source)
//        return;
//    mDefaultSource = source;
//    this->setSource(mDefaultSource);
//}

//void CoverCircleImage::setAutoChange(bool autoChange)
//{
//    mAutoChange = autoChange;
//}

//void CoverCircleImage::setSongHash(const QString &hash)
//{
//    if (hash.isEmpty()) {
//        this->setSource(mDefaultSource);
//        return;
//    }
//    mSongHash = hash;
//    setImage(mSongHash);
//}

void CoverCircleImage::drawImage()
{
    if (autoChange()) {
//        AudioMetaObject *d = m_playCore->curTrackMetadata ();
        AudioMetaObject d = m_playCore->curTrackMetadata ();
        if (!d.isEmpty ())
            setImage (d.queryImgUri ());
        else
            setImage (QUrl());
    }
}

void CoverCircleImage::setImage(const QUrl &url)
{
    if (url.isEmpty () || !url.isValid ())
        this->setSource(m_defaultSource);
    else
        this->setSource(url);
}

} //QmlPlugin
} //PhoenixPlayer
