#ifndef ISPECTRUMGENERATOR_H
#define ISPECTRUMGENERATOR_H

#include <QObject>

#include "AudioMetaObject.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

class ISpectrumGenerator : public QObject
{
    Q_OBJECT
public:
    explicit ISpectrumGenerator(QObject *parent = Q_NULLPTR) : QObject(parent) {}
    virtual ~ISpectrumGenerator() {}

    virtual QList<QList<qreal>> generate(const AudioMetaObject &obj) const= 0;
};

} //MusicLibrary
} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::MusicLibrary::ISpectrumGenerator, "PhoenixPlayer.SpectrumGenerator.ISpectrumGenerator/1.0")

#endif // ISPECTRUMGENERATOR_H
