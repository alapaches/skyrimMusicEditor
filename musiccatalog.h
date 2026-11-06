#ifndef MUSICCATALOG_H
#define MUSICCATALOG_H

#include <QString>

// Regles de nommage des fichiers musicaux, au meme endroit pour
// le remplacement, la lecture, la restauration et la detection de statut.
namespace MusicCatalog {

// Chemin relatif au dossier music/ du jeu, ex. "dungeon/ice/mus_dungeon_ice_01.xwm".
// `fileName` est le nom technique sans extension (colonne cachee des tables).
QString gameRelativePath(const QString &category, const QString &fileName);

// Chemin relatif au dossier audio/ de l'application pour l'apercu de l'original.
// `displayName` est le titre affiche (colonne 0 des tables).
QString previewRelativePath(const QString &category, const QString &displayName);

} // namespace MusicCatalog

#endif // MUSICCATALOG_H
