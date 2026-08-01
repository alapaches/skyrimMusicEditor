#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "trackdelegate.h"
#include "shoutrplacedialog.h"
#include "json.hpp"
#include <QFileSystemModel>
#include <QDebug>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QtMultimedia/QAudioOutput>
#include <QMessageBox>
#include <QProcess>
#include <QAbstractButton>
#include <QButtonGroup>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QTime>
#include <QJsonDocument>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QLabel>
#include <QGridLayout>
#include <QComboBox>
#include <QTableWidget>
#include <QGridLayout>

using json = nlohmann::json;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    player = new QMediaPlayer(this);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onPlayerStatusChanged);
    DetectSkyrimInstallation();
    ui->setupUi(this);
    playIcon.addPixmap(QPixmap(":/images/images/ico/play.ico"));
    launchGameIcon.addPixmap(QPixmap(":/images/images/ico/mainIcon.ico"));
    playCurrentIcon.addPixmap(QPixmap(":/images/images/ico/play_current.ico"));
    stopIcon.addPixmap(QPixmap(":/images/images/ico/stop.ico"));
    browseIcon.addPixmap(QPixmap(":/images/images/ico/folder.ico"));
    ui->launchSkyrimButton->setIcon(launchGameIcon);
    QIcon folderIcon;
    folderIcon.addPixmap(QPixmap(":/images/images/ico/folder.ico"));
    ui->stopMusicButton->setIcon(stopIcon);
    ui->pushButton->setIcon(folderIcon);
    SetupSidebar();
    // --- 1. Layouts : une page d'onglet = une table qui s'etire ---
    struct PageSetup { QWidget *page; QTableWidget *table; QPushButton *restoreBtn; };
    QList<PageSetup> pages = {
                              {ui->tab_combat,     ui->combatTableWidget,     ui->btnClearCombat},
                              {ui->tab_dungeon,    ui->dungeonTableWidget,    ui->btnClearDungeon},
                              {ui->tab_dread,      ui->dreadTableWidget,      ui->btnClearDread},
                              {ui->tab_explore,    ui->exploreTableWidget,    ui->btnClearExplore},
                              {ui->tab_other,      ui->otherTableWidget,      ui->btnClearOther},
                              {ui->tab_reveal,     ui->revealTableWidget,     ui->btnClearReveal},
                              {ui->tab_reward,     ui->rewardTableWidget,     ui->btnClearReward},
                              {ui->tab_special,    ui->specialTableWidget,    ui->btnClearSpecial},
                              {ui->tab_stinger,    ui->stingerTableWidget,    ui->btnClearStinger},
                              {ui->tab_tavern,     ui->tavernTableWidget,     ui->btnClearTavern},
                              {ui->tab_town,       ui->townTableWidget,       ui->btnClearTown},
                              {ui->tab_dawnguard,  ui->dawnguardTableWidget,  ui->btnClearDawnguard},
                              {ui->tab_dragonborn, ui->dragonbornTableWidget, ui->btnClearDragonborn},
                              };

    for (const PageSetup &p : pages) {
        auto *pageLayout = new QVBoxLayout(p.page);
        pageLayout->setContentsMargins(12, 0, 12, 12);
        pageLayout->setSpacing(10);
        pageLayout->addWidget(p.table);
        if (p.restoreBtn) {
            auto *btnRow = new QHBoxLayout();
            btnRow->addStretch();
            btnRow->addWidget(p.restoreBtn);
            pageLayout->addLayout(btnRow);
        }
    }

    auto *musicTabLayout = new QVBoxLayout(ui->musicTab);
    musicTabLayout->setContentsMargins(0, 0, 0, 0);
    musicTabLayout->addWidget(ui->musicTabWidget);

    auto *voiceTabLayout = new QVBoxLayout(ui->voiceTab);
    voiceTabLayout->setContentsMargins(0, 0, 0, 0);
    voiceTabLayout->addWidget(ui->voiceTabWidget);

    ui->shoutTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->shoutGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    SetupShoutsScreen();
    SetupBardsScreen();

    // --- 2. Bandeau d'en-tete : titre a gauche, actions a droite ---
    auto *headerWidget = new QWidget(this);
    headerWidget->setObjectName("headerBar");
    headerWidget->setFixedHeight(62);

    auto *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 10, 20, 10);

    auto *titleBlock = new QVBoxLayout();
    titleBlock->setSpacing(1);
    titleBlock->addWidget(categoryTitle);
    titleBlock->addWidget(categorySubtitle);
    headerLayout->addLayout(titleBlock);
    headerLayout->addStretch();
    headerLayout->addWidget(voiceSelectors);
    headerLayout->addWidget(bardSelectors);
    headerLayout->addWidget(ui->stopMusicButton);
    headerLayout->addWidget(ui->pushButton);
    headerLayout->addWidget(ui->launchSkyrimButton);

    // --- 3. Assemblage : sidebar | (header + contenu) ---
    auto *rightSide = new QVBoxLayout();
    rightSide->setContentsMargins(0, 0, 0, 0);
    rightSide->setSpacing(0);
    rightSide->addWidget(headerWidget);
    rightSide->addWidget(ui->tabWidget, 1);

    ui->tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->musicTabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->voiceTabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->musicTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->voiceTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    for (const PageSetup &p : pages) {
        p.page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        p.table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        p.table->setMinimumSize(0, 0);
        p.table->setMaximumSize(16777215, 16777215);
    }

    auto *mainLayout = new QHBoxLayout(centralWidget());
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(categoryList);
    mainLayout->addLayout(rightSide, 1);

    categoryList->setFixedWidth(180);
    this->setMinimumSize(1000, 640);
    this->resize(1080, 700);

    ui->skyrimPathGroupBox->hide();
    ui->toolsGroupBox->hide();

    // --- 4. Tables : lignes deux niveaux, pas de grille ---
    for (const PageSetup &p : pages) {
        p.table->insertColumn(2);              // nouvelle colonne Status
        p.table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        p.table->setAlternatingRowColors(false);
        p.table->verticalHeader()->setVisible(false);
        p.table->setShowGrid(false);
        p.table->setSelectionBehavior(QAbstractItemView::SelectRows);
        p.table->setSelectionMode(QAbstractItemView::SingleSelection);
        p.table->setItemDelegate(new TrackDelegate(this));
        p.table->hideColumn(1);
        p.table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        p.table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
        p.table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
        p.table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
        p.table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
        p.table->setColumnWidth(2, 120);
        p.table->setColumnWidth(3, 70);
        p.table->setColumnWidth(4, 70);
        p.table->setColumnWidth(5, 70);
        p.table->setHorizontalHeaderLabels({tr("Track"), "", tr("Status"), tr("Replace"), tr("Original"), tr("Current")});
        p.table->verticalHeader()->setDefaultSectionSize(44);
        p.table->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        p.table->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        p.table->setMinimumWidth(0);
    }

    // --- 5. Barre de statut : chemin + version detectee ---
    auto *pathLabel = new QLabel(skyrimPath, this);
    pathLabel->setObjectName("pathStatus");
    pathLabel->setMinimumWidth(0);
    pathLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    auto *versionLabel = new QLabel(skyrimVersion == "sse" ? "SE detected" : "Skyrim detected", this);
    versionLabel->setObjectName("versionStatus");
    statusBar()->addWidget(pathLabel, 1);
    statusBar()->addPermanentWidget(versionLabel);


    ui->toolsGroupBox->hide();
    ui->skyrimPathGroupBox->hide();

    player->setAudioOutput(new QAudioOutput());

    qApp->installTranslator(&translator);
    QString disclaimerTitle = tr("Bards Song Replacing");
    QMessageBox *disclaimer = new QMessageBox();
    disclaimer->setIcon(QMessageBox::Information);

    disclaimer->setWindowTitle(disclaimerTitle);
    disclaimer->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
    disclaimer->setTextFormat(Qt::RichText);
    disclaimer->setText(tr("v2.0 : <br />- Added compatibility with Special Edition. <br />- Added background image"));

    connect(ui->ageOfOppressionBtn, SIGNAL(clicked()), this, SLOT(ProcessFileBards()));
    connect(ui->ageOfViolenceBtn, SIGNAL(clicked()), this, SLOT(ProcessFileBards()));
    connect(ui->ragnarTheRedBtn, SIGNAL(clicked()), this, SLOT(ProcessFileBards()));
    connect(ui->taleOfTheTonguesBtn, SIGNAL(clicked()), this, SLOT(ProcessFileBards()));
    connect(ui->theDragonbornComesBtn, SIGNAL(clicked()), this, SLOT(ProcessFileBards()));

    connect(ui->restoreOppressionBtn, SIGNAL(clicked()), this, SLOT(RestoreBards()));
    connect(ui->restoreViolenceBtn, SIGNAL(clicked()), this, SLOT(RestoreBards()));
    connect(ui->restoreRagnarBtn, SIGNAL(clicked()), this, SLOT(RestoreBards()));
    connect(ui->restoreTonguesBtn, SIGNAL(clicked()), this, SLOT(RestoreBards()));
    connect(ui->restoreDragonbornComesBtn, SIGNAL(clicked()), this, SLOT(RestoreBards()));

    connect(ui->oppressionLyricsPushButton, SIGNAL(clicked()), this, SLOT(ShowLyrics()));
    connect(ui->violenceLyricsPushButton, SIGNAL(clicked()), this, SLOT(ShowLyrics()));
    connect(ui->ragnarLyricsPushButton, SIGNAL(clicked()), this, SLOT(ShowLyrics()));
    connect(ui->dragonbornComesLyricsPushButton, SIGNAL(clicked()), this, SLOT(ShowLyrics()));
    connect(ui->tonguesLyricsPushButton, SIGNAL(clicked()), this, SLOT(ShowLyrics()));

    //disclaimer->exec();
    ui->tabWidget->setCurrentIndex(0);
    ui->musicTabWidget->setCurrentIndex(0);

    this->setWindowTitle(version);
    //std::cout << "|- " << skyrimVanillaRealPath.toStdString() << " -|" << std::endl;
    //std::cout << "|- " <<  skyrimSpecialEditionRealPath.toStdString() << " -|" << std::endl;
    QMessageBox noPath;
    noPath.setWindowTitle("Attention");
    noPath.setIcon(QMessageBox::Warning);
    QMessageBox versionBox;
    versionBox.setWindowTitle("Attention");
    versionBox.setIcon(QMessageBox::Warning);
    versionBox.setText(tr("Is this path for Skyrim or Skyrim Special Edition ?"));
    if(skyrimVanillaRealPath != "" && skyrimSpecialEditionRealPath != "") {
        noPath.setText(tr("Both Skyrim AND Skyrim Special Edition were detected. Please choose the version you want to use with the 2 buttons below."));
        QAbstractButton *skyrimVanillaPathButton = noPath.addButton(tr("Skyrim"), QMessageBox::ActionRole);
        QAbstractButton *skyrimSpecialEditionPathButton = noPath.addButton(tr("Skyrim Special Edition"), QMessageBox::ActionRole);
        connect(skyrimVanillaPathButton, &QAbstractButton::clicked, this, [this]() {
            ConfirmOrChangePath(skyrimVanillaRealPath, tr("Skyrim"), [this](QString path) {
                skyrimVanillaRealPath = path;
                skyrimDataPath = skyrimVanillaRealPath + "/Data/";
                skyrimMusicRealPath = skyrimDataPath + "Music/";
                skyrimShoutsRealPath = skyrimDataPath + "sound/voice/";
                skyrimPath = skyrimVanillaRealPath;
                skyrimVersion = "vanilla";
                UpdateStatusBar();
            });
        });
        connect(skyrimSpecialEditionPathButton, &QAbstractButton::clicked, this, [this]() {
            ConfirmOrChangePath(skyrimSpecialEditionRealPath, tr("Skyrim Special Edition"), [this](QString path) {
                skyrimSpecialEditionRealPath = path;
                skyrimDataPath = skyrimSpecialEditionRealPath + "/Data/";
                skyrimMusicRealPath = skyrimDataPath + "music/";
                skyrimShoutsRealPath = skyrimDataPath + "sound/voice/";
                skyrimPath = skyrimSpecialEditionRealPath;
                ui->skyrimPathGroupBox->setTitle("Skyrim Special Edition Installed Path");
                skyrimVersion = "sse";
                UpdateStatusBar();
            });
        });
        noPath.exec();
    } else if(skyrimVanillaRealPath != "" && skyrimSpecialEditionRealPath == "") {
        ConfirmOrChangePath(skyrimVanillaRealPath, tr("Skyrim"), [this](QString path) {
            skyrimVanillaRealPath = path;
            skyrimDataPath = skyrimVanillaRealPath + "/Data/";
            skyrimMusicRealPath = skyrimDataPath + "Music/";
            skyrimShoutsRealPath = skyrimDataPath + "sound/voice/";
            skyrimPath = skyrimVanillaRealPath;
            skyrimVersion = "vanilla";
            UpdateStatusBar();
        });
    } else if(skyrimVanillaRealPath == "" && skyrimSpecialEditionRealPath != "") {
        ConfirmOrChangePath(skyrimSpecialEditionRealPath, tr("Skyrim Special Edition"), [this](QString path) {
            skyrimSpecialEditionRealPath = path;
            skyrimDataPath = skyrimSpecialEditionRealPath + "/Data/";
            skyrimMusicRealPath = skyrimDataPath + "music/";
            skyrimShoutsRealPath = skyrimDataPath + "sound/voice/";
            skyrimPath = skyrimSpecialEditionRealPath;
            ui->skyrimPathGroupBox->setTitle("Skyrim Special Edition Installed Path");
            skyrimVersion = "sse";
            UpdateStatusBar();
        });
    } else if(skyrimVanillaRealPath == "" && skyrimSpecialEditionRealPath == "") {
        noPath.setText(tr("None version of the game was detected. You may now select your game directory by clicking the button below"));
        QAbstractButton *skyrimSetPathButton = noPath.addButton(tr("Locate game directory"), QMessageBox::ActionRole);
        connect(skyrimSetPathButton, &QAbstractButton::clicked, this, [=]() {
            QString directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            skyrimDataPath = directory + "/Data/";
            skyrimPath = directory;
            ui->skyrimPathGroupBox->setTitle("Skyrim Installed Path");
        });
        noPath.exec();
        QAbstractButton *skyrimSetVanillaVersionButton = versionBox.addButton(tr("Skyrim"), QMessageBox::ActionRole);
        QAbstractButton *skyrimSetSpecialEditionVersionButton = versionBox.addButton(tr("Skyrim Special Edition"), QMessageBox::ActionRole);
        connect(skyrimSetVanillaVersionButton, &QAbstractButton::clicked, this, [=]() {
            skyrimMusicRealPath = skyrimDataPath + "Music/";
            skyrimShoutsRealPath = skyrimDataPath + "sound/voice/";
            skyrimVersion = "vanilla";
            UpdateStatusBar();
        });
        connect(skyrimSetSpecialEditionVersionButton, &QAbstractButton::clicked, this, [=]() {
            skyrimMusicRealPath = skyrimDataPath + "music/";
            skyrimShoutsRealPath = skyrimDataPath + "sound/voice/";
            skyrimVersion = "sse";
            UpdateStatusBar();
        });
        versionBox.exec();
    }
    QString currentMusicString = qApp->applicationDirPath() + "/current";
    QDir currentMusicDir = currentMusicString;
    if(!currentMusicDir.exists()) {
        currentMusicDir.mkdir(currentMusicString);
        currentMusicDir.mkdir(currentMusicString + "/converted");
    }

    // --- Combat category setup ---
    qDebug() << "Combat rowCount:" << ui->combatTableWidget->rowCount();
    for (int row = 0; row < ui->combatTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->combatTableWidget);
        auto* playBtn = new QPushButton(ui->combatTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->combatTableWidget);

        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);

        ui->combatTableWidget->setCellWidget(row, 3, browseBtn);
        ui->combatTableWidget->setCellWidget(row, 4, playBtn);
        ui->combatTableWidget->setCellWidget(row, 5, playCurrentBtn);
        if (row == 0) {
            qDebug() << "browseIcon null:" << browseIcon.isNull()
            << "| col count:" << ui->combatTableWidget->columnCount()
            << "| col3 width:" << ui->combatTableWidget->columnWidth(3)
            << "| widget at 0,3:" << (ui->combatTableWidget->cellWidget(0,3) != nullptr);
        }

        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ CombatButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ CombatPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ CombatPlayCurrentButtonClicked(row); });
    }

    // --- Dungeon category setup ---
    for (int row = 0; row < ui->dungeonTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->dungeonTableWidget);
        auto* playBtn = new QPushButton(ui->dungeonTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->dungeonTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->dungeonTableWidget->setCellWidget(row, 3, browseBtn);
        ui->dungeonTableWidget->setCellWidget(row, 4, playBtn);
        ui->dungeonTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ DungeonButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ DungeonPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ DungeonPlayCurrentButtonClicked(row); });
    }

    // --- Dread category setup ---
    for (int row = 0; row < ui->dreadTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->dreadTableWidget);
        auto* playBtn = new QPushButton(ui->dreadTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->dreadTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->dreadTableWidget->setCellWidget(row, 3, browseBtn);
        ui->dreadTableWidget->setCellWidget(row, 4, playBtn);
        ui->dreadTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ DreadButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ DreadPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ DreadPlayCurrentButtonClicked(row); });
    }

    // --- Explore category setup ---
    for (int row = 0; row < ui->exploreTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->exploreTableWidget);
        auto* playBtn = new QPushButton(ui->exploreTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->exploreTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->exploreTableWidget->setCellWidget(row, 3, browseBtn);
        ui->exploreTableWidget->setCellWidget(row, 4, playBtn);
        ui->exploreTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ ExploreButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ ExplorePlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ ExplorePlayCurrentButtonClicked(row); });
    }

    // --- Other category setup ---
    for (int row = 0; row < ui->otherTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->otherTableWidget);
        auto* playBtn = new QPushButton(ui->otherTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->otherTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->otherTableWidget->setCellWidget(row, 3, browseBtn);
        ui->otherTableWidget->setCellWidget(row, 4, playBtn);
        ui->otherTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ OtherButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ OtherPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ OtherPlayCurrentButtonClicked(row); });
    }

    // --- Reward category setup ---
    for (int row = 0; row < ui->rewardTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->rewardTableWidget);
        auto* playBtn = new QPushButton(ui->rewardTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->rewardTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->rewardTableWidget->setCellWidget(row, 3, browseBtn);
        ui->rewardTableWidget->setCellWidget(row, 4, playBtn);
        ui->rewardTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ RewardButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ RewardPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ RewardPlayCurrentButtonClicked(row); });
    }

    // --- Reveal category setup ---
    for (int row = 0; row < ui->revealTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->revealTableWidget);
        auto* playBtn = new QPushButton(ui->revealTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->revealTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->revealTableWidget->setCellWidget(row, 3, browseBtn);
        ui->revealTableWidget->setCellWidget(row, 4, playBtn);
        ui->revealTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ RevealButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ RevealPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ RevealPlayCurrentButtonClicked(row); });
    }

    // --- Special category setup ---
    for (int row = 0; row < ui->specialTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->specialTableWidget);
        auto* playBtn = new QPushButton(ui->specialTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->specialTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->specialTableWidget->setCellWidget(row, 3, browseBtn);
        ui->specialTableWidget->setCellWidget(row, 4, playBtn);
        ui->specialTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ SpecialButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ SpecialPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ SpecialPlayCurrentButtonClicked(row); });
    }

    // --- Stinger category setup ---
    for (int row = 0; row < ui->stingerTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->stingerTableWidget);
        auto* playBtn = new QPushButton(ui->stingerTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->stingerTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->stingerTableWidget->setCellWidget(row, 3, browseBtn);
        ui->stingerTableWidget->setCellWidget(row, 4, playBtn);
        ui->stingerTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ StingerButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ StingerPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ StingerPlayCurrentButtonClicked(row); });
    }

    // --- Tavern category setup ---
    for (int row = 0; row < ui->tavernTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->tavernTableWidget);
        auto* playBtn = new QPushButton(ui->tavernTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->tavernTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->tavernTableWidget->setCellWidget(row, 3, browseBtn);
        ui->tavernTableWidget->setCellWidget(row, 4, playBtn);
        ui->tavernTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ TavernButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ TavernPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ TavernPlayCurrentButtonClicked(row); });
    }

    // --- Town category setup ---
    for (int row = 0; row < ui->townTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->townTableWidget);
        auto* playBtn = new QPushButton(ui->townTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->townTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->townTableWidget->setCellWidget(row, 3, browseBtn);
        ui->townTableWidget->setCellWidget(row, 4, playBtn);
        ui->townTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ TownButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ TownPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ TownPlayCurrentButtonClicked(row); });
    }

    // --- Dawnguard category setup ---
    for (int row = 0; row < ui->dawnguardTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->dawnguardTableWidget);
        auto* playBtn = new QPushButton(ui->dawnguardTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->dawnguardTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->dawnguardTableWidget->setCellWidget(row, 3, browseBtn);
        ui->dawnguardTableWidget->setCellWidget(row, 4, playBtn);
        ui->dawnguardTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ DawnguardButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ DawnguardPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ DawnguardPlayCurrentButtonClicked(row); });
    }

    // --- Dragonborn category setup ---
    for (int row = 0; row < ui->dragonbornTableWidget->rowCount(); ++row) {
        auto* browseBtn = new QPushButton(ui->dragonbornTableWidget);
        auto* playBtn = new QPushButton(ui->dragonbornTableWidget);
        auto* playCurrentBtn = new QPushButton(ui->dragonbornTableWidget);
        browseBtn->setIcon(browseIcon);
        playBtn->setIcon(playIcon);
        playCurrentBtn->setIcon(playCurrentIcon);
        ui->dragonbornTableWidget->setCellWidget(row, 3, browseBtn);
        ui->dragonbornTableWidget->setCellWidget(row, 4, playBtn);
        ui->dragonbornTableWidget->setCellWidget(row, 5, playCurrentBtn);
        connect(browseBtn, &QPushButton::clicked, this, [this, row]{ DragonbornButtonClicked(row); });
        connect(playBtn, &QPushButton::clicked, this, [this, row]{ DragonbornPlayButtonClicked(row); });
        connect(playCurrentBtn, &QPushButton::clicked, this, [this, row]{ DragonbornPlayCurrentButtonClicked(row); });
    }

    RefreshMusicStatus();
    categoryList->setCurrentRow(1);   // force le rafraîchissement du sous-titre
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RetranslateCustomUi()
{
    // --- Sidebar : Shouts et Bards ---
    for (int i = 0; i < categoryList->count(); ++i) {
        QListWidgetItem *item = categoryList->item(i);
        int tabIndex = item->data(Qt::UserRole).toInt();
        int musicIndex = item->data(Qt::UserRole + 1).toInt();
        if (tabIndex == 1 && musicIndex == 0) {
            item->setText(tr("Shouts"));
        } else if (tabIndex == 1 && musicIndex == 1) {
            item->setText(tr("Bards"));
        }
    }

    // --- Tables musique : headers ---
    QList<QTableWidget*> musicTables = {
        ui->combatTableWidget, ui->dungeonTableWidget, ui->dreadTableWidget,
        ui->exploreTableWidget, ui->otherTableWidget, ui->revealTableWidget,
        ui->rewardTableWidget, ui->specialTableWidget, ui->stingerTableWidget,
        ui->tavernTableWidget, ui->townTableWidget, ui->dawnguardTableWidget,
        ui->dragonbornTableWidget
    };
    for (QTableWidget *table : musicTables) {
        table->setHorizontalHeaderLabels({tr("Track"), "", tr("Status"), tr("Replace"), tr("Original"), tr("Current")});
    }

    // --- Shouts ---
    if (shoutsTable) {
        shoutsTable->setHorizontalHeaderLabels({tr("Shout Name"), "", tr("Status"), tr("Actions")});
    }
    if (genderCombo) {
        int gIdx = genderCombo->currentIndex();
        genderCombo->blockSignals(true);
        for (int i = 0; i < genderCombo->count(); ++i) {
            QString val = genderCombo->itemData(i).toString();
            if (val == "male")   genderCombo->setItemText(i, tr("Male"));
            if (val == "female") genderCombo->setItemText(i, tr("Female"));
        }
        genderCombo->setCurrentIndex(gIdx);
        genderCombo->blockSignals(false);
    }
    if (raceCombo) {
        int rIdx = raceCombo->currentIndex();
        raceCombo->blockSignals(true);
        for (int i = 0; i < raceCombo->count(); ++i) {
            QString val = raceCombo->itemData(i).toString();
            if (val == "nord")       raceCombo->setItemText(i, tr("Nord"));
            if (val == "argonian")   raceCombo->setItemText(i, tr("Argonian"));
            if (val == "elfhaughty") raceCombo->setItemText(i, tr("Elf"));
            if (val == "khajiit")    raceCombo->setItemText(i, tr("Khajiit"));
            if (val == "orc")        raceCombo->setItemText(i, tr("Orc"));
        }
        raceCombo->setCurrentIndex(rIdx);
        raceCombo->blockSignals(false);
    }

    // --- Noms des cris : relire depuis les boutons retraduits ---
    QList<QPushButton*> shoutButtons = {
        ui->animalAllegianceBtn, ui->auraWhisperBtn,      ui->becomeEtheralBtn,
        ui->callDragonBtn,       ui->callOfValorBtn,      ui->clearSkiesBtn,
        ui->disarmBtn,           ui->dismayBtn,           ui->dragonrendBtn,
        ui->elementalFuryBtn,    ui->fireBreathBtn,       ui->frostBreathBtn,
        ui->iceFormBtn,          ui->kynePeaceBtn,        ui->markedForDeathBtn,
        ui->slowTimeBtn,         ui->stormCallBtn,        ui->throwVoiceBtn,
        ui->unrelentingForceBtn, ui->whirlwindSprintBtn,
        ui->drainVitalityBtn,    ui->soulTearBtn,         ui->summonDurnehviirBtn,
        ui->battleFuryBtn,       ui->bendWillBtn,         ui->cycloneBtn,
        ui->dragonAspectBtn,
    };
    for (int i = 0; i < shoutButtons.size() && i < shoutEntries.size(); ++i) {
        if (shoutButtons[i]) {
            shoutEntries[i].name = shoutButtons[i]->text();
        }
    }

    // --- Bards ---
    if (bardsTable) {
        bardsTable->setHorizontalHeaderLabels({tr("Song"), "", tr("Status"), tr("Lyrics"), tr("Actions")});
    }
    if (bardGenderCombo) {
        int bIdx = bardGenderCombo->currentIndex();
        bardGenderCombo->blockSignals(true);
        for (int i = 0; i < bardGenderCombo->count(); ++i) {
            QString val = bardGenderCombo->itemData(i).toString();
            if (val == "male")   bardGenderCombo->setItemText(i, tr("Male"));
            if (val == "female") bardGenderCombo->setItemText(i, tr("Female"));
        }
        bardGenderCombo->setCurrentIndex(bIdx);
        bardGenderCombo->blockSignals(false);
    }

    // --- Noms des chants : relire depuis les boutons retraduits ---
    QList<QPushButton*> bardButtons = {
        ui->ageOfOppressionBtn, ui->ageOfViolenceBtn, ui->ragnarTheRedBtn,
        ui->taleOfTheTonguesBtn, ui->theDragonbornComesBtn,
    };
    for (int i = 0; i < bardButtons.size() && i < bardEntries.size(); ++i) {
        if (bardButtons[i]) {
            bardEntries[i].name = bardButtons[i]->text();
        }
    }

    // --- Labels du bandeau ---
    if (auto *lbl = findChild<QLabel*>("bardFieldLabel")) {
        lbl->setText(tr("Voice"));
    }

    // --- Contenu des tables (statuts, sous-titres) ---
    RefreshShoutsTable();
    RefreshBardsTable();
    RefreshMusicStatus();
    UpdateStatusBar();

    // --- Sous-titre de la catégorie courante ---
    if (categoryList->currentItem()) {
        int count = categoryList->currentItem()->data(Qt::UserRole + 2).toInt();
        int replaced = categoryList->currentItem()->data(Qt::UserRole + 4).toInt();
        if (count > 0) {
            categorySubtitle->setText(tr("%1 tracks · %2 replaced").arg(count).arg(replaced));
        }
    }
}

