#include "IniFile.hpp"

using namespace MellowPlayer::Infrastructure;

IniFile::IniFile(const QString& path): iniFile_(path, QSettings::IniFormat)
{

}

QVariant IniFile::value(const QString& key, const QVariant& defaultValue) const
{
    return iniFile_.value(key, defaultValue);
}
