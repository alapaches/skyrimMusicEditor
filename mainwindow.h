#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QDialog>
#include <QFileSystemModel>
#include <QSettings>
#include <QDebug>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QSignalMapper>
#include <QObject>
#include <QtWidgets>
#include <QDir>
#include <QDirIterator>
#include <iostream>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSignalMapper ButtonSignalMapperCombat;
    QSignalMapper ButtonSignalMapperDungeon;
    QSignalMapper ButtonSignalMapperDread;
    QSignalMapper ButtonSignalMapperExplore;
    QSignalMapper ButtonSignalMapperOther;
    QSignalMapper ButtonSignalMapperReward;
    QSignalMapper ButtonSignalMapperReveal;
    QSignalMapper ButtonSignalMapperSpecial;
    QSignalMapper ButtonSignalMapperStinger;
    QSignalMapper ButtonSignalMapperTavern;
    QSignalMapper ButtonSignalMapperTown;
    QSignalMapper ButtonSignalMapperDawnguard;
    QSignalMapper ButtonSignalMapperDragonborn;
    QSignalMapper ButtonSignalMapperAudioCombat;
    QSignalMapper ButtonSignalMapperAudioDungeon;
    QSignalMapper ButtonSignalMapperAudioDread;
    QSignalMapper ButtonSignalMapperAudioExplore;
    QSignalMapper ButtonSignalMapperAudioOther;
    QSignalMapper ButtonSignalMapperAudioReward;
    QSignalMapper ButtonSignalMapperAudioReveal;
    QSignalMapper ButtonSignalMapperAudioSpecial;
    QSignalMapper ButtonSignalMapperAudioStinger;
    QSignalMapper ButtonSignalMapperAudioTavern;
    QSignalMapper ButtonSignalMapperAudioTown;
    QSignalMapper ButtonSignalMapperAudioDawnguard;
    QSignalMapper ButtonSignalMapperAudioDragonborn;
    QSignalMapper ButtonsBardsDragonborn;
    QSignalMapper ButtonsBardsOppression;
    QSignalMapper ButtonsBardsRagnar;
    QSignalMapper ButtonsBardsTongues;
    QSignalMapper ButtonsBardsViolence;

    QString gender;
    QString genderBards;
    QString race;
    QString raceBards;
    QString fileEndWithExtension = "_1.fuz";
    QString fileEndWithoutExtension = "_1";
    QString firstWordV1File;
    QString firstWordV2File;
    QString secondWordFile;
    QString secondAndThirdWordFile;
    QString bardDebutFile = "bardsongs__000";
    QString language = "en";

    QMediaPlayer *player = new QMediaPlayer;

    QSettings *registry = new QSettings(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Bethesda Softworks\Skyrim)", QSettings::NativeFormat);
    QString skyrimDir = registry->value("Installed Path").toString();
    QString skyrimRealPath = skyrimDir.replace("\\","/");
    QString skyrimDataPath = skyrimRealPath + "Data/";
    QString skyrimMusicRealPath = skyrimRealPath + "Data/music/";
    QString skyrimShoutsRealPath = skyrimRealPath + "Data/sound/voice/";

public slots:
    void CombatButtonClicked(int RowNum, QString category = "combat");
    void CombatPlayButtonClicked(int RowNum);
    void DungeonButtonClicked(int RowNum, QString category = "dungeon");
    void DungeonPlayButtonClicked(int RowNum);
    void DreadButtonClicked(int RowNum, QString category = "dread");
    void DreadPlayButtonClicked(int RowNum);
    void ExploreButtonClicked(int RowNum, QString category = "explore");
    void ExplorePlayButtonClicked(int RowNum);
    void OtherButtonClicked(int RowNum, QString category = "other");
    void OtherPlayButtonClicked(int RowNum);
    void RewardButtonClicked(int RowNum, QString category = "reward");
    void RewardPlayButtonClicked(int RowNum);
    void RevealButtonClicked(int RowNum, QString category = "reveal");
    void RevealPlayButtonClicked(int RowNum);
    void StingerButtonClicked(int RowNum, QString category = "stinger");
    void StingerPlayButtonClicked(int RowNum);
    void SpecialButtonClicked(int RowNum, QString category = "special");
    void SpecialPlayButtonClicked(int RowNum);
    void TavernButtonClicked(int RowNum, QString category = "tavern");
    void TavernPlayButtonClicked(int RowNum);
    void TownButtonClicked(int RowNum, QString category = "town");
    void TownPlayButtonClicked(int RowNum);
    void DawnguardButtonClicked(int RowNum, QString category = "dlc01");
    void DawnguardPlayButtonClicked(int RowNum);
    void DragonbornButtonClicked(int RowNum, QString category = "dragonborn");
    void DragonbornPlayButtonClicked(int RowNum);
    void ProcessFile(QString type, QString reference);
    void ProcessFileBards();
    void RestoreOriginal(QString type);
    void Help(QString folder);
    void ReplaceShout();
    void WavEncoder(QString file, QJsonObject item, QJsonObject fileItem, QString index);
    void XwmEncoder(QString file, QJsonObject item, QJsonObject fileItem, QString index);
    void FuzEncoder(QJsonObject item, QJsonObject fileItem, QString index);
    void WavBardsEncoder(QString file, QJsonObject item, QString index);
    void XwmBardsEncoder(QString file, QJsonObject item, QString index);
    void FuzBardsEncoder(QJsonObject item, QString index);
    void Delay(int seconds);
    void RestoreShouts();
    void RestoreBards();
    void ShowLyrics();
    QString BoolToString(bool e);

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

    void on_setRaceBtn_clicked();

    void on_pushButton_2_clicked();

    void on_RestoreShoutsBtn_clicked();

    void on_maleBardRadioButton_toggled(bool checked);

    void on_femaleBardRadioButton_toggled(bool checked);

    void on_resetBardsBtn_clicked();

    void on_btnClearStinger_clicked();

    void on_actionFrench_triggered();

    void on_actionRussian_triggered();

    void on_actionEnglish_triggered();

private:
    Ui::MainWindow *ui;
    QString version = "Skyrim Music Editor version 1.0.4";
    QIcon playIcon = QIcon();
    QIcon stopIcon = QIcon();
    QIcon browseIcon = QIcon();
    QTranslator translator;
};
#endif // MAINWINDOW_H
