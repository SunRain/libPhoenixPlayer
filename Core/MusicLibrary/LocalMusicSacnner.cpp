#include "MusicLibrary/LocalMusicSacnner.h"

#include <QDebug>
#include <QCoreApplication>

#include "MusicLibrary/IPlayListDAO.h"
#include "AsyncDiskLookup.h"
#include "AsyncTagParserMgrWrapper.h"
#include "PluginLoader.h"
#include "Settings.h"
#include "Util.h"
#include "SongMetaData.h"
#include "SingletonPointer.h"
#include "SongMetaData.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

LocalMusicSacnner::LocalMusicSacnner(QObject *parent) :
    QObject(parent)
{
    mTagParserWrapper = nullptr;
    mAsyncDiskLookup = nullptr;

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    mSettings = Settings::instance();
    mPluginLoader = PluginLoader::instance();
#else
    qDebug()<<"For other os";
    mSettings = SingletonPointer<Settings>::instance ();
    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
#endif
    mDao = mPluginLoader->getCurrentPlayListDAO ();
}

LocalMusicSacnner::~LocalMusicSacnner()
{
    if (mTagParserWrapper) {
        mTagParserWrapper->deleteLater ();
    }
    if (mAsyncDiskLookup) {
        mAsyncDiskLookup->deleteLater ();
    }
}

void LocalMusicSacnner::scanLocalMusic()
{
    if (mTagParserWrapper == nullptr) {
        mTagParserWrapper = new AsyncTagParserMgrWrapper(this);
        connect (mTagParserWrapper, &AsyncTagParserMgrWrapper::started, [&] {
            qDebug()<<Q_FUNC_INFO<<" mTagParserWrapper started";
        });

        connect (mTagParserWrapper, &AsyncTagParserMgrWrapper::finished, [&] {
            qDebug()<<Q_FUNC_INFO<<" mTagParserWrapper finished";

            mDao->beginTransaction ();
            for (int i=0; i<mMetaDataList.size (); ++i) {
                qDebug()<<Q_FUNC_INFO<<" insert ("<<i<<") ["<<mMetaDataList.at (i)->toString ()<<"]";
                mDao->insertMetaData (mMetaDataList.at (i));
            }
            mDao->commitTransaction ();

            mTagParserWrapper->deleteLater ();
            mTagParserWrapper = nullptr;

            emit searchingFinished ();
        });
    }

    if (mAsyncDiskLookup == nullptr) {
        mAsyncDiskLookup = new AsyncDiskLookup(this);

        connect (mAsyncDiskLookup, &AsyncDiskLookup::started, [&] {
            qDebug()<<Q_FUNC_INFO<<" AsyncDiskLookup::started";
        });

        connect (mAsyncDiskLookup, &AsyncDiskLookup::finished,
                 [&] (const QList<SongMetaData *> &list){
            qDebug()<<Q_FUNC_INFO<<" AsyncDiskLookup::finished , result list size "<<list.size();
            foreach (SongMetaData *d, list) {
                SongMetaData *data = new SongMetaData(0);
                for (int i = (int)(Common::SongMetaTags::E_FirstFlag) +1;
                     i < (int)Common::SongMetaTags::E_LastFlag;
                     ++i) {
                    data->setMeta (Common::SongMetaTags(i), d->getMeta (Common::SongMetaTags(i)));
                }
                mMetaDataList.append (data);
            }

            mAsyncDiskLookup->deleteLater ();
            mAsyncDiskLookup = nullptr;
            mTagParserWrapper->parser (&mMetaDataList);
        });
    }

    if (mTagParserWrapper != nullptr && mTagParserWrapper->isRunning ()) {
        qWarning()<<Q_FUNC_INFO<<" Parser track tags, can't scan local music atm";
        return;
    }

    mAsyncDiskLookup->setLookupDirs (mSettings->getMusicDirs ());
    mAsyncDiskLookup->startLookup ();
}

} //MusicLibrary
} //PhoenixPlayer
