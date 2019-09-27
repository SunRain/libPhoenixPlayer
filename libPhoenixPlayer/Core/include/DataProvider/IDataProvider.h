#ifndef IDATAPROVIDER_H
#define IDATAPROVIDER_H

#include <QObject>

#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {
    namespace DataProvider {

class IDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit IDataProvider(QObject *parent = Q_NULLPTR);
    virtual ~IDataProvider();
};

} //DataProvider
} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::DataProvider::IDataProvider, "PhoenixPlayer.DataProvider.IDataProvider/1.0")

#endif // IDATAPROVIDER_H
