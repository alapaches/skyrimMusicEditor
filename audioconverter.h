#ifndef AUDIOCONVERTER_H
#define AUDIOCONVERTER_H

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <functional>

// Encapsule les outils externes (ffmpeg, xWMAEncode, fuz_extractor) et
// l'execution sequentielle et asynchrone des conversions.
//
// Les outils et donnees en lecture seule restent a cote de l'executable,
// mais tous les fichiers intermediaires sont ecrits dans un dossier temporaire
// de l'utilisateur : l'application fonctionne donc aussi une fois installee
// dans un dossier en lecture seule (Program Files).
namespace AudioConverter {

struct Step {
    QString program;
    QStringList arguments;
    QString expectedOutput;   // fichier qui doit exister apres l'etape (optionnel)
};

using Steps = QList<Step>;
using SuccessCallback = std::function<void()>;
using ErrorCallback = std::function<void(const QString &message)>;

QString toolsDir();                 // dossier de l'executable
QString workDir();                  // dossier temporaire (cree si besoin)
QString workFile(const QString &fileName);
bool    toolsAvailable(QString *missing = nullptr);

// Normalise n'importe quelle source audio en WAV PCM 16 bits 44,1 kHz,
// format accepte par xWMAEncode.
Step toWav(const QString &input, const QString &outputWav);
Step wavToXwm(const QString &inputWav, const QString &outputXwm);
Step xwmToFuz(const QString &inputXwm, const QString &outputFuz);
// Decode un .xwm du jeu en WAV lisible par QMediaPlayer.
Step xwmToWav(const QString &inputXwm, const QString &outputWav);

// Execute les etapes l'une apres l'autre. S'arrete a la premiere erreur
// (programme introuvable, code de sortie non nul, fichier attendu absent).
void run(QObject *context, Steps steps, SuccessCallback onSuccess, ErrorCallback onError);

// Remplace dest par src (QFile::copy refuse d'ecraser un fichier existant).
bool replaceFile(const QString &src, const QString &dest);

} // namespace AudioConverter

#endif // AUDIOCONVERTER_H