void MainWindow::UpdateStatusBar()
{
    if (auto *lbl = statusBar()->findChild<QLabel*>("pathStatus")) {
        lbl->setText(skyrimPath);
    }
    if (auto *lbl = statusBar()->findChild<QLabel*>("versionStatus")) {
        lbl->setText(skyrimVersion == "sse" ? tr("SE detected") : tr("Skyrim detected"));
    }
}

// Compte combien des N fichiers .fuz existent deja pour ce chant
// et le genre actuellement selectionne.
int MainWindow::CountReplacedBardFiles(const QString &songKey, int fileCount) const
{
    if (genderBards.isEmpty() || raceBards.isEmpty()) return 0;

    QFile jsonFile(qApp->applicationDirPath() + "/bards.json");
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;
    QJsonObject root = QJsonDocument::fromJson(jsonFile.readAll()).object();
    jsonFile.close();

    QJsonObject item = root.value(songKey).toObject();
    QString folder = skyrimShoutsRealPath + "skyrim.esm/" + genderBards + raceBards + "/";

    int found = 0;
    for (int i = 0; i < fileCount; ++i) {
        QString fileName = bardDebutFile + item[QString::number(i)].toString()
        + fileEndWithExtension;
        if (QFile::exists(folder + fileName)) {
            ++found;
        }
    }
    return found;
}


