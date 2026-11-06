#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QIcon>
#include <QTranslator>
#include <QJsonObject>
#include <QListWidget>
#include <QLabel>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <functional>

#include "skyriminstall.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Connectes automatiquement par nom depuis mainwindow.ui
    void on_stopMusicButton_clicked();
    void on_pushButton_clicked();
    void on_launchSkyrimButton_clicked();
    void on_actionFrench_triggered();
    void on_actionRussian_triggered();
    void on_actionEnglish_triggered();

    void onPlayerStatusChanged(QMediaPlayer::MediaStatus status);

private:
    // --- Une categorie de musique = une page avec sa table ---
    struct MusicCategory {
        QString key;              // nom du dossier dans music/ (ex. "dlc01")
        QString label;            // libelle de la sidebar
        QWidget *page;
        QTableWidget *table;
        QPushButton *restoreBtn;
    };

    struct ShoutEntry {
        QString name;
        QString subDir;
        QString jsonKey;
    };

    struct BardEntry {
        QString name;
        QString songKey;
        QString lyricsKey;
        int     fileCount;
    };

    // --- Installation du jeu ---
    void SelectInstallation();
    QString PromptForGameDir(const QString &versionName, const QString &startDir = QString());
    bool EnsureInstallation();
    void UpdateStatusBar();

    // --- Construction de l'interface ---
    void SetupMusicPages();
    void SetupSidebar();
    void SetupShoutsScreen();
    void SetupBardsScreen();
    void RetranslateCustomUi();
    void SetLanguage(const QString &lang);

    // --- Musique (generique pour toutes les categories) ---
    QString MusicFilePath(const MusicCategory &cat, int row) const;
    void ReplaceTrack(const MusicCategory &cat, int row);
    void PlayOriginal(const MusicCategory &cat, int row);
    void PlayCurrent(const MusicCategory &cat, int row);
    void RestoreCategory(const MusicCategory &cat);
    void RefreshMusicStatus();
    void UpdateCategorySubtitle();

    // --- Cris ---
    void RefreshShoutsTable();
    void ReplaceShoutFor(const ShoutEntry &shout);
    QStringList ShoutFileNames(const ShoutEntry &shout) const;
    QString ShoutFolder(const QString &subDir) const;

    // --- Chants des bardes ---
    void RefreshBardsTable();
    void ReplaceBardSongFor(const BardEntry &song);
    void ShowLyricsFor(const QString &lyricsKey, const QString &displayName);
    QStringList BardFileNames(const BardEntry &song) const;
    QString BardFolder() const;
    static int BardFileCount(const QString &songKey);

    // --- Voix (cris et bardes) : encodage puis copie des .fuz ---
    void EncodeVoiceFiles(const QStringList &sources, const QStringList &fuzNames,
                          const QString &destFolder, const QString &title,
                          const QString &successText, std::function<void()> onDone);

    // --- Utilitaires ---
    static QJsonObject LoadJson(const QString &fileName);
    void StopPlayback();
    void PlayFile(const QString &path);
    void ShowError(const QString &title, const QString &message);

    Ui::MainWindow *ui;
    const QString version = "Skyrim Music Editor version 3.2";
    QString language = "en";
    QTranslator translator;
    QMediaPlayer *player = nullptr;
    SkyrimInstall install;

    QIcon playIcon;
    QIcon playCurrentIcon;
    QIcon stopIcon;
    QIcon browseIcon;
    QIcon launchGameIcon;

    QList<MusicCategory> musicCategories;
    QListWidget *categoryList = nullptr;
    QLabel *categoryTitle = nullptr;
    QLabel *categorySubtitle = nullptr;

    // Cris
    QList<ShoutEntry> shoutEntries;
    QTableWidget *shoutsTable = nullptr;
    QComboBox *genderCombo = nullptr;
    QComboBox *raceCombo = nullptr;
    QWidget *voiceSelectors = nullptr;
    QString gender;
    QString race;

    // Bardes
    QList<BardEntry> bardEntries;
    QTableWidget *bardsTable = nullptr;
    QComboBox *bardGenderCombo = nullptr;
    QWidget *bardSelectors = nullptr;
    QString genderBards;
    QString raceBards;
};
#endif // MAINWINDOW_H
