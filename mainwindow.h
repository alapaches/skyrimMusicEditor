#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QSignalMapper>
#include <QMediaPlayer>
#include <QListWidget>
#include <QIcon>
#include <QTranslator>
#include <QJsonObject>
#include <functional>
#include <QLabel>
#include <QTableWidget>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // --- Signal mappers (catégories musique) ---
    QSignalMapper ButtonsBardsDragonborn;
    QSignalMapper ButtonsBardsOppression;
    QSignalMapper ButtonsBardsRagnar;
    QSignalMapper ButtonsBardsTongues;
    QSignalMapper ButtonsBardsViolence;

    // --- État courant sélection genre/race ---
    QString gender;
    QString genderBards;
    QString race;
    QString raceBards;

    // --- Constantes de nommage fichiers ---
    QString fileEndWithExtension = "_1.fuz";
    QString fileEndWithoutExtension = "_1";
    QString firstWordV1File;
    QString firstWordV2File;
    QString secondWordFile;
    QString secondAndThirdWordFile;
    QString bardDebutFile = "bardsongs__000";
    QString language = "en";

    // --- Lecteur audio ---
    // CHANGEMENT : plus de "new" sans parent. Construit dans le corps du
    // constructeur avec `this` en parent -> Qt gère la destruction automatiquement.
    QMediaPlayer *player = nullptr;

    // --- Détection installation Skyrim ---
    // CHANGEMENT : les QSettings ne sont plus construits en initialiseur de membre
    // (ça exécutait une lecture de registre Windows à chaque construction de
    // MainWindow, avant même le corps du constructeur). Tout est maintenant
    // regroupé dans DetectSkyrimInstallation(), appelée explicitement depuis
    // le constructeur. Prépare aussi le terrain pour rendre ça lazy plus tard
    // si besoin (ex: ne détecter qu'au premier accès).
    QSettings *registryVanilla = nullptr;
    QSettings *registrySpecialEdition = nullptr;
    QString skyrimVanillaDir;
    QString skyrimSpecialEditionDir;
    QString skyrimVanillaRealPath;
    QString skyrimSpecialEditionRealPath;
    QString skyrimPath;
    QString skyrimDataPath;
    QString skyrimMusicRealPath;
    QString skyrimShoutsRealPath;
    QString skyrimVersion;

public slots:
    void CombatButtonClicked(int RowNum, QString category = "combat");
    void DungeonButtonClicked(int RowNum, QString category = "dungeon");
    void DreadButtonClicked(int RowNum, QString category = "dread");
    void ExploreButtonClicked(int RowNum, QString category = "explore");
    void OtherButtonClicked(int RowNum, QString category = "other");
    void RewardButtonClicked(int RowNum, QString category = "reward");
    void RevealButtonClicked(int RowNum, QString category = "reveal");
    void StingerButtonClicked(int RowNum, QString category = "stinger");
    void SpecialButtonClicked(int RowNum, QString category = "special");
    void TavernButtonClicked(int RowNum, QString category = "tavern");
    void TownButtonClicked(int RowNum, QString category = "town");
    void DawnguardButtonClicked(int RowNum, QString category = "dlc01");
    void DragonbornButtonClicked(int RowNum, QString category = "dragonborn");

    void CombatPlayButtonClicked(int RowNum);
    void DungeonPlayButtonClicked(int RowNum);
    void DreadPlayButtonClicked(int RowNum);
    void ExplorePlayButtonClicked(int RowNum);
    void OtherPlayButtonClicked(int RowNum);
    void RewardPlayButtonClicked(int RowNum);
    void RevealPlayButtonClicked(int RowNum);
    void StingerPlayButtonClicked(int RowNum);
    void SpecialPlayButtonClicked(int RowNum);
    void TavernPlayButtonClicked(int RowNum);
    void TownPlayButtonClicked(int RowNum);
    void DawnguardPlayButtonClicked(int RowNum);
    void DragonbornPlayButtonClicked(int RowNum);

    void CombatPlayCurrentButtonClicked(int RowNum);
    void DungeonPlayCurrentButtonClicked(int RowNum);
    void DreadPlayCurrentButtonClicked(int RowNum);
    void ExplorePlayCurrentButtonClicked(int RowNum);
    void OtherPlayCurrentButtonClicked(int RowNum);
    void RewardPlayCurrentButtonClicked(int RowNum);
    void RevealPlayCurrentButtonClicked(int RowNum);
    void StingerPlayCurrentButtonClicked(int RowNum);
    void SpecialPlayCurrentButtonClicked(int RowNum);
    void TavernPlayCurrentButtonClicked(int RowNum);
    void TownPlayCurrentButtonClicked(int RowNum);
    void DawnguardPlayCurrentButtonClicked(int RowNum);
    void DragonbornPlayCurrentButtonClicked(int RowNum);

    void ProcessFile(QString type, QString reference);
    void ProcessFileBards();
    void RestoreOriginal(QString type);
    void Delay(int seconds);
    void RestoreBards();
    void ShowLyrics();
    QString BoolToString(bool e);

    // NOUVEAU (étape 2) : slot unique remplaçant les ~25 lambdas dupliquées
    // connectées à mediaStatusChanged dans chaque fonction PlayButtonClicked /
    // PlayCurrentButtonClicked. Voir mainwindow.cpp.
    void onPlayerStatusChanged(QMediaPlayer::MediaStatus status);

