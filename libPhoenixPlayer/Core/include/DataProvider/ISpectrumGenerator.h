#ifndef ISPECTRUMGENERATOR_H
#define ISPECTRUMGENERATOR_H

#include <QObject>
#include <QtGlobal>

#include "AudioMetaObject.h"
#include "DataProvider/IDataProvider.h"

namespace PhoenixPlayer {
namespace DataProvider {


typedef QList<QList<qreal>> SpectrumDataList;

class ISpectrumGenerator : public IDataProvider
{
    Q_OBJECT
public:
    explicit ISpectrumGenerator(QObject *parent = Q_NULLPTR) : IDataProvider(parent) {}
    virtual ~ISpectrumGenerator() override {}

    virtual SpectrumDataList generate(const AudioMetaObject &obj) const = 0;

    // IDataProvider interface
public:
    virtual bool support(SupportedTypes type) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(type)
        return SupportedType::SupportUndefined;
    }
};

} //DataProvider
} //PhoenixPlayer


#endif // ISPECTRUMGENERATOR_H