void MainWindow::RefreshBardsTable()
{
    if (!bardsTable) return;

    bardsTable->setRowCount(bardEntries.size());
    int replacedCount = 0;

    for (int row = 0; row < bardEntries.size(); ++row) {
        const BardEntry &e = bardEntries[row];
        int done = CountReplacedBardFiles(e.songKey, e.fileCount);
        bool fullyReplaced = (done == e.fileCount);
        if (fullyReplaced) ++replacedCount;

        // Colonne 0 : nom du chant (+ flag pour la mise en valeur)
        auto *nameItem = new QTableWidgetItem(e.name);
        nameItem->setData(Qt::UserRole + 10, fullyReplaced);
        bardsTable->setItem(row, 0, nameItem);

        // Colonne 1 : sous-titre (masquee, lue par le delegate)
        auto *subItem = new QTableWidgetItem(tr("%1 files").arg(e.fileCount));
        bardsTable->setItem(row, 1, subItem);

        // Colonne 2 : statut
        QString statusText;
        QColor statusColor;
        if (fullyReplaced) {
            statusText  = tr("Replaced");
            statusColor = QColor("#7bc98a");
        } else if (done > 0) {
            statusText  = tr("%1 of %2 selected").arg(done).arg(e.fileCount);
            statusColor = QColor("#d4a94a");
        } else {
            statusText  = tr("Original");
            statusColor = QColor("#6e6e78");
        }
        auto *statusItem = new QTableWidgetItem(statusText);
        statusItem->setForeground(statusColor);
        bardsTable->setItem(row, 2, statusItem);

        // Colonne 3 : bouton Lyrics
        auto *lyricsWidget = new QWidget(bardsTable);
        auto *lyricsLayout = new QHBoxLayout(lyricsWidget);
        lyricsLayout->setContentsMargins(0, 0, 0, 0);
        lyricsLayout->addStretch();
        auto *lyricsBtn = new QPushButton(tr("Lyrics"), lyricsWidget);
        lyricsBtn->setObjectName("linkButton");
        lyricsLayout->addWidget(lyricsBtn);
        lyricsLayout->addStretch();
        bardsTable->setCellWidget(row, 3, lyricsWidget);

        // Colonne 4 : bouton de remplacement
        auto *actionWidget = new QWidget(bardsTable);
        auto *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 8, 0);
        actionLayout->addStretch();
        auto *replaceBtn = new QPushButton(actionWidget);
        replaceBtn->setIcon(browseIcon);
        replaceBtn->setToolTip(tr("Replace this song"));
        actionLayout->addWidget(replaceBtn);
        bardsTable->setCellWidget(row, 4, actionWidget);

        BardEntry captured = e;
        connect(replaceBtn, &QPushButton::clicked, this, [this, captured]() {
            ReplaceBardSongFor(captured.songKey);
        });
        connect(lyricsBtn, &QPushButton::clicked, this, [this, captured]() {
            ShowLyricsFor(captured.lyricsKey, captured.name);
        });
    }

    categorySubtitle->setText(tr("%1 songs · %2 replaced")
                                  .arg(bardEntries.size())
                                  .arg(replacedCount));
}


// Version parametree de ShowLyrics (l'ancienne lisait sender())
void MainWindow::ShowLyricsFor(const QString &lyricsKey, const QString &displayName)
{
    QFile file(qApp->applicationDirPath() + "/lyrics_" + language + ".json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not open the lyrics file."));
        return;
    }
    QJsonObject lyrics = QJsonDocument::fromJson(file.readAll()).object();
    file.close();

    QJsonObject lyricsSong = lyrics.value(lyricsKey).toObject();
    QString lyricsText;
    const QStringList keys = lyricsSong.keys();
    for (const QString &key : keys) {
        lyricsText += lyricsSong.value(key).toString();
    }

    auto *lyricsBox = new QMessageBox(this);
    lyricsBox->setTextFormat(Qt::RichText);
    lyricsBox->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
    lyricsBox->setWindowTitle(tr("Lyrics for %1").arg(displayName));
    lyricsBox->setText(lyricsText);
    lyricsBox->exec();
    lyricsBox->deleteLater();
}


// Version parametree de ProcessFileBards, avec progression visible
void MainWindow::ReplaceBardSongFor(const QString &songKey)
{
    if (genderBards.isEmpty() || raceBards.isEmpty()) {
        QMessageBox::warning(this, tr("Voice not selected"),
                             tr("Select a gender first."));
        return;
    }

    QFile file(qApp->applicationDirPath() + "/bards.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not open bards.json"));
        return;
    }
    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
    file.close();

    QJsonObject item = root.value(songKey).toObject();
    const int fileCount = BardFileCount(songKey);

    QString voiceInfo = bardGenderCombo->currentText();
    ShoutReplaceDialog dlg(songKey, voiceInfo, fileCount, this);
    dlg.setWindowTitle(tr("Replace bard song"));

    if (dlg.exec() != QDialog::Accepted) return;
    if (!dlg.allSelected()) return;

    QStringList files = dlg.selectedFiles();

    auto *waitBox = new QMessageBox(this);
    waitBox->setIcon(QMessageBox::Information);
    waitBox->setWindowTitle(tr("Replace bard song"));
    waitBox->setText(tr("Encoding %1 files, please wait...").arg(fileCount));
    waitBox->setStandardButtons(QMessageBox::NoButton);
    waitBox->open();

    QList<QPair<QString, QStringList>> steps;
    for (int i = 0; i < files.size(); ++i) {
        steps += BuildWavXwmBardsSteps(files[i], item, QString::number(i));
    }
    for (int i = 0; i < files.size(); ++i) {
        steps.append(BuildFuzBardsStep(item, QString::number(i)));
    }

    RunProcessChain(steps, [this, waitBox, item, fileCount]() {
        QString folderName = genderBards + raceBards;
        QString stringPath = skyrimShoutsRealPath + "skyrim.esm/" + folderName + "/";
        QDir directory(stringPath);
        if (!directory.exists()) {
            directory.mkpath(".");
        }

        QString appDir = qApp->applicationDirPath();
        for (int i = 0; i < fileCount; ++i) {
            QString idx     = QString::number(i);
            QString base    = bardDebutFile + item[idx].toString();
            QString fuzName = base + fileEndWithExtension;
            QString wavName = base + fileEndWithoutExtension + ".wav";
            QString xwmName = base + fileEndWithoutExtension + ".xwm";

            QFile::copy(appDir + "/" + fuzName, stringPath + fuzName);
            QFile::remove(appDir + "/" + wavName);
            QFile::remove(appDir + "/" + xwmName);
            QFile::remove(appDir + "/" + fuzName);
        }

        waitBox->setText(tr("Bard song replaced successfully."));
        waitBox->setStandardButtons(QMessageBox::Close);
        RefreshBardsTable();
    });
}

int MainWindow::BardFileCount(const QString &songKey)
{
    if (songKey == "Ragnar the Red") return 6;
    if (songKey == "Tale of the Tongues" || songKey == "The Dragonborn Comes") return 8;
    return 5;
}

