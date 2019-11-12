#ifndef IMUSICTAGPARSER
#define IMUSICTAGPARSER

#include <QtGlobal>
#include <QObject>
#include <QStringList>

#include "libphoenixplayer_global.h"
#include "DataProvider/IDataProvider.h"

namespace PhoenixPlayer {
    class AudioMetaObject;

    namespace DataProvider {

class IMusicTagParser : public IDataProvider
{
    Q_OBJECT
public:
    explicit IMusicTagParser(QObject *parent = Q_NULLPTR);
    virtual ~IMusicTagParser() override
    {
        m_list.clear ();
    }

    /*!
     * \brief parserTag 解析文件tag
     * \param target targetMetaDate
     * \return 成功返回true
     */
    virtual bool parserTag(AudioMetaObject *target) = 0;

    // IDataProvider interface
public:
    virtual bool support(SupportedTypes type) const Q_DECL_OVERRIDE
    {
        return (type & SupportedType::SupportMusicTagParser) == SupportedType::SupportMusicTagParser;
    }

    // BasePlugin interface
public:
    virtual PluginProperty property() const Q_DECL_OVERRIDE
    {
        return PluginProperty();
    }
    virtual PluginType type() const Q_DECL_OVERRIDE
    {
        return BasePlugin::PluginDataProvider;
    }

protected:
    inline QString indexToGenre(const int &index) {
        return (index > 0 && index < m_list.size ())
                ? m_list.at (index)
                : "Unknown";
    }

private:
    QStringList m_list;
};
} //DataProvider
} //PhoenixPlayer

#endif // IMUSICTAGPARSER

