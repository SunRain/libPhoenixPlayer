#include "Utility.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QByteArray>
#include <QTextCodec>
#include <QLocale>
#include <QMutex>
#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QQmlEngine>
#include <QNetworkAccessManager>
#include <QNetworkConfiguration>

#include "PluginLoader.h"
#include "PluginHost.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {

Utility::Utility(QObject *parent) : QObject(parent)
{
    m_QQmlEngine = nullptr;
}

//#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//Util *Util::instance()
//{
//    static QMutex mutex;
//    static QScopedPointer<Util> scp;

//    if (Q_UNLIKELY(scp.isNull())) {
//        qDebug()<<Q_FUNC_INFO<<">>>>>>>> statr new";
//        mutex.lock();
//        scp.reset(new Util(0));
//        mutex.unlock();
//    }
//    return scp.data();
//}
//#endif

Utility::~Utility()
{

}

void Utility::setQQmlEngine(QQmlEngine *engine)
{
    m_QQmlEngine = engine;
}

QString Utility::calculateHash(const QString &str)
{
    return QString (QCryptographicHash::hash (str.toLocal8Bit (), QCryptographicHash::Md5).toHex ());
}

QTextCodec *Utility::localeDefaultCodec()
{
    switch(QLocale::system().country())
       {
       case QLocale::China:
           return QTextCodec::codecForName("GB18030");
       case QLocale::HongKong:
           return QTextCodec::codecForName("Big5-HKSCS");
       case QLocale::Macau:
       case QLocale::Taiwan:
           return QTextCodec::codecForName("Big5");
       case QLocale::Japan:
           return QTextCodec::codecForName("Shift-JIS");
       default:
           return QTextCodec::codecForName("ISO 8859-1");
    }
}

QString Utility::formateSongDuration(int time)
{
    if (time <= 0)
        return QString("00:00");

    QString str;
    if (time < 10) {
        str = QString("00:0%1").arg(time);
    } else if (time >= 10 && time < 60) {
        str = QString("00:%1").arg(time);
    } else {
        QString m = QString::number(time / 60);
        if (m.toInt() < 10)
            m = QString("0%1").arg(m);
        QString s = QString::number(time % 60);
        if (s.toInt() < 10)
            s = QString("0%1").arg(s);
        str = QString("%1:%2").arg(m).arg(s);
    }
    return str;
}

QString Utility::formateFileSize(int size)
{
    if (size < 0)
        return QString("0B");
    QString str;
    if (size < 1024) { //bytes
        str = QString("%1 B").arg(size);
    } else if (size >= 1024 && size < 1024 * 1024) { //kb
        str = QString("%1 KB").arg(QString::number(((float)size)/1024, 'f', 2));
    } else { //mb
        str = QString("%1 MB").arg(QString::number(((float)size)/1024/1024, 'f', 2));
    }
    return str;
}

QStringList Utility::getAddonDirList()
{
    QStringList list;
#ifdef UBUNTU_TOUCH
    QDir dir(qApp->applicationDirPath ());
    dir.cdUp ();
    list.append (QString("%1/addon").arg (dir.absolutePath ()));
#endif
    QString dataPath = QStandardPaths::writableLocation (QStandardPaths::DataLocation);
    list.append (QString("%1/addon").arg (dataPath));
    return list;
}

Utility::NetworkType Utility::getNetworkType()
{
    if (!m_QQmlEngine)
        return Utility::NetworkType::TypeUnknown;

    QNetworkConfiguration::BearerType t = m_QQmlEngine->networkAccessManager ()
            ->activeConfiguration ().bearerType ();
//    qDebug()<<Q_FUNC_INFO<<" BearerType is "<<t<<" str "<<mQQmlEngine->networkAccessManager ()->activeConfiguration ().bearerTypeName ();

    switch (t) {
    case QNetworkConfiguration::BearerEthernet:
        return Utility::NetworkType::TypeEthernet;
    case QNetworkConfiguration::BearerUnknown:
        return Utility::NetworkType::TypeUnknown;
    case QNetworkConfiguration::BearerWLAN:
        return Utility::NetworkType::TypeWLAN;
    default:
        return Utility::NetworkType::TypeMobile;
    }
}

//QString Util::supportMDLookupTypeJsonStr(const QString &pluginHash)
//{
//    if (pluginHash.isEmpty ())
//        return QString();
//    PluginLoader *loader = PluginLoader::instance ();
//    PluginHost *h = loader->getPluginHostByHash (pluginHash);
//    QObject *o = h->instance ();
//    if (o) {
//        MetadataLookup::IMetadataLookup *p = qobject_cast<MetadataLookup::IMetadataLookup *>(o);
//        if (p) {
////            TypeUndefined = 0x0,
////            TypeLyrics,             //歌词
////            TypeAlbumImage,         //专辑封面
////            TypeAlbumDescription,   //专辑介绍
////            TypeAlbumDate,          //
////            TypeArtistImage,        //艺术家封面
////            TypeArtistDescription,  //艺术家信息
////            TypeTrackDescription    //歌曲信息
//            QString s = "{";
//            s += QString("\"KEY_%1\":%2,").arg (MetadataLookup::IMetadataLookup::TypeAlbumDate)
//                    .arg (p->supportLookup (MetadataLookup::IMetadataLookup::TypeAlbumDate));

//            s += QString("\"KEY_%1\":%2,").arg (MetadataLookup::IMetadataLookup::TypeAlbumDescription)
//                    .arg (p->supportLookup (MetadataLookup::IMetadataLookup::TypeAlbumDescription));

//            s += QString("\"KEY_%1\":%2,").arg (MetadataLookup::IMetadataLookup::TypeAlbumImage)
//                    .arg (p->supportLookup (MetadataLookup::IMetadataLookup::TypeAlbumImage));

//            s += QString("\"KEY_%1\":%2,").arg (MetadataLookup::IMetadataLookup::TypeArtistDescription)
//                    .arg (p->supportLookup (MetadataLookup::IMetadataLookup::TypeArtistDescription));

//            s += QString("\"KEY_%1\":%2,").arg (MetadataLookup::IMetadataLookup::TypeArtistImage)
//                    .arg (p->supportLookup (MetadataLookup::IMetadataLookup::TypeArtistImage));

//            s += QString("\"KEY_%1\":%2,").arg (MetadataLookup::IMetadataLookup::TypeLyrics)
//                    .arg (p->supportLookup (MetadataLookup::IMetadataLookup::TypeLyrics));

//            s += QString("\"KEY_%1\":%2").arg (MetadataLookup::IMetadataLookup::TypeTrackDescription)
//                    .arg (p->supportLookup (MetadataLookup::IMetadataLookup::TypeTrackDescription));

//            s += "}";

//            return s;
//        }
//    }
//    return QString();
//}

} //PhoenixPlayer