void MainWindow::SetupBardsScreen()
{
    // --- 1. Construire la liste des chants depuis les boutons du .ui ---
    //     Chaque chant a trois boutons distincts dans le .ui :
    //       - le bouton de remplacement  -> accessibleName() = cle bards.json
    //       - le bouton Restore          -> accessibleName() = cle de RestoreBards
    //       - le bouton Lyrics           -> accessibleName() = cle lyrics_xx.json
    struct BardSource {
        QPushButton *replaceBtn;
        QPushButton *restoreBtn;
        QPushButton *lyricsBtn;
    };
    QList<BardSource> sources = {
                                  {ui->ageOfOppressionBtn,    ui->restoreOppressionBtn,      ui->oppressionLyricsPushButton},
                                  {ui->ageOfViolenceBtn,      ui->restoreViolenceBtn,        ui->violenceLyricsPushButton},
                                  {ui->ragnarTheRedBtn,       ui->restoreRagnarBtn,          ui->ragnarLyricsPushButton},
                                  {ui->taleOfTheTonguesBtn,   ui->restoreTonguesBtn,         ui->tonguesLyricsPushButton},
                                  {ui->theDragonbornComesBtn, ui->restoreDragonbornComesBtn, ui->dragonbornComesLyricsPushButton},
                                  };

    bardEntries.clear();
    for (const BardSource &s : sources) {
        if (!s.replaceBtn) continue;
        BardEntry e;
        e.name       = s.replaceBtn->text();
        e.songKey    = s.replaceBtn->accessibleName();
        e.restoreKey = s.restoreBtn ? s.restoreBtn->accessibleName() : QString();
        e.lyricsKey  = s.lyricsBtn  ? s.lyricsBtn->accessibleName()  : QString();
        e.fileCount  = BardFileCount(e.songKey);
        bardEntries.append(e);
    }

    // --- 2. Masquer l'ancienne interface ---
    ui->bardGenderGroupBox->hide();
    ui->resetBardsBtn->hide();
    ui->lyricsGroupBox->hide();
    ui->bardsSongsGroupBox->hide();

    // --- 3. Selecteur de genre pour le bandeau global ---
    auto *bardVoiceLabel = new QLabel(tr("Voice"), this);
    bardVoiceLabel->setObjectName("fieldLabel");

    bardGenderCombo = new QComboBox(this);
    bardGenderCombo->setObjectName("voiceCombo");
    // male -> nord, female -> eventoned (logique d'origine du projet)
    bardGenderCombo->addItem(tr("Male"),   "male");
    bardGenderCombo->addItem(tr("Female"), "female");

    bardSelectors = new QWidget(this);
    auto *bardVoiceLayout = new QHBoxLayout(bardSelectors);
    bardVoiceLayout->setContentsMargins(0, 0, 0, 0);
    bardVoiceLayout->setSpacing(8);
    bardVoiceLayout->addWidget(bardVoiceLabel);
    bardVoiceLayout->addWidget(bardGenderCombo);
    bardSelectors->hide();

    // --- 4. Table des chants ---
    bardsTable = new QTableWidget(ui->bardsTab);
    bardsTable->setObjectName("shoutsTable");   // reutilise le meme style
    bardsTable->setColumnCount(5);
    bardsTable->setHorizontalHeaderLabels(
        {tr("Song"), "", tr("Status"), tr("Lyrics"), tr("Actions")});
    bardsTable->verticalHeader()->setVisible(false);
    bardsTable->setShowGrid(false);
    bardsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    bardsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    bardsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bardsTable->setItemDelegate(new TrackDelegate(this));
    bardsTable->hideColumn(1);
    bardsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    bardsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    bardsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    bardsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    bardsTable->setColumnWidth(2, 150);
    bardsTable->setColumnWidth(3, 80);
    bardsTable->setColumnWidth(4, 70);
    bardsTable->verticalHeader()->setDefaultSectionSize(44);
    bardsTable->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    bardsTable->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    bardsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bardsTable->setMinimumWidth(0);

    // --- 5. Pied de page ---
    auto *footerWidget = new QWidget(ui->bardsTab);
    footerWidget->setObjectName("footerBar");
    footerWidget->setFixedHeight(48);
    auto *footerLayout = new QHBoxLayout(footerWidget);
    footerLayout->setContentsMargins(20, 8, 20, 8);
    footerLayout->addStretch();

    auto *restoreAllBtn = new QPushButton(tr("Restore all bard songs"), footerWidget);
    restoreAllBtn->setObjectName("restoreAllShoutsBtn");
    footerLayout->addWidget(restoreAllBtn);

    // --- 6. Assemblage ---
    auto *pageLayout = new QVBoxLayout(ui->bardsTab);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);
    pageLayout->addWidget(bardsTable, 1);
    pageLayout->addWidget(footerWidget);
    ui->bardsTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // --- 7. Connexions ---
    connect(bardGenderCombo, &QComboBox::currentIndexChanged, this, [this](int) {
        genderBards = bardGenderCombo->currentData().toString();
        raceBards   = (genderBards == "male") ? "nord" : "eventoned";
        RefreshBardsTable();
    });

    connect(restoreAllBtn, &QPushButton::clicked, this, [this]() {
        QMessageBox box(this);
        box.setWindowTitle(tr("Restore bard songs"));
        box.setIcon(QMessageBox::Warning);
        box.setText(tr("Restore all original bard songs for %1?")
                        .arg(bardGenderCombo->currentText()));
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        box.setDefaultButton(QMessageBox::No);
        if (box.exec() != QMessageBox::Yes) return;

        QFile jsonFile(qApp->applicationDirPath() + "/bards.json");
        if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        QJsonObject root = QJsonDocument::fromJson(jsonFile.readAll()).object();
        jsonFile.close();

        QString folder = skyrimShoutsRealPath + "skyrim.esm/" + genderBards + raceBards + "/";
        for (const BardEntry &e : bardEntries) {
            QJsonObject item = root.value(e.songKey).toObject();
            for (int i = 0; i < e.fileCount; ++i) {
                QString fileName = bardDebutFile + item[QString::number(i)].toString()
                + fileEndWithExtension;
                QFile::remove(folder + fileName);
            }
        }
        RefreshBardsTable();
    });

    // Initialise genre/race depuis la selection par defaut
    genderBards = bardGenderCombo->currentData().toString();
    raceBards   = (genderBards == "male") ? "nord" : "eventoned";
    RefreshBardsTable();
}


// Reconstruit le chemin du .xwm dans le dossier du jeu, en tenant compte
// des sous-dossiers specifiques (dungeon/special/dlc01).
QString MainWindow::ResolveMusicFilePath(const QString &category, const QString &fileName) const
{
    QString musicPath = (skyrimVersion == "vanilla")
                            ? skyrimPath + "/Data/Music/"
                            : skyrimPath + "/Data/music/";

    if (category == "other" || category == "dragonborn") {
        return musicPath + fileName + ".xwm";
    }
    if (category == "dungeon") {
        QString sub = fileName.mid(12, 2);
        if (sub == "ic") return musicPath + category + "/ice/"  + fileName + ".xwm";
        if (sub == "ca") return musicPath + category + "/cave/" + fileName + ".xwm";
        if (sub == "fo") return musicPath + category + "/fort/" + fileName + ".xwm";
        return musicPath + category + "/" + fileName + ".xwm";
    }
    if (category == "special") {
        if (fileName.mid(4, 5) == "failu")
            return musicPath + category + "/failure/" + fileName + ".xwm";
        return musicPath + category + "/" + fileName + ".xwm";
    }
    if (category == "dlc01") {
        QString sub = fileName.mid(4, 7);
        if (sub == "dungeon") return musicPath + category + "/dungeon/" + fileName + ".xwm";
        if (sub == "explore") return musicPath + category + "/explore/falmervalley/" + fileName + ".xwm";
        if (sub == "soulcai") return musicPath + category + "/explore/soulcairn/palette/" + fileName + ".xwm";
        return musicPath + category + "/" + fileName + ".xwm";
    }
    return musicPath + category + "/" + fileName + ".xwm";
}

void MainWindow::RefreshMusicStatus()
{
    struct CatTable { QString category; QTableWidget *table; };
    QList<CatTable> cats = {
                            {"combat",     ui->combatTableWidget},
                            {"dungeon",    ui->dungeonTableWidget},
                            {"dread",      ui->dreadTableWidget},
                            {"explore",    ui->exploreTableWidget},
                            {"other",      ui->otherTableWidget},
                            {"reveal",     ui->revealTableWidget},
                            {"reward",     ui->rewardTableWidget},
                            {"special",    ui->specialTableWidget},
                            {"stinger",    ui->stingerTableWidget},
                            {"tavern",     ui->tavernTableWidget},
                            {"town",       ui->townTableWidget},
                            {"dlc01",      ui->dawnguardTableWidget},
                            {"dragonborn", ui->dragonbornTableWidget},
                            };

    for (const CatTable &c : cats) {
        int replaced = 0;
        for (int row = 0; row < c.table->rowCount(); ++row) {
            QTableWidgetItem *fileItem = c.table->item(row, 1);
            if (!fileItem) continue;

            QString path = ResolveMusicFilePath(c.category, fileItem->text());
            bool isReplaced = QFile::exists(path);
            if (isReplaced) ++replaced;

            auto *statusItem = c.table->item(row, 2);
            if (!statusItem) {
                statusItem = new QTableWidgetItem();
                statusItem->setFlags(Qt::ItemIsEnabled);
                c.table->setItem(row, 2, statusItem);
            }
            statusItem->setText(isReplaced ? tr("Replaced") : tr("Original"));
            statusItem->setForeground(isReplaced ? QColor("#7bc98a") : QColor("#6e6e78"));
            if (auto *nameItem = c.table->item(row, 0)) {
                nameItem->setData(Qt::UserRole + 10, isReplaced);
            }
        }

        // Met a jour le compteur de la sidebar pour cette categorie
        for (int i = 0; i < categoryList->count(); ++i) {
            QListWidgetItem *it = categoryList->item(i);
            if (it->data(Qt::UserRole + 3).toString() == c.category) {
                it->setData(Qt::UserRole + 4, replaced);
                break;
            }
        }
    }
}

int MainWindow::CountReplacedWords(const QString &subDir, const QString &jsonKey) const
{
    if (gender.isEmpty() || race.isEmpty()) return 0;

    QFile jsonFile(qApp->applicationDirPath() + "/shouts.json");
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;
    QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();

    QJsonObject root = doc.object();
    QJsonObject item = root.value(jsonKey).toObject();
    QJsonObject fileItem = (subDir == "dragonborn")
                               ? root.value("Files_Dragonborn").toObject()
                               : root.value("Files").toObject();

    QString folder = skyrimShoutsRealPath + subDir + ".esm/" + gender + race + "/";

    int found = 0;
    for (int i = 0; i < 4; ++i) {
        QString idx = QString::number(i);
        QString fileName = fileItem[idx].toString() + item[idx].toString() + fileEndWithExtension;
        if (QFile::exists(folder + fileName)) {
            ++found;
        }
    }
    return found;
}


void MainWindow::RefreshShoutsTable()
{
    if (!shoutsTable) return;

    shoutsTable->setRowCount(shoutEntries.size());
    int replacedCount = 0;

    for (int row = 0; row < shoutEntries.size(); ++row) {
        const ShoutEntry &e = shoutEntries[row];

        // Colonne 0 : nom du cri (le delegate lit la colonne 1 comme sous-titre)
                QString origin;
        if (e.subDir == "dawnguard")        origin = tr("Dawnguard");
        else if (e.subDir == "dragonborn")  origin = tr("Dragonborn");
        else                                origin = tr("Vanilla");
        auto *subItem = new QTableWidgetItem(origin + " · " + tr("4 words"));
        shoutsTable->setItem(row, 1, subItem);

        // Colonne 2 : statut
        int done = CountReplacedWords(e.subDir, e.jsonKey);
        QString statusText;
        QColor statusColor;
        if (done == 4) {
            statusText = tr("Replaced");
            statusColor = QColor("#7bc98a");
            ++replacedCount;
        } else if (done > 0) {
            statusText = tr("%1 of 4 selected").arg(done);
            statusColor = QColor("#d4a94a");
        } else {
            statusText = tr("Original");
            statusColor = QColor("#6e6e78");
        }
        auto *nameItem = new QTableWidgetItem(e.name);
        nameItem->setData(Qt::UserRole + 10, done == 4);
        shoutsTable->setItem(row, 0, nameItem);
        auto *statusItem = new QTableWidgetItem(statusText);
        statusItem->setForeground(statusColor);
        shoutsTable->setItem(row, 2, statusItem);

        // Colonne 3 : bouton de remplacement
        auto *actionWidget = new QWidget(shoutsTable);
        auto *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 8, 0);
        actionLayout->setSpacing(8);
        actionLayout->addStretch();

        auto *replaceBtn = new QPushButton(actionWidget);
        replaceBtn->setIcon(browseIcon);
        replaceBtn->setToolTip(tr("Replace this shout"));
        actionLayout->addWidget(replaceBtn);
        shoutsTable->setCellWidget(row, 3, actionWidget);

        ShoutEntry captured = e;
        connect(replaceBtn, &QPushButton::clicked, this, [this, captured]() {
            ReplaceShoutFor(captured.name, captured.subDir, captured.jsonKey);
        });
    }

    categorySubtitle->setText(tr("%1 shouts · %2 replaced")
                                .arg(shoutEntries.size())
                                .arg(replacedCount));
}

