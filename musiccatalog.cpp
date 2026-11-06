#include "musiccatalog.h"

namespace MusicCatalog {

QString gameRelativePath(const QString &category, const QString &fileName)
{
    const QString file = fileName + ".xwm";

    // Ces pistes sont directement a la racine de music/.
    if (category == "other" || category == "dragonborn") {
        return file;
    }
    if (category == "dungeon") {
        const QString sub = fileName.mid(12, 2);
        if (sub == "ic") return category + "/ice/"  + file;
        if (sub == "ca") return category + "/cave/" + file;
        if (sub == "fo") return category + "/fort/" + file;
    }
    if (category == "special" && fileName.mid(4, 5) == "failu") {
        return category + "/failure/" + file;
    }
    if (category == "dlc01") {
        const QString sub = fileName.mid(4, 7);
        if (sub == "dungeon") return category + "/dungeon/" + file;
        if (sub == "explore") return category + "/explore/falmervalley/" + file;
        if (sub == "soulcai") return category + "/explore/soulcairn/palette/" + file;
    }
    return category + "/" + file;
}

QString previewRelativePath(const QString &category, const QString &displayName)
{
    const QString file = displayName + ".mp3";

    if (category == "dungeon" && displayName.startsWith("Bene")) {
        return "dungeon/Beneath the Ice.mp3";
    }
    if ((category == "explore" || category == "town") && displayName.startsWith("From")) {
        return "explore/From Past to Present.mp3";
    }
    if (category == "special" && displayName.startsWith("Failure")) {
        return "special/failure/" + file;
    }
    if (category == "dlc01") {
        if (displayName.startsWith("The"))  return "dlc01/" + file;
        if (displayName.startsWith("Cast") || displayName.startsWith("Soul"))
            return "dlc01/dungeon/" + file;
        if (displayName.startsWith("Forg")) return "dlc01/explore/falmervalley/" + file;
        return "dlc01/explore/soulcairn/palette/" + file;
    }
    return category + "/" + file;
}

} // namespace MusicCatalog
