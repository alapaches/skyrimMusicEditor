#include "skyriminstall.h"

#include <QDir>
#include <QSettings>

QString SkyrimInstall::detectFromRegistry(Edition edition)
{
    QString key;
    switch (edition) {
    case Edition::Vanilla:
        key = R"(HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Bethesda Softworks\Skyrim)";
        break;
    case Edition::SpecialEdition:
        key = R"(HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Bethesda Softworks\Skyrim Special Edition)";
        break;
    case Edition::None:
        return QString();
    }

    QSettings registry(key, QSettings::NativeFormat);
    QString path = QDir::fromNativeSeparators(registry.value("Installed Path").toString());
    while (path.endsWith('/')) path.chop(1);

    // Une cle de registre peut survivre a une desinstallation : on verifie.
    return isValidGameDir(path) ? path : QString();
}

bool SkyrimInstall::isValidGameDir(const QString &dir)
{
    return !dir.isEmpty() && QDir(dir + "/Data").exists();
}

QString SkyrimInstall::editionName(Edition edition)
{
    switch (edition) {
    case Edition::Vanilla:        return QStringLiteral("Skyrim");
    case Edition::SpecialEdition: return QStringLiteral("Skyrim Special Edition");
    case Edition::None:           break;
    }
    return QString();
}

void SkyrimInstall::set(const QString &rootDir, Edition edition)
{
    m_root = QDir::fromNativeSeparators(rootDir);
    while (m_root.endsWith('/')) m_root.chop(1);
    m_edition = edition;
}

void SkyrimInstall::clear()
{
    m_root.clear();
    m_edition = Edition::None;
}

QString SkyrimInstall::dataPath() const
{
    return m_root + "/Data/";
}

QString SkyrimInstall::musicPath() const
{
    return dataPath() + (m_edition == Edition::Vanilla ? "Music/" : "music/");
}

QString SkyrimInstall::voicePath() const
{
    return dataPath() + "sound/voice/";
}