void MainWindow::ReplaceShoutFor(const QString &shoutName,
                                 const QString &subDir,
                                 const QString &jsonKey)
{
    if (gender.isEmpty() || race.isEmpty()) {
        QMessageBox::warning(this, tr("Voice not selected"),
                             tr("Select a gender and a race first."));
        return;
    }

    QFile file(qApp->applicationDirPath() + "/shouts.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not open shouts.json"));
        return;
    }
    QJsonDocument d = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject root = d.object();
    QJsonObject item = root.value(jsonKey).toObject();
    QJsonObject fileItem = (subDir == "dragonborn")
                               ? root.value("Files_Dragonborn").toObject()
                               : root.value("Files").toObject();

    QString voiceInfo = genderCombo->currentText() + " " + raceCombo->currentText();
    ShoutReplaceDialog dlg(shoutName, voiceInfo, 4, this);

    if (dlg.exec() != QDialog::Accepted) return;
    if (!dlg.allSelected()) return;

    QStringList files = dlg.selectedFiles();

    // Dialogue d'attente
    auto *waitBox = new QMessageBox(this);
    waitBox->setIcon(QMessageBox::Information);
    waitBox->setWindowTitle(tr("Replace shout"));
    waitBox->setText(tr("Encoding, please wait..."));
    waitBox->setStandardButtons(QMessageBox::NoButton);
    waitBox->open();

    QList<QPair<QString, QStringList>> steps;
    for (int i = 0; i < files.size(); ++i) {
        steps += BuildWavXwmSteps(files[i], item, fileItem, QString::number(i));
    }
    for (int i = 0; i < files.size(); ++i) {
        steps.append(BuildFuzStep(item, fileItem, QString::number(i)));
    }

    RunProcessChain(steps, [this, waitBox, item, fileItem, subDir]() {
        QString folderName = gender + race;
        QString stringPath = skyrimShoutsRealPath + subDir + ".esm/" + folderName + "/";
        QDir directory(stringPath);
        if (!directory.exists()) {
            directory.mkpath(".");
        }

        QString appDir = qApp->applicationDirPath();
        for (int i = 0; i < 4; ++i) {
            QString idx = QString::number(i);
            QString base    = fileItem[idx].toString() + item[idx].toString();
            QString fuzName = base + fileEndWithExtension;
            QString wavName = base + fileEndWithoutExtension + ".wav";
            QString xwmName = base + fileEndWithoutExtension + ".xwm";

            QFile::copy(appDir + "/" + fuzName, stringPath + fuzName);
            QFile::remove(appDir + "/" + wavName);
            QFile::remove(appDir + "/" + xwmName);
            QFile::remove(appDir + "/" + fuzName);
        }

        waitBox->setText(tr("Shout replaced successfully."));
        waitBox->setStandardButtons(QMessageBox::Close);
        RefreshShoutsTable();
    });
}

void MainWindow::SetupShoutsScreen()
{
    // --- 1. Recuperer les donnees des cris depuis les boutons du .ui ---
    //     text()                 -> libelle affiche
    //     accessibleName()       -> sous-dossier (skyrim / dawnguard / dragonborn)
    //     accessibleDescription()-> cle dans shouts.json
    QList<QPushButton*> sourceButtons = {
        ui->animalAllegianceBtn, ui->auraWhisperBtn,      ui->becomeEtheralBtn,
        ui->callDragonBtn,       ui->callOfValorBtn,      ui->clearSkiesBtn,
        ui->disarmBtn,           ui->dismayBtn,           ui->dragonrendBtn,
        ui->elementalFuryBtn,    ui->fireBreathBtn,       ui->frostBreathBtn,
        ui->iceFormBtn,          ui->kynePeaceBtn,        ui->markedForDeathBtn,
        ui->slowTimeBtn,         ui->stormCallBtn,        ui->throwVoiceBtn,
        ui->unrelentingForceBtn, ui->whirlwindSprintBtn,
        ui->drainVitalityBtn,    ui->soulTearBtn,         ui->summonDurnehviirBtn,
        ui->battleFuryBtn,       ui->bendWillBtn,         ui->cycloneBtn,
        ui->dragonAspectBtn,
    };

    shoutEntries.clear();
    for (QPushButton *btn : sourceButtons) {
        if (!btn) continue;
        ShoutEntry e;
        e.name    = btn->text();
        e.subDir  = btn->accessibleName();
        e.jsonKey = btn->accessibleDescription();
        shoutEntries.append(e);
    }

    // --- 2. Masquer l'ancienne interface a boutons ---
    ui->genderGroupBox->hide();
    ui->raceGroupBox->hide();
    ui->setRaceBtn->hide();
    ui->pushButton_2->hide();
    ui->shoutGroupBox->hide();

    // --- 3. Bandeau : titre + selecteurs de voix ---
    // Les combos vont dans le bandeau global, pas dans un nouveau
    auto *voiceLabel = new QLabel(tr("Voice"), this);
    voiceLabel->setObjectName("fieldLabel");

    genderCombo = new QComboBox(this);
    genderCombo->setObjectName("voiceCombo");
    QList<QRadioButton*> genderButtons = { ui->maleRadioButton, ui->femaleRadioButton };
    for (QRadioButton *rb : genderButtons) {
        if (rb) genderCombo->addItem(rb->text(), rb->accessibleName());
    }

    raceCombo = new QComboBox(this);
    raceCombo->setObjectName("voiceCombo");
    QList<QRadioButton*> raceButtons = {
        ui->argonianRadioButton, ui->elfhaughtyRadioButton, ui->kahjiitRadioButton,
        ui->nordicRadioButton,   ui->orcRadioButton
    };
    for (QRadioButton *rb : raceButtons) {
        if (rb) raceCombo->addItem(rb->text(), rb->accessibleName());
    }

    voiceSelectors = new QWidget(this);
    auto *voiceLayout = new QHBoxLayout(voiceSelectors);
    voiceLayout->setContentsMargins(0, 0, 0, 0);
    voiceLayout->setSpacing(8);
    voiceLayout->addWidget(voiceLabel);
    voiceLayout->addWidget(genderCombo);
    voiceLayout->addWidget(raceCombo);
    voiceSelectors->hide();

    // --- 4. Table des cris ---
    shoutsTable = new QTableWidget(ui->shoutTab);
    shoutsTable->setObjectName("shoutsTable");
    shoutsTable->setColumnCount(4);
    shoutsTable->setHorizontalHeaderLabels({tr("Shout Name"), "", tr("Status"), tr("Actions")});
    shoutsTable->verticalHeader()->setVisible(false);
    shoutsTable->setShowGrid(false);
    shoutsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    shoutsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    shoutsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    shoutsTable->setItemDelegate(new TrackDelegate(this));
    shoutsTable->hideColumn(1);
    shoutsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    shoutsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    shoutsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    shoutsTable->setColumnWidth(2, 150);
    shoutsTable->setColumnWidth(3, 70);
    shoutsTable->setMinimumWidth(0);
    shoutsTable->verticalHeader()->setDefaultSectionSize(44);
    shoutsTable->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    shoutsTable->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    shoutsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // --- 5. Pied de page : restauration globale pour la voix courante ---
    auto *footerWidget = new QWidget(ui->shoutTab);
    footerWidget->setObjectName("footerBar");
    footerWidget->setFixedHeight(48);
    auto *footerLayout = new QHBoxLayout(footerWidget);
    footerLayout->setContentsMargins(20, 8, 20, 8);
    footerLayout->addStretch();

    auto *restoreAllBtn = new QPushButton(tr("Restore all for this voice"), footerWidget);
    restoreAllBtn->setObjectName("restoreAllShoutsBtn");
    footerLayout->addWidget(restoreAllBtn);

    // --- 6. Assemblage de la page ---
    auto *pageLayout = new QVBoxLayout(ui->shoutTab);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);
    pageLayout->addWidget(shoutsTable, 1);
    pageLayout->addWidget(footerWidget);
    ui->shoutTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // --- 7. Connexions ---
    connect(genderCombo, &QComboBox::currentIndexChanged, this, [this](int) {
        gender = genderCombo->currentData().toString();
        RefreshShoutsTable();
    });
    connect(raceCombo, &QComboBox::currentIndexChanged, this, [this](int) {
        race = raceCombo->currentData().toString();
        RefreshShoutsTable();
    });
    connect(restoreAllBtn, &QPushButton::clicked, this, [this]() {
        QString folderName = gender + race;
        QMessageBox box(this);
        box.setWindowTitle(tr("Restore shouts"));
        box.setIcon(QMessageBox::Warning);
        box.setText(tr("Restore all original shouts for %1 %2?")
                        .arg(genderCombo->currentText(), raceCombo->currentText()));
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        box.setDefaultButton(QMessageBox::No);
        if (box.exec() == QMessageBox::Yes) {
            for (const QString &sub : {"skyrim", "dawnguard", "dragonborn"}) {
                QDir dir(skyrimShoutsRealPath + sub + ".esm/" + folderName);
                if (dir.exists()) {
                    dir.removeRecursively();
                }
            }
            RefreshShoutsTable();
        } else {
            return;
        }
    });

    // Initialise gender/race depuis la selection par defaut des combos
    gender = genderCombo->currentData().toString();
    race   = raceCombo->currentData().toString();
    RefreshShoutsTable();
}


void MainWindow::SetupSidebar()
{
    ui->tabWidget->tabBar()->hide();
    ui->musicTabWidget->tabBar()->hide();
    ui->voiceTabWidget->tabBar()->hide();

    categoryList = new QListWidget(this);
    categoryList->setObjectName("categorySidebar");
    categoryList->setFrameShape(QFrame::NoFrame);
    categoryList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    struct Entry { QString label; int tabIndex; int musicIndex; bool isHeader; int count; QString category; };
    QList<Entry> entries = {
                            {"Music",      -1, -1, true,  0, ""},
                            {"Combat",      0,  0, false, ui->combatTableWidget->rowCount(),     "combat"},
                            {"Dungeon",     0,  1, false, ui->dungeonTableWidget->rowCount(),    "dungeon"},
                            {"Dread",       0,  2, false, ui->dreadTableWidget->rowCount(),      "dread"},
                            {"Explore",     0,  3, false, ui->exploreTableWidget->rowCount(),    "explore"},
                            {"Other",       0,  4, false, ui->otherTableWidget->rowCount(),      "other"},
                            {"Reveal",      0,  5, false, ui->revealTableWidget->rowCount(),     "reveal"},
                            {"Reward",      0,  6, false, ui->rewardTableWidget->rowCount(),     "reward"},
                            {"Special",     0,  7, false, ui->specialTableWidget->rowCount(),    "special"},
                            {"Stinger",     0,  8, false, ui->stingerTableWidget->rowCount(),    "stinger"},
                            {"Tavern",      0,  9, false, ui->tavernTableWidget->rowCount(),     "tavern"},
                            {"Town",        0, 10, false, ui->townTableWidget->rowCount(),       "town"},
                            {"Dawnguard",   0, 11, false, ui->dawnguardTableWidget->rowCount(),  "dlc01"},
                            {"Dragonborn",  0, 12, false, ui->dragonbornTableWidget->rowCount(), "dragonborn"},
                            {"Voice",      -1, -1, true,  0, ""},
                            {"Shouts",      1,  0, false, 0, ""},
                            {"Bards",       1,  1, false, 0, ""},
                            };

    for (const Entry &e : entries) {
        auto *item = new QListWidgetItem(e.label, categoryList);
        if (e.isHeader) {
            item->setFlags(Qt::ItemIsEnabled);
            item->setText(e.label.toUpper());
            item->setData(Qt::UserRole, -1);
            item->setData(Qt::UserRole + 1, -1);
            item->setData(Qt::UserRole + 2, 0);
            item->setData(Qt::UserRole + 3, e.category);

            QFont headerFont = item->font();
            headerFont.setBold(true);
            int ps = headerFont.pointSize();
            if (ps > 0) {
                headerFont.setPointSize(ps + 1);
            }
            item->setFont(headerFont);
            item->setForeground(QColor("#7ecbff"));
        } else {
            item->setData(Qt::UserRole, e.tabIndex);
            item->setData(Qt::UserRole + 1, e.musicIndex);
            item->setData(Qt::UserRole + 2, e.count);
            item->setData(Qt::UserRole + 3, e.category);
            if (e.count > 0) {
                item->setToolTip(QString("%1 tracks").arg(e.count));
            }
        }
    }

    categoryList->setItemDelegate(new CategoryDelegate(categoryList));
    categoryTitle = new QLabel(this);
    categoryTitle->setObjectName("categoryTitle");

    categorySubtitle = new QLabel(this);
    categorySubtitle->setObjectName("categorySubtitle");

    connect(categoryList, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem *current, QListWidgetItem *) {
                if (!current) return;
                int tabIndex = current->data(Qt::UserRole).toInt();
                int musicIndex = current->data(Qt::UserRole + 1).toInt();
                if (tabIndex < 0) return;

                ui->tabWidget->setCurrentIndex(tabIndex);
                if (tabIndex == 0) {
                    ui->musicTabWidget->setCurrentIndex(musicIndex);
                } else {
                    ui->voiceTabWidget->setCurrentIndex(musicIndex);
                }

                categoryTitle->setText(current->text());
                int count = current->data(Qt::UserRole + 2).toInt();
                int replaced = current->data(Qt::UserRole + 4).toInt();
                if (count > 0) {
                    categorySubtitle->setText(tr("%1 tracks · %2 replaced").arg(count).arg(replaced));
                } else {
                    categorySubtitle->setText(QString());
                }
                if (voiceSelectors) {
                    voiceSelectors->setVisible(current->text() == "Shouts");
                }
                if (bardSelectors) {
                    bardSelectors->setVisible(current->text() == "Bards");
                }

            });

    categoryList->setCurrentRow(1);
}

