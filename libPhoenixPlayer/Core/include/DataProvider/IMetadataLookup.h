#ifndef IMETADATALOOKUP
#define IMETADATALOOKUP

#include <QObject>
#include <QtGlobal>

#include "libphoenixplayer_global.h"
#include "DataProvider/IDataProvider.h"

namespace PhoenixPlayer{
    class AudioMetaObject;

    namespace DataProvider {

class LIBPHOENIXPLAYER_EXPORT IMetadataLookup : public IDataProvider
{
    Q_OBJECT
public:
    explicit IMetadataLookup(QObject *parent = Q_NULLPTR);

    virtual ~IMetadataLookup() override;

//    /*!
//     * \brief setCurrentLookupFlag
//     * 设置当前插件的查询状态,便于外部调用查看插件查询类型
//     * \param type
//     */
//    inline void setCurrentLookupFlag(SupportedType type = SupportedType::SupportUndefined)
//    {
//        m_type = type;
//    }

//    /*!
//     * \brief currentLookupFlag
//     * \return 得到当前插件查询的类型
//     */
//    inline SupportedType currentLookupFlag() const
//    {
//        return m_type;
//    }

    /*!
     * \brief lookup
     * Use synchronization method to reimplement this function
     * \param object Object need to lookup
     * \param tyep Lookup type
     * \param errorMsg
     * \return Empty QByteArray if failure
     */
    virtual QByteArray lookup(const AudioMetaObject &object, SupportedType type, QString *errorMsg) const = 0;

    // IDataProvider interface
public:
    virtual bool support(SupportedTypes type) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(type)
        return SupportedType::SupportUndefined;
    }

private:
//    SupportedType m_type;
};

} //DataProvider
} //PhoenixPlayer

#endif // IMETADATALOOKUP

