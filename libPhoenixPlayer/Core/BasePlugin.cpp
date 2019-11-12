#include "BasePlugin.h"


namespace PhoenixPlayer {



PluginProperty::PluginProperty()
{
    name = "Base plugin interface";
    description = "Base plugin interface";
    version = "1.0";
    hasAbout = false;
    hasSettings = false;
}

PluginProperty::PluginProperty(const PluginProperty &other)
{
    this->name = other.name;
    this->description = other.description;
    this->version = other.version;
    this->hasAbout = other.hasAbout;
    this->hasSettings = other.hasSettings;
}

PluginProperty::PluginProperty(const QString &name, const QString &version, const QString &description, bool hasAbout, bool hasSettings)
{
    this->name = name;
    this->description = description;
    this->version = version;
    this->hasAbout = hasAbout;
    this->hasSettings = hasSettings;
}

PluginProperty &PluginProperty::operator =(const PluginProperty &other)
{
    this->name = other.name;
    this->description = other.description;
    this->version = other.version;
    this->hasAbout = other.hasAbout;
    this->hasSettings = other.hasSettings;
    return *this;
}




BasePlugin::BasePlugin(QObject *parent)
    : BaseObject(parent)
{

}

BasePlugin::~BasePlugin()
{

}

} //PhoenixPlayer