void MainWindow::ConfirmOrChangePath(QString detectedPath, const QString &versionName, std::function<void(QString)> onPathReady)
{
    QMessageBox confirmBox(this);
    confirmBox.setWindowTitle(tr("Verify %1 path").arg(versionName));
    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setText(tr("Detected %1 path:\n%2\n\nIs this correct?").arg(versionName, detectedPath));
    confirmBox.addButton(tr("Confirm"), QMessageBox::YesRole);
    QAbstractButton *changeButton = confirmBox.addButton(tr("Change..."), QMessageBox::ActionRole);

    confirmBox.exec();
    if (confirmBox.clickedButton() == changeButton) {
        QString newDir = QFileDialog::getExistingDirectory(this, tr("Select %1 folder").arg(versionName), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!newDir.isEmpty()) {
            onPathReady(newDir);
        } else {
            onPathReady(detectedPath);
        }
    } else if(confirmBox.clickedButton() == nullptr) {
        return;
    }  else {
        onPathReady(detectedPath);
    }
}

void MainWindow::RunProcessChain(QList<QPair<QString, QStringList>> steps, std::function<void()> onComplete)
{
    if (steps.isEmpty()) {
        if (onComplete) onComplete();
        return;
    }
    auto step = steps.takeFirst();
    auto *process = new QProcess(this);
    auto alreadyHandled = std::make_shared<bool>(false);

    auto continueChain = [this, process, steps, onComplete, alreadyHandled]() mutable {
        if (*alreadyHandled) return;
        *alreadyHandled = true;
        process->deleteLater();
        RunProcessChain(steps, onComplete);
    };

    connect(process, &QProcess::finished, this, [continueChain](int, QProcess::ExitStatus) mutable {
        continueChain();
    });
    connect(process, &QProcess::errorOccurred, this, [continueChain](QProcess::ProcessError error) mutable {
        qDebug() << "Process failed to run:" << error;
        continueChain();
    });

    process->start(step.first, step.second);
}

void MainWindow::onPlayerStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) {
        player->play();
    }
}

