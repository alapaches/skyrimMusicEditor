#include "audioconverter.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <memory>

namespace AudioConverter {

namespace {

QString ffmpegPath()       { return toolsDir() + "/ffmpeg.exe"; }
QString xwmEncoderPath()   { return toolsDir() + "/encode.exe"; }
QString fuzExtractorPath() { return toolsDir() + "/fuz_extractor.exe"; }
QString lipFilePath()      { return toolsDir() + "/lipfile.lip"; }

QString describe(const Step &step)
{
    return QFileInfo(step.program).fileName();
}

QString tail(const QByteArray &output, int maxLines = 6)
{
    QStringList lines = QString::fromLocal8Bit(output).trimmed().split('\n');
    if (lines.size() > maxLines) {
        lines = lines.mid(lines.size() - maxLines);
    }
    return lines.join('\n').trimmed();
}

} // namespace

QString toolsDir()
{
    return QCoreApplication::applicationDirPath();
}

QString workDir()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                  + "/SkyrimMusicEditor";
    QDir().mkpath(dir);
    return dir;
}

QString workFile(const QString &fileName)
{
    return workDir() + "/" + fileName;
}

bool toolsAvailable(QString *missing)
{
    const QStringList required = { ffmpegPath(), xwmEncoderPath(), fuzExtractorPath(), lipFilePath() };
    QStringList absent;
    for (const QString &path : required) {
        if (!QFile::exists(path)) absent << QFileInfo(path).fileName();
    }
    if (missing) *missing = absent.join(", ");
    return absent.isEmpty();
}

Step toWav(const QString &input, const QString &outputWav)
{
    return { ffmpegPath(),
             { "-y", "-loglevel", "error", "-i", input,
               "-ar", "44100", "-c:a", "pcm_s16le", outputWav },
             outputWav };
}

Step wavToXwm(const QString &inputWav, const QString &outputXwm)
{
    return { xwmEncoderPath(), { inputWav, outputXwm }, outputXwm };
}

Step xwmToFuz(const QString &inputXwm, const QString &outputFuz)
{
    return { fuzExtractorPath(), { "-c", outputFuz, inputXwm, lipFilePath() }, outputFuz };
}

Step xwmToWav(const QString &inputXwm, const QString &outputWav)
{
    return { ffmpegPath(), { "-y", "-loglevel", "error", "-i", inputXwm, outputWav }, outputWav };
}

void run(QObject *context, Steps steps, SuccessCallback onSuccess, ErrorCallback onError)
{
    if (steps.isEmpty()) {
        if (onSuccess) onSuccess();
        return;
    }

    const Step step = steps.takeFirst();

    // Un fichier de sortie laisse par une execution precedente fausserait
    // la verification "le fichier attendu existe".
    if (!step.expectedOutput.isEmpty()) {
        QFile::remove(step.expectedOutput);
    }

    auto *process = new QProcess(context);
    process->setProcessChannelMode(QProcess::MergedChannels);
    auto handled = std::make_shared<bool>(false);

    auto finish = [=](bool ok, const QString &error) {
        if (*handled) return;
        *handled = true;
        const QByteArray output = process->readAll();
        process->deleteLater();

        if (!ok) {
            QString message = error;
            const QString details = tail(output);
            if (!details.isEmpty()) message += "\n\n" + details;
            if (onError) onError(message);
            return;
        }
        run(context, steps, onSuccess, onError);
    };

    QObject::connect(process, &QProcess::finished, context,
                     [=](int exitCode, QProcess::ExitStatus status) {
        if (status != QProcess::NormalExit) {
            finish(false, QCoreApplication::translate("AudioConverter", "%1 crashed.").arg(describe(step)));
        } else if (exitCode != 0) {
            finish(false, QCoreApplication::translate("AudioConverter", "%1 failed (exit code %2).")
                              .arg(describe(step)).arg(exitCode));
        } else if (!step.expectedOutput.isEmpty() && !QFile::exists(step.expectedOutput)) {
            finish(false, QCoreApplication::translate("AudioConverter", "%1 did not produce %2.")
                              .arg(describe(step), QFileInfo(step.expectedOutput).fileName()));
        } else {
            finish(true, QString());
        }
    });
    QObject::connect(process, &QProcess::errorOccurred, context,
                     [=](QProcess::ProcessError error) {
        // Les plantages sont deja signales par finished().
        if (error == QProcess::FailedToStart) {
            finish(false, QCoreApplication::translate("AudioConverter", "Could not start %1.\n%2")
                              .arg(describe(step), step.program));
        }
    });

    process->start(step.program, step.arguments);
}

bool replaceFile(const QString &src, const QString &dest)
{
    QDir().mkpath(QFileInfo(dest).absolutePath());
    if (QFile::exists(dest) && !QFile::remove(dest)) {
        return false;
    }
    return QFile::copy(src, dest);
}

} // namespace AudioConverter