private slots:
    void on_btnClearCombat_clicked();
    void on_btnClearDungeon_clicked();
    void on_btnClearExplore_clicked();
    void on_btnClearOther_clicked();
    void on_btnClearSpecial_clicked();
    void on_btnClearTavern_clicked();
    void on_btnClearTown_clicked();
    void on_btnClearDawnguard_clicked();
    void on_btnClearDragonborn_clicked();
    void on_stopMusicButton_clicked();
    void on_pushButton_clicked();
    void on_btnClearReveal_clicked();
    void on_btnClearReward_clicked();
    void on_btnClearDread_clicked();
    void on_changePathButton_clicked();
    void on_restoreMusicPathButton_clicked();
    void on_maleBardRadioButton_toggled(bool checked);
    void on_femaleBardRadioButton_toggled(bool checked);
    void on_resetBardsBtn_clicked();
    void on_btnClearStinger_clicked();
    void on_actionFrench_triggered();
    void on_actionRussian_triggered();
    void on_actionEnglish_triggered();

private:
    // NOUVEAU (étape 1) : regroupe toute la détection d'installation Skyrim
    // (lecture registre + résolution des chemins) dans une méthode dédiée,
    // appelée une fois depuis le constructeur, au lieu de s'exécuter comme
    // effet de bord d'initialiseurs de membres dans le header.
    void DetectSkyrimInstallation();
    void RunProcessChain(QList<QPair<QString, QStringList>> steps, std::function<void()> onComplete);
    void ConfirmOrChangePath(QString detectedPath, const QString &versionName, std::function<void(QString)> onPathReady);
    void SetupSidebar();
    void SetupShoutsScreen();
    void RefreshShoutsTable();
    void ReplaceShoutFor(const QString &shoutName, const QString &subDir, const QString &jsonKey);
    void RefreshMusicStatus();
    int  CountReplacedWords(const QString &subDir, const QString &jsonKey) const;
    struct ShoutEntry {
        QString name;
        QString subDir;
        QString jsonKey;
    };
    void SetupBardsScreen();
    void RefreshBardsTable();
    void ReplaceBardSongFor(const QString &songKey);
    void ShowLyricsFor(const QString &lyricsKey, const QString &displayName);
    int  CountReplacedBardFiles(const QString &songKey, int fileCount) const;
    static int BardFileCount(const QString &songKey);
    struct BardEntry {
        QString name;
        QString songKey;
        QString restoreKey;
        QString lyricsKey;
        int     fileCount;
    };
    void UpdateStatusBar();
    void RetranslateCustomUi();



    Ui::MainWindow *ui;
    QString version = "Skyrim Music Editor version 3.0";
    QIcon playIcon;
    QIcon playCurrentIcon;
    QIcon stopIcon;
    QIcon browseIcon;
    QIcon launchGameIcon;
    QTranslator translator;
    QList<QPair<QString, QStringList>> BuildWavXwmSteps(QString fileToEncode, QJsonObject item, QJsonObject fileItem, QString index);
    QPair<QString, QStringList> BuildFuzStep(QJsonObject item, QJsonObject fileItem, QString index);
    QList<QPair<QString, QStringList>> BuildWavXwmBardsSteps(QString fileToEncode, QJsonObject item, QString index);
    QPair<QString, QStringList> BuildFuzBardsStep(QJsonObject item, QString index);
    QListWidget *categoryList = nullptr;
    QLabel *categoryTitle = nullptr;
    QLabel *categorySubtitle = nullptr;
    QList<ShoutEntry> shoutEntries;
    QTableWidget *shoutsTable = nullptr;
    QComboBox *genderCombo = nullptr;
    QComboBox *raceCombo = nullptr;
    QWidget *voiceSelectors = nullptr;
    QString ResolveMusicFilePath(const QString &category, const QString &fileName) const;
    QList<BardEntry> bardEntries;
    QTableWidget *bardsTable = nullptr;
    QComboBox *bardGenderCombo = nullptr;
    QWidget *bardSelectors = nullptr;

};
#endif // MAINWINDOW_H
