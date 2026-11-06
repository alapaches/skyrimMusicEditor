#ifndef SKYRIMINSTALL_H
#define SKYRIMINSTALL_H

#include <QString>

// Emplacement et version de l'installation Skyrim utilisee par l'application.
// Regroupe la detection (registre Windows) et la construction des chemins
// (Data/, music/, sound/voice/) qui etaient auparavant dupliquees dans MainWindow.
class SkyrimInstall
{
public:
    enum class Edition { None, Vanilla, SpecialEdition };

    // Chemins lus dans le registre, ou chaine vide si absents / invalides.
    static QString detectFromRegistry(Edition edition);

    // Un dossier est accepte s'il contient un sous-dossier Data.
    static bool isValidGameDir(const QString &dir);

    static QString editionName(Edition edition);

    void set(const QString &rootDir, Edition edition);
    void clear();

    bool isValid() const { return m_edition != Edition::None && isValidGameDir(m_root); }
    Edition edition() const { return m_edition; }
    QString root() const { return m_root; }
    QString dataPath() const;    // ".../Data/"
    QString musicPath() const;   // ".../Data/Music/" (vanilla) ou ".../Data/music/" (SE)
    QString voicePath() const;   // ".../Data/sound/voice/"

private:
    QString m_root;
    Edition m_edition = Edition::None;
};

#endif // SKYRIMINSTALL_H