void MainWindow::CombatButtonClicked(int row, QString type)
{
    QString ref = ui->combatTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::DungeonButtonClicked(int row, QString type)
{
    QString ref = ui->dungeonTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::DreadButtonClicked(int row, QString type)
{
    QString ref = ui->dreadTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::ExploreButtonClicked(int row, QString type)
{
    QString ref = ui->exploreTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::OtherButtonClicked(int row, QString type)
{
    QString ref = ui->otherTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::RewardButtonClicked(int row, QString type)
{
    QString ref = ui->rewardTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::RevealButtonClicked(int row, QString type)
{
    QString ref = ui->revealTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::StingerButtonClicked(int row, QString type)
{
    QString ref = ui->stingerTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::SpecialButtonClicked(int row, QString type)
{
    QString ref = ui->specialTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::TavernButtonClicked(int row, QString type)
{
    QString ref = ui->tavernTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::TownButtonClicked(int row, QString type)
{
    QString ref = ui->townTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::DawnguardButtonClicked(int row, QString type)
{
    QString ref = ui->dawnguardTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::DragonbornButtonClicked(int row, QString type)
{
    QString ref = ui->dragonbornTableWidget->item(row,1)->text()+".xwm";
    ProcessFile(type,ref);
}

void MainWindow::CombatPlayButtonClicked(int row)
{
    QString name = ui->combatTableWidget->item(row,0)->text();
    player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/combat/"+name+".mp3"));
}

void MainWindow::CombatPlayCurrentButtonClicked(int row)
{
    QString name = ui->combatTableWidget->item(row,1)->text();
    QString type = "combat";
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";
    QString previewFile = musicPath + type + "/" + name + ".xwm";

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) {
        QFile file(itCurrent.next());
        file.remove();
    }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) {
        QFile file(itCurrentConverted.next());
        file.remove();
    }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::DungeonPlayCurrentButtonClicked(int row)
{
    QString name = ui->dungeonTableWidget->item(row,1)->text();
    QString type = "dungeon";
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";

    QString fileSubCategory = name.mid(12,2);
    QString previewFile;
    if(fileSubCategory == "ic") {
        previewFile = musicPath + type + "/ice/" + name + ".xwm";
    } else if(fileSubCategory == "ca") {
        previewFile = musicPath + type + "/cave/" + name + ".xwm";
    } else if(fileSubCategory == "fo") {
        previewFile = musicPath + type + "/fort/" + name + ".xwm";
    } else {
        previewFile = musicPath + type + "/" + name + ".xwm";
    }

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::DreadPlayCurrentButtonClicked(int row)
{
    QString name = ui->dreadTableWidget->item(row,1)->text();
    QString type = "dread";
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";
    QString previewFile = musicPath + type + "/" + name + ".xwm";

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::ExplorePlayCurrentButtonClicked(int row)
{
    QString name = ui->exploreTableWidget->item(row,1)->text();
    QString type = "explore";
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";
    QString previewFile = musicPath + type + "/" + name + ".xwm";

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::OtherPlayCurrentButtonClicked(int row)
{
    QString name = ui->otherTableWidget->item(row,1)->text();
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";
    QString previewFile = musicPath + name + ".xwm";

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::RewardPlayCurrentButtonClicked(int row)
{
    QString name = ui->rewardTableWidget->item(row,1)->text();
    QString type = "reward";
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";
    QString previewFile = musicPath + type + "/" + name + ".xwm";

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::RevealPlayCurrentButtonClicked(int row)
{
    QString name = ui->revealTableWidget->item(row,1)->text();
    QString type = "reveal";
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";
    QString previewFile = musicPath + type + "/" + name + ".xwm";

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::StingerPlayCurrentButtonClicked(int row)
{
    QString name = ui->stingerTableWidget->item(row,1)->text();
    QString type = "stinger";
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";
    QString previewFile = musicPath + type + "/" + name + ".xwm";

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::SpecialPlayCurrentButtonClicked(int row)
{
    QString name = ui->specialTableWidget->item(row,1)->text();
    QString type = "special";
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";

    QString fileSubCategory = name.mid(4,5);
    QString previewFile = (fileSubCategory == "failu")
                              ? musicPath + type + "/failure/" + name + ".xwm"
                              : musicPath + type + "/" + name + ".xwm";

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::TavernPlayCurrentButtonClicked(int row)
{
    QString name = ui->tavernTableWidget->item(row,1)->text();
    QString type = "tavern";
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";
    QString previewFile = musicPath + type + "/" + name + ".xwm";

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::TownPlayCurrentButtonClicked(int row)
{
    QString name = ui->townTableWidget->item(row,1)->text();
    QString type = "town";
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";
    QString previewFile = musicPath + type + "/" + name + ".xwm";

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::DawnguardPlayCurrentButtonClicked(int row)
{
    QString name = ui->dawnguardTableWidget->item(row,1)->text();
    QString type = "dlc01";
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";

    QString fileSubCategory = name.mid(4,7);
    QString previewFile;
    if(fileSubCategory == "dungeon") {
        previewFile = musicPath + type + "/dungeon/" + name + ".xwm";
    } else if(fileSubCategory == "explore") {
        previewFile = musicPath + type + "/explore/falmervalley/" + name + ".xwm";
    } else if(fileSubCategory == "soulcai") {
        previewFile = musicPath + type + "/explore/soulcairn/palette/" + name + ".xwm";
    } else if(fileSubCategory == "eclipse") {
        previewFile = musicPath + type + "/" + name + ".xwm";
    }

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}

void MainWindow::DragonbornPlayCurrentButtonClicked(int row)
{
    QString name = ui->dragonbornTableWidget->item(row,1)->text();
    QString src = qApp->applicationDirPath();
    QString musicPath = (skyrimVersion == "vanilla") ? skyrimPath + "/Data/Music/" : skyrimPath + "/Data/music/";
    QString previewFile = musicPath + name + ".xwm";

    QDirIterator itCurrent(src + "/current/", QDir::Files);
    while(itCurrent.hasNext()) { QFile file(itCurrent.next()); file.remove(); }
    QDirIterator itCurrentConverted(src + "/current/converted/", QDir::Files);
    while(itCurrentConverted.hasNext()) { QFile file(itCurrentConverted.next()); file.remove(); }

    QFile customFile(previewFile);
    customFile.copy(previewFile, src + "/current/" + name + ".xwm");

    QString wavConverter = src + "/ffmpeg.exe";
    QString destWav = src + "/current/converted/" + name + ".wav";
    QStringList argumentsWav;
    argumentsWav << "-i" << src + "/current/" + name + ".xwm" << destWav;

    auto *wavEncoder = new QProcess(this);
    connect(wavEncoder, &QProcess::finished, this, [this, wavEncoder, destWav](int, QProcess::ExitStatus) {
        player->setSource(QUrl::fromLocalFile(destWav));
        wavEncoder->deleteLater();
    });
    wavEncoder->start(wavConverter, argumentsWav);
}



void MainWindow::DungeonPlayButtonClicked(int row)
{
    QString name = ui->dungeonTableWidget->item(row,0)->text();
    QString replace = name;
    if(replace.left(4) == "Bene") {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/dungeon/Beneath the Ice.mp3"));
    } else {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/dungeon/"+name+".mp3"));
    }
}

void MainWindow::DreadPlayButtonClicked(int row)
{
    QString name = ui->dreadTableWidget->item(row,0)->text();
    player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/dread/"+name+".mp3"));
}

void MainWindow::ExplorePlayButtonClicked(int row)
{
    QString name = ui->exploreTableWidget->item(row,0)->text();
    QString replace = name;
    if(replace.left(4) == "From") {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/explore/From Past to Present.mp3"));
    } else {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/explore/"+name+".mp3"));
    }
}

void MainWindow::OtherPlayButtonClicked(int row)
{
    QString name = ui->otherTableWidget->item(row,0)->text();
    player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/other/"+name+".mp3"));
}

void MainWindow::RewardPlayButtonClicked(int row)
{
    QString name = ui->rewardTableWidget->item(row,0)->text();
    player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/reward/"+name+".mp3"));
}

void MainWindow::RevealPlayButtonClicked(int row)
{
    QString name = ui->revealTableWidget->item(row,0)->text();
    player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/reveal/"+name+".mp3"));
}

void MainWindow::StingerPlayButtonClicked(int row)
{
    QString name = ui->stingerTableWidget->item(row,0)->text();
    player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/stinger/"+name+".mp3"));
}

void MainWindow::SpecialPlayButtonClicked(int row)
{
    QString name = ui->specialTableWidget->item(row,0)->text();
    QString replace = name;
    if(replace.left(7) == "Failure") {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/special/failure/"+name+".mp3"));
    } else {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/special/"+name+".mp3"));
    }
}

void MainWindow::TavernPlayButtonClicked(int row)
{
    QString name = ui->tavernTableWidget->item(row,0)->text();
    player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/tavern/"+name+".mp3"));
}

void MainWindow::TownPlayButtonClicked(int row)
{
    QString name = ui->townTableWidget->item(row,0)->text();
    QString replace = name;
    if(replace.left(4) == "From") {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/explore/From Past to Present.mp3"));
    } else {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/town/"+name+".mp3"));
    }
}

void MainWindow::DawnguardPlayButtonClicked(int row)
{
    QString name = ui->dawnguardTableWidget->item(row,0)->text();
    QString replace = name;
    if(replace.left(3) == "The") {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/dlc01/"+name+".mp3"));
    } else if(replace.left(4) == "Cast" || replace.left(4) == "Soul") {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/dlc01/dungeon/"+name+".mp3"));
    } else if(replace.left(4) == "Forg") {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/dlc01/explore/falmervalley/"+name+".mp3"));
    } else {
        player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/dlc01/explore/soulcairn/palette/"+name+".mp3"));
    }
}

void MainWindow::DragonbornPlayButtonClicked(int row)
{
    QString name = ui->dragonbornTableWidget->item(row,0)->text();
    QString replace = name;
    player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/audio/dragonborn/"+name+".mp3"));
}

void MainWindow::ProcessFile(QString type, QString reference)
{
    QString src = qApp->applicationDirPath();
    QString completeFile = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Audio (*.wav *.mp3 *.wma)"));
    if (completeFile == "") return;

    QDir pathDir;
    QString finalPath;
    QString xwmConverter = qApp->applicationDirPath()+"/encode.exe";
    QString convertedString = src + "/converted";
    QDir convertedDir = convertedString;
    if (!convertedDir.exists()){
        convertedDir.mkdir(convertedString);
    }

    if(type == "other" || type == "dragonborn") {
        pathDir = skyrimMusicRealPath;
        finalPath = skyrimMusicRealPath;
    } else if(type == "dlc01") {
        QString filenameDebut = reference.mid(4,7);
        if(filenameDebut == "dungeon") {
            pathDir = skyrimMusicRealPath+type+"/dungeon";
            finalPath = skyrimMusicRealPath + type + "/dungeon/";
        } else if(filenameDebut == "eclipse") {
            pathDir = skyrimMusicRealPath+type;
            finalPath = skyrimMusicRealPath + type + "/";
        } else if(filenameDebut == "explore") {
            pathDir = skyrimMusicRealPath+type+"/explore/falmervalley";
            finalPath = skyrimMusicRealPath + type + "/explore/falmervalley/";
        } else if(filenameDebut == "soulcai") {
            pathDir = skyrimMusicRealPath+type+"/explore/soulcairn/palette";
            finalPath = skyrimMusicRealPath + type + "/explore/soulcairn/palette/";
        }
    } else if(type == "dungeon") {
        QString fileSubCategory = reference.mid(12,2);
        if(fileSubCategory == "ic") {
            pathDir = skyrimMusicRealPath+type+"/ice";
            finalPath = skyrimMusicRealPath + type + "/ice/";
        } else if(fileSubCategory == "ca") {
            pathDir = skyrimMusicRealPath+type+"/cave";
            finalPath = skyrimMusicRealPath + type + "/cave/";
        } else if(fileSubCategory == "fo") {
            pathDir = skyrimMusicRealPath+type+"/fort";
            finalPath = skyrimMusicRealPath + type + "/fort/";
        } else {
            pathDir = skyrimMusicRealPath+type;
            finalPath = skyrimMusicRealPath + type + "/";
        }
    } else if(type == "special") {
        QString fileSubCategory = reference.mid(4,5);
        if(fileSubCategory == "failu") {
            pathDir = skyrimMusicRealPath+type+"/failure";
            finalPath = skyrimMusicRealPath + type + "/failure/";
        } else {
            pathDir = skyrimMusicRealPath+type;
            finalPath = skyrimMusicRealPath + type + "/";
        }
    } else {
        pathDir = skyrimMusicRealPath+type;
        finalPath = skyrimMusicRealPath + type + "/";
    }

    QFileInfo fileInfo(completeFile);
    QDir::setCurrent(fileInfo.absoluteFilePath());
    QString refWav = QString(reference).replace(".xwm", ".wav");
    QString fileExtension = completeFile.right(3);

    QList<QPair<QString, QStringList>> steps;
    QString wavConverter = qApp->applicationDirPath()+"/ffmpeg.exe";

    if (fileExtension != "wav") {
        QStringList argumentsWav;
        argumentsWav << "-i" << completeFile << qApp->applicationDirPath() + "/" + refWav;
        steps.append({wavConverter, argumentsWav});

        QStringList argumentsXwm;
        argumentsXwm << qApp->applicationDirPath()+"/"+refWav << qApp->applicationDirPath()+"/"+reference;
        steps.append({xwmConverter, argumentsXwm});
    } else {
        QFile originalFile(completeFile);
        originalFile.copy(completeFile, src + "/" + refWav);
        QStringList argumentsXwm;
        argumentsXwm << completeFile << qApp->applicationDirPath()+"/"+reference;
        steps.append({xwmConverter, argumentsXwm});
    }

    RunProcessChain(steps, [this, reference, refWav, pathDir, finalPath, type]() mutable {
        QString src = qApp->applicationDirPath();

        QDir musicDir = skyrimMusicRealPath;
        if(!musicDir.exists()) {
            musicDir.mkdir(skyrimDataPath+"music");
        }
        if(!pathDir.exists()) {
            pathDir.mkdir(skyrimMusicRealPath+type);
            if(type == "dlc01") {
                pathDir.mkdir(skyrimMusicRealPath+type+"/dungeon");
                pathDir.mkdir(skyrimMusicRealPath+type+"/explore");
                pathDir.mkdir(skyrimMusicRealPath+type+"/explore/falmervalley");
                pathDir.mkdir(skyrimMusicRealPath+type+"/explore/soulcairn");
                pathDir.mkdir(skyrimMusicRealPath+type+"/explore/soulcairn/palette");
            } else if(type == "dungeon") {
                pathDir.mkdir(skyrimMusicRealPath+type+"/dungeon");
                pathDir.mkdir(skyrimMusicRealPath+type+"/cave");
                pathDir.mkdir(skyrimMusicRealPath+type+"/fort");
                pathDir.mkdir(skyrimMusicRealPath+type+"/ice");
            } else if(type == "special") {
                pathDir.mkdir(skyrimMusicRealPath+type+"/failure");
            }
        }

        QFile finalFile(reference);
        QFile wavFile(refWav);
        wavFile.copy(src + "/" + refWav, src + "/converted/" + refWav);

        QMessageBox *preview = new QMessageBox(this);
        preview->setIcon(QMessageBox::Warning);
        preview->setWindowTitle("Attention");
        preview->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
        preview->setText(tr("If you want to get a preview of your new file, you may click \"Preview\". Otherwise, you can just click \"Replace\""));
        QAbstractButton *previewButton = preview->addButton(tr("Preview"), QMessageBox::ActionRole);
        QAbstractButton *replaceButton = preview->addButton(tr("Replace"), QMessageBox::ActionRole);
        QAbstractButton *cancelButton = preview->addButton(tr("Cancel"), QMessageBox::ActionRole);

        connect(previewButton, &QAbstractButton::clicked, this, [this, refWav](){
            player->setSource(QUrl::fromLocalFile(qApp->applicationDirPath()+"/"+refWav));
        });
        connect(cancelButton, &QAbstractButton::clicked, this, [preview](){
            preview->close();
        });

        preview->exec();

        if(preview->clickedButton() == nullptr) {
            player->stop();
            player->setSource(QUrl());
        } else if(preview->clickedButton() == replaceButton) {
            player->stop();
            player->setSource(QUrl());
            bool wavExists = QFile::exists(qApp->applicationDirPath()+"/"+refWav);
            finalFile.copy(src + "/" + reference, finalPath + reference);
            finalFile.remove(src + "/" + reference);
            finalFile.remove(src + "/converted/" + reference);
            if(wavExists) {
                wavFile.remove(src + "/" + refWav);
            }
            QMessageBox *confirm = new QMessageBox();
            confirm->setWindowTitle(tr("Music Replacement"));
            confirm->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
            confirm->setText("Success !");
            confirm->open();
            QDir::setCurrent(qApp->applicationDirPath());
            RefreshMusicStatus();
        } else {
            // Cancel ou autre — ne rien faire
            player->stop();
            player->setSource(QUrl());
        }
    });
}

QString MainWindow::BoolToString(bool a)
{
    return a ? "true" : "false";
}


void MainWindow::on_btnClearCombat_clicked()
{
    RestoreOriginal("combat");
}

void MainWindow::on_btnClearDungeon_clicked()
{
    RestoreOriginal("dungeon");
}

void MainWindow::on_btnClearExplore_clicked()
{
    RestoreOriginal("explore");
}

void MainWindow::on_btnClearOther_clicked()
{
    RestoreOriginal("other");
}

void MainWindow::on_btnClearSpecial_clicked()
{
    RestoreOriginal("special");
}

void MainWindow::on_btnClearStinger_clicked()
{
    RestoreOriginal("stinger");
}

void MainWindow::on_btnClearTavern_clicked()
{
    RestoreOriginal("tavern");
}

void MainWindow::on_btnClearTown_clicked()
{
    RestoreOriginal("town");
}

void MainWindow::on_btnClearDawnguard_clicked()
{
    RestoreOriginal("dlc01");
}

void MainWindow::on_btnClearDragonborn_clicked()
{
    RestoreOriginal("dragonborn");
}

void MainWindow::RestoreOriginal(QString typeMusic)
{
    QDir musicDir = skyrimMusicRealPath;
    QDir typeMusicDir = skyrimMusicRealPath+typeMusic;
    QString realType = typeMusic;
    if(realType == "dlc01") {
        realType = "Dawnguard";
    }
    if(realType == "dragonborn") {
        QDirIterator it(skyrimMusicRealPath, QStringList() << "mus_dlc2*.xwm" << "mus_dungeon_dlc2*.xwm" << "mus_explore_dlc2*.xwm", QDir::Files);
        while(it.hasNext()) {
            QFile file(it.next());
            file.remove();
        }
    }
    QMessageBox *warningBox = new QMessageBox();
    warningBox->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
    warningBox->setWindowTitle("ATTENTION");
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText(tr("Are you sure to restore \"%1\" musics to original ?").arg(realType));
    warningBox->setStandardButtons(QMessageBox::Yes);
    warningBox->addButton(QMessageBox::No);
    warningBox->setDefaultButton(QMessageBox::No);
    if(warningBox->exec() == QMessageBox::Yes) {
        if(typeMusic != "other" && typeMusic != "dragonborn") {
            if(typeMusicDir.exists()) {
                typeMusicDir.removeRecursively();
            }
        } else if(typeMusic == "dragonborn") {
            QDirIterator it(skyrimMusicRealPath, QStringList() << "mus_dlc2*.xwm" << "mus_dungeon_dlc2*.xwm" << "mus_explore_dlc2*.xwm", QDir::Files);
            while(it.hasNext()) {
                QFile file(it.next());
                file.remove();
            }
        } else {
            QDirIterator it(skyrimMusicRealPath, QStringList() << "mus_castle_stormcloaks.xwm" << "mus_castle_imperial_01.xwm" << "mus_discover_highhrothgar.xwm" << "mus_sovngarde_chant_lp.xwm", QDir::Files);
            while(it.hasNext()) {
                QFile file(it.next());
                file.remove();
            }
        }
    }
}

void MainWindow::on_stopMusicButton_clicked()
{
    player->pause();
}

void MainWindow::on_pushButton_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(skyrimDataPath));
}

void MainWindow::on_btnClearReveal_clicked()
{
    RestoreOriginal("reveal");
}

void MainWindow::on_btnClearReward_clicked()
{
    RestoreOriginal("reward");
}

void MainWindow::on_btnClearDread_clicked()
{
    RestoreOriginal("dread");
}

void MainWindow::on_changePathButton_clicked()
{
    QMessageBox *changePathBox = new QMessageBox();
    changePathBox->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
    changePathBox->setWindowTitle("ATTENTION");
    changePathBox->setIcon(QMessageBox::Warning);
    changePathBox->setText(tr("Are you sure to change the default path of \"Music\" directory ?"));
    changePathBox->setStandardButtons(QMessageBox::Yes);
    changePathBox->addButton(QMessageBox::No);
    changePathBox->setDefaultButton(QMessageBox::No);
    if(changePathBox->exec() == QMessageBox::Yes) {
        QString changeMusicDirectory = QFileDialog::getExistingDirectory();
        skyrimMusicRealPath = changeMusicDirectory + "/";
    }
}

void MainWindow::on_restoreMusicPathButton_clicked()
{
    QMessageBox *changePathBox = new QMessageBox();
    changePathBox->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
    changePathBox->setWindowTitle("ATTENTION");
    changePathBox->setIcon(QMessageBox::Warning);
    changePathBox->setText(tr("Are you sure to restore the original path of \"Music\" directory ?"));
    changePathBox->setStandardButtons(QMessageBox::Yes);
    changePathBox->addButton(QMessageBox::No);
    changePathBox->setDefaultButton(QMessageBox::No);
    if(changePathBox->exec() == QMessageBox::Yes) {
        skyrimMusicRealPath = skyrimDataPath + "music/";
    }
}

QList<QPair<QString, QStringList>> MainWindow::BuildWavXwmSteps(QString fileToEncode, QJsonObject item, QJsonObject fileItem, QString index)
{
    QList<QPair<QString, QStringList>> steps;
    QString wavConverter = qApp->applicationDirPath()+"/ffmpeg.exe";
    QString xwmConverter = qApp->applicationDirPath()+"/encode.exe";
    QString id = item[index].toString();
    QString fileDebut = fileItem[index].toString();
    QString reference = fileDebut + id + fileEndWithoutExtension + ".xwm";
    QString fileExtension = fileToEncode.right(3);

    if (fileExtension == "wav") {
        QStringList argsXwm;
        argsXwm << fileToEncode << qApp->applicationDirPath()+"/"+reference;
        steps.append({xwmConverter, argsXwm});
    } else {
        QFileInfo file(fileToEncode);
        QDir::setCurrent(file.absoluteFilePath());
        QString refWav = QString(reference).replace(".xwm", ".wav");
        QStringList argsWav;
        argsWav << "-i" << fileToEncode << qApp->applicationDirPath()+"/"+refWav;
        steps.append({wavConverter, argsWav});
        QStringList argsXwm;
        argsXwm << qApp->applicationDirPath()+"/"+refWav << qApp->applicationDirPath()+"/"+reference;
        steps.append({xwmConverter, argsXwm});
    }
    return steps;
}

QPair<QString, QStringList> MainWindow::BuildFuzStep(QJsonObject item, QJsonObject fileItem, QString index)
{
    QString fuzConverter = qApp->applicationDirPath()+"/fuz_extractor.exe";
    QString id = item[index].toString();
    QString fileDebut = fileItem[index].toString();
    QString reference = fileDebut + id + fileEndWithoutExtension + ".xwm";
    QStringList args;
    args << "-c" << qApp->applicationDirPath() + "/" + fileDebut + id + fileEndWithExtension
         << qApp->applicationDirPath() + "/" + reference
         << qApp->applicationDirPath() + "/lipfile.lip";
    return {fuzConverter, args};
}

QList<QPair<QString, QStringList>> MainWindow::BuildWavXwmBardsSteps(QString fileToEncode, QJsonObject item, QString index)
{
    QList<QPair<QString, QStringList>> steps;
    QString wavConverter = qApp->applicationDirPath()+"/ffmpeg.exe";
    QString xwmConverter = qApp->applicationDirPath()+"/encode.exe";
    QString id = item[index].toString();
    QString reference = bardDebutFile + id + fileEndWithoutExtension + ".xwm";
    QString fileExtension = fileToEncode.right(3);

    if (fileExtension == "wav") {
        QStringList argsXwm;
        argsXwm << fileToEncode << qApp->applicationDirPath()+"/"+reference;
        steps.append({xwmConverter, argsXwm});
    } else {
        QFileInfo file(fileToEncode);
        QDir::setCurrent(file.absoluteFilePath());
        QString refWav = QString(reference).replace(".xwm", ".wav");
        QStringList argsWav;
        argsWav << "-i" << fileToEncode << qApp->applicationDirPath()+"/"+refWav;
        steps.append({wavConverter, argsWav});
        QStringList argsXwm;
        argsXwm << qApp->applicationDirPath()+"/"+refWav << qApp->applicationDirPath()+"/"+reference;
        steps.append({xwmConverter, argsXwm});
    }
    return steps;
}

QPair<QString, QStringList> MainWindow::BuildFuzBardsStep(QJsonObject item, QString index)
{
    QString fuzConverter = qApp->applicationDirPath()+"/fuz_extractor.exe";
    QString id = item[index].toString();
    QString reference = bardDebutFile + id + fileEndWithoutExtension + ".xwm";
    QStringList args;
    args << "-c" << qApp->applicationDirPath() + "/" + bardDebutFile + id + fileEndWithExtension
         << qApp->applicationDirPath() + "/" + reference
         << qApp->applicationDirPath() + "/lipfile.lip";
    return {fuzConverter, args};
}

void MainWindow::Delay(int seconds)
{
    QTime dieTime= QTime::currentTime().addSecs(seconds);
    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void MainWindow::on_maleBardRadioButton_toggled(bool checked)
{
    genderBards = "male";
    raceBards = "nord";
    ui->bardsSongsGroupBox->setEnabled(true);
}

void MainWindow::on_femaleBardRadioButton_toggled(bool checked)
{
    genderBards = "female";
    raceBards = "eventoned";
    ui->bardsSongsGroupBox->setEnabled(true);
}

void MainWindow::ProcessFileBards()
{
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    QString songName = buttonSender->accessibleName();
    QString val;
    QJsonValue value;
    QFile file(qApp->applicationDirPath() + "/" + "bards.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject sett2 = d.object();
    value = sett2.value(QString(songName));
    QJsonObject item = value.toObject();

    QString firstFile = QFileDialog::getOpenFileName(this, tr("File 1"), QString(), "Audio (*.wav *.mp3 *.wma)");
    QString secondFile = QFileDialog::getOpenFileName(this, tr("File 2"), QString(), "Audio (*.wav *.mp3 *.wma)");
    QString thirdFile = QFileDialog::getOpenFileName(this, tr("File 3"), QString(), "Audio (*.wav *.mp3 *.wma)");
    QString fourthFile = QFileDialog::getOpenFileName(this, tr("File 4"), QString(), "Audio (*.wav *.mp3 *.wma)");
    QString fifthFile = QFileDialog::getOpenFileName(this, tr("File 5"), QString(), "Audio (*.wav *.mp3 *.wma)");

    QString folderName = genderBards+raceBards;
    QMessageBox *bardBox = new QMessageBox;
    bardBox->setWindowTitle(tr("Bards Replacing"));
    QDir directory(skyrimShoutsRealPath+"skyrim.esm/"+folderName);
    if(!directory.exists()) {
        directory.mkpath(".");
    }

    QStringList allFiles = {firstFile, secondFile, thirdFile, fourthFile, fifthFile};
    QString sixthFile, seventhFile, eigthFile;

    if(songName == "Ragnar the Red") {
        sixthFile = QFileDialog::getOpenFileName(this, tr("File 6"), QString(), "Audio (*.wav *.mp3 *.wma)");
        allFiles << sixthFile;
    } else if(songName == "Tale of the Tongues" || songName == "The Dragonborn Comes") {
        sixthFile = QFileDialog::getOpenFileName(this, tr("File 6"), QString(), "Audio (*.wav *.mp3 *.wma)");
        seventhFile = QFileDialog::getOpenFileName(this, tr("File 7"), QString(), "Audio (*.wav *.mp3 *.wma)");
        eigthFile = QFileDialog::getOpenFileName(this, tr("File 8"), QString(), "Audio (*.wav *.mp3 *.wma)");
        allFiles << sixthFile << seventhFile << eigthFile;
    }

    bool allValid = true;
    for (const QString &f : allFiles) {
        if (f.isEmpty()) { allValid = false; break; }
    }

    if (!allValid) {
        bardBox->setText(tr("All files must be valid in order to continue !"));
        bardBox->exec();
        return;
    }

    bardBox->setText(tr("You will now wait for a few seconds. Press Ok to proceed"));
    bardBox->exec();

    QList<QPair<QString, QStringList>> steps;
    for (int i = 0; i < allFiles.size(); ++i) {
        steps += BuildWavXwmBardsSteps(allFiles[i], item, QString::number(i));
    }
    for (int i = 0; i < allFiles.size(); ++i) {
        steps.append(BuildFuzBardsStep(item, QString::number(i)));
    }

    int fileCount = allFiles.size();

    RunProcessChain(steps, [this, bardBox, item, folderName, fileCount]() {
        QString stringPath = skyrimShoutsRealPath+"skyrim.esm/"+folderName+"/";
        for (int i = 0; i < fileCount; ++i) {
            QString index = QString::number(i);
            QString file = bardDebutFile+item[index].toString()+fileEndWithExtension;
            QString fileWav = bardDebutFile+item[index].toString()+fileEndWithoutExtension+".wav";
            QString fileXwm = bardDebutFile+item[index].toString()+fileEndWithoutExtension+".xwm";

            QFile finalFile(file);
            QFile finalFileWav(fileWav);
            QFile finalFileXwm(fileXwm);

            finalFile.copy(qApp->applicationDirPath() + "/" + file, stringPath + file);
            finalFileWav.remove(qApp->applicationDirPath() + "/" + fileWav);
            finalFileXwm.remove(qApp->applicationDirPath() + "/" + fileXwm);
            finalFile.remove(qApp->applicationDirPath() + "/" + file);
        }
        bardBox->setText(tr("Bard song replaced !"));
        bardBox->exec();
    });
}

void MainWindow::RestoreBards()
{
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    QString songName = buttonSender->accessibleName();
    QStringList argumentsDelete;
    if(songName == "oppression") {
        argumentsDelete << "bardsongs__000d0836_1.fuz" << "bardsongs__000d0837_1.fuz" << "bardsongs__000d0838_1.fuz" << "bardsongs__000d0839_1.fuz" << "bardsongs__000d083a_1.fuz";
    } else if(songName == "violence") {
        argumentsDelete << "bardsongs__0009583f_1.fuz" << "bardsongs__00095840_1.fuz" << "bardsongs__00095841_1.fuz" << "bardsongs__000d0839_1.fuz" << "bardsongs__000d083a_1.fuz";
    } else if(songName == "ragnar") {
        argumentsDelete << "bardsongs__000d083c_1.fuz" << "bardsongs__000d083d_1.fuz" << "bardsongs__000d083e_1.fuz" << "bardsongs__000d083f_1.fuz" << "bardsongs__000d0840_1.fuz" << "bardsongs__000d0841_1.fuz";
    } else if(songName == "tongues") {
        argumentsDelete << "bardsongs__00074773_1.fuz" << "bardsongs__000748da_1.fuz" << "bardsongs__00074922_1.fuz" << "bardsongs__00074984_1.fuz" << "bardsongs__00074988_1.fuz" << "bardsongs__00074989_1.fuz" << "bardsongs__00074997_1.fuz" << "bardsongs__000749b1_1.fuz";
    } else if(songName == "dragonbornComes") {
        argumentsDelete << "bardsongs__00074a09_1.fuz" << "bardsongs__00074a16_1.fuz" << "bardsongs__00074a0a_1.fuz" << "bardsongs__00074a43_1.fuz" << "bardsongs__00074a0d_1.fuz" << "bardsongs__00074a44_1.fuz" << "bardsongs__00074a10_1.fuz" << "bardsongs__00074a50_1.fuz";
    }
    QDirIterator it(skyrimShoutsRealPath + "skyrim.esm/"+genderBards+raceBards+"/", argumentsDelete, QDir::Files);
    while(it.hasNext()) {
        QFile file(it.next());
        file.remove();
    }
}

void MainWindow::on_resetBardsBtn_clicked()
{
    genderBards = "";
    raceBards = "";
    QButtonGroup *gendersBards = ui->genderBardsButtonGroup;
    QAbstractButton* checkedGendersBards = gendersBards->checkedButton();
    if (checkedGendersBards) {
        gendersBards->setExclusive(false);
        checkedGendersBards->setChecked(false);
        gendersBards->setExclusive(true);
        ui->bardsSongsGroupBox->setDisabled(true);
    }
}

void MainWindow::ShowLyrics()
{
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    QString song = buttonSender->accessibleName();
    QString translatedSong = buttonSender->text();
    QMessageBox *lyricsBox = new QMessageBox;
    QString lyricsText;
    lyricsBox->setTextFormat(Qt::RichText);
    lyricsBox->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
    lyricsBox->setWindowTitle(tr("Lyrics for %1").arg(translatedSong));
    QString val;
    QJsonValue fileValue;
    QFile file(qApp->applicationDirPath() + "/" + "lyrics_"+language+".json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject lyrics = d.object();
    QJsonValue json = lyrics.value(QString(song));
    QJsonObject lyricsSong = json.toObject();
    foreach(const QString &key, lyricsSong.keys()) {
        QJsonValue value = lyricsSong.value(key);
        lyricsText += value.toString();
    }
    lyricsBox->setText(lyricsText);
    lyricsBox->exec();
}

void MainWindow::on_actionFrench_triggered()
{
    language = "fr";
    translator.load(":/translation_fr.qm");
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
    RetranslateCustomUi();
    this->setWindowTitle(version);
    QSettings settings("SkyrimMusicEditor", "SkyrimMusicEditor");
    settings.setValue("app/language", "fr");
}

void MainWindow::on_actionRussian_triggered()
{
    language = "ru";
    translator.load(":/translation_ru.qm");
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
    RetranslateCustomUi();
    this->setWindowTitle(version);
    QSettings settings("SkyrimMusicEditor", "SkyrimMusicEditor");
    settings.setValue("app/language", "ru");
}

void MainWindow::on_actionEnglish_triggered()
{
    language = "en";
    translator.load(":/translation_en.qm");
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
    RetranslateCustomUi();
    this->setWindowTitle(version);
    QSettings settings("SkyrimMusicEditor", "SkyrimMusicEditor");
    settings.setValue("app/language", "en");
}

void MainWindow::DetectSkyrimInstallation()
{
    registryVanilla = new QSettings(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Bethesda Softworks\Skyrim)", QSettings::NativeFormat, this);
    registrySpecialEdition = new QSettings(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Bethesda Softworks\Skyrim Special Edition)", QSettings::NativeFormat, this);
    skyrimVanillaDir = registryVanilla->value("Installed Path").toString();
    skyrimSpecialEditionDir = registrySpecialEdition->value("Installed Path").toString();
    skyrimVanillaRealPath = skyrimVanillaDir.replace("\\", "/");
    skyrimSpecialEditionRealPath = skyrimSpecialEditionDir.replace("\\", "/");
}
