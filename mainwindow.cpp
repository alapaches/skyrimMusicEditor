#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "trackdelegate.h"
#include "shoutrplacedialog.h"
#include "audioconverter.h"
#include "musiccatalog.h"

#include <QAudioOutput>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QMessageBox>
#include <QPointer>
#include <QProcess>
#include <QSettings>
#include <QStatusBar>
#include <QUrl>
#include <QVBoxLayout>

namespace {

const QString kFuzSuffix   = QStringLiteral("_1.fuz");
const QString kBardPrefix  = QStringLiteral("bardsongs__000");

// Roles des items de la sidebar
const int RoleTab       = Qt::UserRole;       // index dans tabWidget (-1 = en-tete)
const int RolePage      = Qt::UserRole + 1;   // index dans music/voiceTabWidget
const int RoleCount     = Qt::UserRole + 2;   // nombre d'elements
const int RoleCategory  = Qt::UserRole + 3;   // cle de categorie
const int RoleReplaced  = Qt::UserRole + 4;   // nombre d'elements remplaces

const QColor kColorReplaced("#7bc98a");
const QColor kColorPartial("#d4a94a");
const QColor kColorOriginal("#6e6e78");

QIcon iconFrom(const QString &resource)
{
    QIcon icon;
    icon.addPixmap(QPixmap(resource));
    return icon;
}

QTableWidgetItem *statusItem(const QString &text, const QColor &color)
{
    auto *item = new QTableWidgetItem(text);
    item->setFlags(Qt::ItemIsEnabled);
    item->setForeground(color);
    return item;
}

} // namespace

// ============================================================================
//  Construction
// ============================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    language = QSettings("SkyrimMusicEditor", "SkyrimMusicEditor")
                   .value("app/language", "en").toString();
    if (language != "en" && translator.load(":/translation_" + language + ".qm")) {
        qApp->installTranslator(&translator);
    }

    player = new QMediaPlayer(this);
    player->setAudioOutput(new QAudioOutput(this));
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onPlayerStatusChanged);

    ui->setupUi(this);
    setWindowTitle(version);

    playIcon        = iconFrom(":/images/images/ico/play.ico");
    playCurrentIcon = iconFrom(":/images/images/ico/play_current.ico");
    stopIcon        = iconFrom(":/images/images/ico/stop.ico");
    browseIcon      = iconFrom(":/images/images/ico/folder.ico");
    launchGameIcon  = iconFrom(":/images/images/ico/mainIcon.ico");
    ui->launchSkyrimButton->setIcon(launchGameIcon);
    ui->stopMusicButton->setIcon(stopIcon);
    ui->pushButton->setIcon(browseIcon);

    // Anciens panneaux du .ui, remplaces par la barre de statut et le bandeau
    ui->skyrimPathGroupBox->hide();
    ui->toolsGroupBox->hide();

    musicCategories = {
        {"combat",     "Combat",     ui->tab_combat,     ui->combatTableWidget,     ui->btnClearCombat},
        {"dungeon",    "Dungeon",    ui->tab_dungeon,    ui->dungeonTableWidget,    ui->btnClearDungeon},
        {"dread",      "Dread",      ui->tab_dread,      ui->dreadTableWidget,      ui->btnClearDread},
        {"explore",    "Explore",    ui->tab_explore,    ui->exploreTableWidget,    ui->btnClearExplore},
        {"other",      "Other",      ui->tab_other,      ui->otherTableWidget,      ui->btnClearOther},
        {"reveal",     "Reveal",     ui->tab_reveal,     ui->revealTableWidget,     ui->btnClearReveal},
        {"reward",     "Reward",     ui->tab_reward,     ui->rewardTableWidget,     ui->btnClearReward},
        {"special",    "Special",    ui->tab_special,    ui->specialTableWidget,    ui->btnClearSpecial},
        {"stinger",    "Stinger",    ui->tab_stinger,    ui->stingerTableWidget,    ui->btnClearStinger},
        {"tavern",     "Tavern",     ui->tab_tavern,     ui->tavernTableWidget,     ui->btnClearTavern},
        {"town",       "Town",       ui->tab_town,       ui->townTableWidget,       ui->btnClearTown},
        {"dlc01",      "Dawnguard",  ui->tab_dawnguard,  ui->dawnguardTableWidget,  ui->btnClearDawnguard},
        {"dragonborn", "Dragonborn", ui->tab_dragonborn, ui->dragonbornTableWidget, ui->btnClearDragonborn},
    };

    SetupSidebar();
    SetupMusicPages();
    SetupShoutsScreen();
    SetupBardsScreen();

    // --- Bandeau d'en-tete : titre a gauche, actions a droite ---
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

    // --- Assemblage : sidebar | (header + contenu) ---
    auto *rightSide = new QVBoxLayout();
    rightSide->setContentsMargins(0, 0, 0, 0);
    rightSide->setSpacing(0);
    rightSide->addWidget(headerWidget);
    rightSide->addWidget(ui->tabWidget, 1);

    for (QWidget *w : {static_cast<QWidget*>(ui->tabWidget), static_cast<QWidget*>(ui->musicTabWidget),
                       static_cast<QWidget*>(ui->voiceTabWidget), ui->musicTab, ui->voiceTab}) {
        w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    auto *mainLayout = new QHBoxLayout(centralWidget());
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(categoryList);
    mainLayout->addLayout(rightSide, 1);

    categoryList->setFixedWidth(180);
    setMinimumSize(1000, 640);
    resize(1080, 700);

    // --- Barre de statut : chemin + version detectee ---
    auto *pathLabel = new QLabel(this);
    pathLabel->setObjectName("pathStatus");
    pathLabel->setMinimumWidth(0);
    pathLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    auto *versionLabel = new QLabel(this);
    versionLabel->setObjectName("versionStatus");
    statusBar()->addWidget(pathLabel, 1);
    statusBar()->addPermanentWidget(versionLabel);

    ui->tabWidget->setCurrentIndex(0);
    ui->musicTabWidget->setCurrentIndex(0);

    SelectInstallation();

    QString missingTools;
    if (!AudioConverter::toolsAvailable(&missingTools)) {
        QMessageBox::warning(this, tr("Missing files"),
                             tr("The following files were not found next to the application:\n%1\n\n"
                                "Replacing music, shouts and bard songs will not work.")
                                 .arg(missingTools));
    }

    RefreshMusicStatus();
    RefreshShoutsTable();
    RefreshBardsTable();
    categoryList->setCurrentRow(1);
    UpdateCategorySubtitle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetupMusicPages()
{
    auto *musicTabLayout = new QVBoxLayout(ui->musicTab);
    musicTabLayout->setContentsMargins(0, 0, 0, 0);
    musicTabLayout->addWidget(ui->musicTabWidget);

    auto *voiceTabLayout = new QVBoxLayout(ui->voiceTab);
    voiceTabLayout->setContentsMargins(0, 0, 0, 0);
    voiceTabLayout->addWidget(ui->voiceTabWidget);

    ui->shoutTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->shoutGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    for (const MusicCategory &cat : musicCategories) {
        // Mise en page : la table s'etire, bouton Restore en bas a droite
        auto *pageLayout = new QVBoxLayout(cat.page);
        pageLayout->setContentsMargins(12, 0, 12, 12);
        pageLayout->setSpacing(10);
        pageLayout->addWidget(cat.table);
        auto *btnRow = new QHBoxLayout();
        btnRow->addStretch();
        btnRow->addWidget(cat.restoreBtn);
        pageLayout->addLayout(btnRow);
        cat.page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // Table : lignes deux niveaux, pas de grille, colonne Status ajoutee
        QTableWidget *t = cat.table;
        t->insertColumn(2);
        t->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        t->setMinimumSize(0, 0);
        t->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        t->setEditTriggers(QAbstractItemView::NoEditTriggers);
        t->setAlternatingRowColors(false);
        t->verticalHeader()->setVisible(false);
        t->verticalHeader()->setDefaultSectionSize(44);
        t->setShowGrid(false);
        t->setSelectionBehavior(QAbstractItemView::SelectRows);
        t->setSelectionMode(QAbstractItemView::SingleSelection);
        t->setItemDelegate(new TrackDelegate(this));
        t->hideColumn(1);
        t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        for (int col = 2; col <= 5; ++col) {
            t->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Fixed);
        }
        t->setColumnWidth(2, 120);
        t->setColumnWidth(3, 70);
        t->setColumnWidth(4, 70);
        t->setColumnWidth(5, 70);
        t->setHorizontalHeaderLabels({tr("Track"), "", tr("Status"), tr("Replace"), tr("Original"), tr("Current")});
        t->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        t->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        // Boutons Remplacer / Original / Actuel, identiques pour chaque categorie
        for (int row = 0; row < t->rowCount(); ++row) {
            auto *browseBtn      = new QPushButton(browseIcon, QString(), t);
            auto *playBtn        = new QPushButton(playIcon, QString(), t);
            auto *playCurrentBtn = new QPushButton(playCurrentIcon, QString(), t);
            browseBtn->setToolTip(tr("Replace this track"));
            playBtn->setToolTip(tr("Play the original track"));
            playCurrentBtn->setToolTip(tr("Play the track currently in the game"));
            t->setCellWidget(row, 3, browseBtn);
            t->setCellWidget(row, 4, playBtn);
            t->setCellWidget(row, 5, playCurrentBtn);

            connect(browseBtn,      &QPushButton::clicked, this, [this, cat, row] { ReplaceTrack(cat, row); });
            connect(playBtn,        &QPushButton::clicked, this, [this, cat, row] { PlayOriginal(cat, row); });
            connect(playCurrentBtn, &QPushButton::clicked, this, [this, cat, row] { PlayCurrent(cat, row); });
        }

        connect(cat.restoreBtn, &QPushButton::clicked, this, [this, cat] { RestoreCategory(cat); });
    }
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

    auto addHeader = [this](const QString &label) {
        auto *item = new QListWidgetItem(label.toUpper(), categoryList);
        item->setFlags(Qt::ItemIsEnabled);
        item->setData(RoleTab, -1);
        item->setData(RolePage, -1);
        item->setData(RoleCount, 0);
        QFont headerFont = item->font();
        headerFont.setBold(true);
        if (headerFont.pointSize() > 0) headerFont.setPointSize(headerFont.pointSize() + 1);
        item->setFont(headerFont);
        item->setForeground(QColor("#7ecbff"));
    };
    auto addEntry = [this](const QString &label, int tab, int page, int count, const QString &key) {
        auto *item = new QListWidgetItem(label, categoryList);
        item->setData(RoleTab, tab);
        item->setData(RolePage, page);
        item->setData(RoleCount, count);
        item->setData(RoleCategory, key);
        item->setData(RoleReplaced, 0);
        if (tab == 0 && count > 0) item->setToolTip(tr("%1 tracks").arg(count));
    };

    addHeader("Music");
    for (int i = 0; i < musicCategories.size(); ++i) {
        const MusicCategory &cat = musicCategories[i];
        addEntry(cat.label, 0, i, cat.table->rowCount(), cat.key);
    }
    addHeader("Voice");
    addEntry(tr("Shouts"), 1, 0, 0, "shouts");
    addEntry(tr("Bards"),  1, 1, 0, "bards");

    categoryList->setItemDelegate(new CategoryDelegate(categoryList));

    categoryTitle = new QLabel(this);
    categoryTitle->setObjectName("categoryTitle");
    categorySubtitle = new QLabel(this);
    categorySubtitle->setObjectName("categorySubtitle");

    connect(categoryList, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem *current, QListWidgetItem *) {
        if (!current) return;
        const int tab = current->data(RoleTab).toInt();
        if (tab < 0) return;

        ui->tabWidget->setCurrentIndex(tab);
        (tab == 0 ? ui->musicTabWidget : ui->voiceTabWidget)->setCurrentIndex(current->data(RolePage).toInt());

        // Comparaison sur la cle et non sur le texte : le texte est traduit.
        const QString key = current->data(RoleCategory).toString();
        if (voiceSelectors) voiceSelectors->setVisible(key == "shouts");
        if (bardSelectors)  bardSelectors->setVisible(key == "bards");
        UpdateCategorySubtitle();
    });
}

void MainWindow::UpdateCategorySubtitle()
{
    QListWidgetItem *current = categoryList ? categoryList->currentItem() : nullptr;
    if (!current || current->data(RoleTab).toInt() < 0) return;

    categoryTitle->setText(current->text());

    const QString key = current->data(RoleCategory).toString();
    const int count    = current->data(RoleCount).toInt();
    const int replaced = current->data(RoleReplaced).toInt();
    if (key == "shouts") {
        categorySubtitle->setText(tr("%1 shouts · %2 replaced").arg(count).arg(replaced));
    } else if (key == "bards") {
        categorySubtitle->setText(tr("%1 songs · %2 replaced").arg(count).arg(replaced));
    } else if (count > 0) {
        categorySubtitle->setText(tr("%1 tracks · %2 replaced").arg(count).arg(replaced));
    } else {
        categorySubtitle->clear();
    }
}

// ============================================================================
//  Installation du jeu
// ============================================================================

QString MainWindow::PromptForGameDir(const QString &versionName, const QString &startDir)
{
    while (true) {
        const QString dir = QFileDialog::getExistingDirectory(
            this, tr("Select %1 folder").arg(versionName), startDir,
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (dir.isEmpty()) return QString();
        if (SkyrimInstall::isValidGameDir(dir)) return dir;

        QMessageBox::warning(this, tr("Invalid folder"),
                             tr("\"%1\" does not contain a Data folder.\n"
                                "Please select the game installation folder.").arg(dir));
    }
}

void MainWindow::SelectInstallation()
{
    using Edition = SkyrimInstall::Edition;
    const QString vanillaPath = SkyrimInstall::detectFromRegistry(Edition::Vanilla);
    const QString sePath      = SkyrimInstall::detectFromRegistry(Edition::SpecialEdition);

    // Version a utiliser
    Edition edition = Edition::None;
    if (!vanillaPath.isEmpty() && !sePath.isEmpty()) {
        QMessageBox box(this);
        box.setWindowTitle(tr("Attention"));
        box.setIcon(QMessageBox::Question);
        box.setText(tr("Both Skyrim AND Skyrim Special Edition were detected. Please choose the version you want to use with the 2 buttons below."));
        QAbstractButton *vanillaBtn = box.addButton(tr("Skyrim"), QMessageBox::ActionRole);
        box.addButton(tr("Skyrim Special Edition"), QMessageBox::ActionRole);
        box.exec();
        edition = (box.clickedButton() == vanillaBtn) ? Edition::Vanilla : Edition::SpecialEdition;
    } else if (!vanillaPath.isEmpty()) {
        edition = Edition::Vanilla;
    } else if (!sePath.isEmpty()) {
        edition = Edition::SpecialEdition;
    }

    if (edition != Edition::None) {
        // Version detectee : confirmer ou changer le chemin
        const QString detected = (edition == Edition::Vanilla) ? vanillaPath : sePath;
        const QString name = SkyrimInstall::editionName(edition);

        QMessageBox confirmBox(this);
        confirmBox.setWindowTitle(tr("Verify %1 path").arg(name));
        confirmBox.setIcon(QMessageBox::Question);
        confirmBox.setText(tr("Detected %1 path:\n%2\n\nIs this correct?").arg(name, detected));
        confirmBox.addButton(tr("Confirm"), QMessageBox::YesRole);
        QAbstractButton *changeButton = confirmBox.addButton(tr("Change..."), QMessageBox::ActionRole);
        confirmBox.exec();

        QString path = detected;
        if (confirmBox.clickedButton() == changeButton) {
            const QString chosen = PromptForGameDir(name, detected);
            if (!chosen.isEmpty()) path = chosen;
        }
        install.set(path, edition);
    } else {
        // Rien de detecte : selection manuelle du dossier puis de la version
        QMessageBox::information(this, tr("Attention"),
                                 tr("None version of the game was detected. You may now select your game directory by clicking the button below"));
        const QString dir = PromptForGameDir(tr("Skyrim"));
        if (!dir.isEmpty()) {
            QMessageBox versionBox(this);
            versionBox.setWindowTitle(tr("Attention"));
            versionBox.setIcon(QMessageBox::Question);
            versionBox.setText(tr("Is this path for Skyrim or Skyrim Special Edition ?"));
            QAbstractButton *vanillaBtn = versionBox.addButton(tr("Skyrim"), QMessageBox::ActionRole);
            versionBox.addButton(tr("Skyrim Special Edition"), QMessageBox::ActionRole);
            versionBox.exec();
            install.set(dir, versionBox.clickedButton() == vanillaBtn ? Edition::Vanilla
                                                                       : Edition::SpecialEdition);
        }
    }

    UpdateStatusBar();
}

// Verifie qu'un dossier de jeu valide est selectionne avant toute ecriture.
bool MainWindow::EnsureInstallation()
{
    if (install.isValid()) return true;

    const auto answer = QMessageBox::warning(
        this, tr("Game folder not set"),
        tr("No valid Skyrim folder is selected. Do you want to select it now?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (answer == QMessageBox::Yes) {
        SelectInstallation();
        RefreshMusicStatus();
        RefreshShoutsTable();
        RefreshBardsTable();
    }
    return install.isValid();
}

void MainWindow::UpdateStatusBar()
{
    if (auto *lbl = statusBar()->findChild<QLabel*>("pathStatus")) {
        lbl->setText(install.isValid() ? install.root() : tr("No game folder selected"));
    }
    if (auto *lbl = statusBar()->findChild<QLabel*>("versionStatus")) {
        switch (install.edition()) {
        case SkyrimInstall::Edition::SpecialEdition: lbl->setText(tr("SE detected")); break;
        case SkyrimInstall::Edition::Vanilla:        lbl->setText(tr("Skyrim detected")); break;
        case SkyrimInstall::Edition::None:           lbl->setText(QString()); break;
        }
    }
}

// ============================================================================
//  Musique
// ============================================================================

QString MainWindow::MusicFilePath(const MusicCategory &cat, int row) const
{
    const QTableWidgetItem *fileItem = cat.table->item(row, 1);
    if (!fileItem) return QString();
    return install.musicPath() + MusicCatalog::gameRelativePath(cat.key, fileItem->text());
}

void MainWindow::ReplaceTrack(const MusicCategory &cat, int row)
{
    if (!EnsureInstallation()) return;

    const QString source = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                        tr("Audio (*.wav *.mp3 *.wma)"));
    if (source.isEmpty()) return;

    const QString fileName = cat.table->item(row, 1)->text();
    const QString dest = MusicFilePath(cat, row);
    const QString wav  = AudioConverter::workFile(fileName + ".wav");
    const QString xwm  = AudioConverter::workFile(fileName + ".xwm");

    StopPlayback();

    QPointer<QMessageBox> waitBox = new QMessageBox(QMessageBox::Information, tr("Music Replacement"),
                                    tr("Encoding, please wait..."), QMessageBox::NoButton, this);
    waitBox->setAttribute(Qt::WA_DeleteOnClose);
    waitBox->open();

    auto cleanup = [wav, xwm] {
        QFile::remove(wav);
        QFile::remove(xwm);
    };

    AudioConverter::run(this,
        { AudioConverter::toWav(source, wav), AudioConverter::wavToXwm(wav, xwm) },
        [this, waitBox, wav, xwm, dest, cleanup] {
            if (waitBox) waitBox->close();

            QMessageBox preview(this);
            preview.setIcon(QMessageBox::Question);
            preview.setWindowTitle(tr("Music Replacement"));
            preview.setText(tr("If you want to get a preview of your new file, you may click \"Preview\". Otherwise, you can just click \"Replace\""));
            QAbstractButton *previewButton = preview.addButton(tr("Preview"), QMessageBox::ActionRole);
            QAbstractButton *replaceButton = preview.addButton(tr("Replace"), QMessageBox::AcceptRole);
            preview.addButton(tr("Cancel"), QMessageBox::RejectRole);

            // QMessageBox se ferme a chaque clic : on la rouvre apres "Apercu".
            do {
                preview.exec();
                if (preview.clickedButton() == previewButton) PlayFile(wav);
            } while (preview.clickedButton() == previewButton);

            StopPlayback();
            if (preview.clickedButton() == replaceButton) {
                if (AudioConverter::replaceFile(xwm, dest)) {
                    QMessageBox::information(this, tr("Music Replacement"), tr("Success !"));
                } else {
                    ShowError(tr("Music Replacement"), tr("Could not write the file:\n%1").arg(dest));
                }
                RefreshMusicStatus();
            }
            cleanup();
        },
        [this, waitBox, cleanup](const QString &error) {
            if (waitBox) waitBox->close();
            cleanup();
            ShowError(tr("Music Replacement"), error);
        });
}

void MainWindow::PlayOriginal(const MusicCategory &cat, int row)
{
    const QString name = cat.table->item(row, 0)->text();
    const QString path = AudioConverter::toolsDir() + "/audio/"
                         + MusicCatalog::previewRelativePath(cat.key, name);
    if (!QFile::exists(path)) {
        statusBar()->showMessage(tr("No preview available for \"%1\"").arg(name), 4000);
        return;
    }
    PlayFile(path);
}

void MainWindow::PlayCurrent(const MusicCategory &cat, int row)
{
    if (!install.isValid()) {
        PlayOriginal(cat, row);
        return;
    }

    const QString gameFile = MusicFilePath(cat, row);
    if (!QFile::exists(gameFile)) {
        // Rien de remplace : la piste actuelle est l'originale.
        PlayOriginal(cat, row);
        return;
    }

    StopPlayback();   // libere le .wav precedent avant de l'ecraser
    const QString wav = AudioConverter::workFile("current_" + QFileInfo(gameFile).completeBaseName() + ".wav");
    AudioConverter::run(this, { AudioConverter::xwmToWav(gameFile, wav) },
        [this, wav] { PlayFile(wav); },
        [this](const QString &error) { ShowError(tr("Playback"), error); });
}

void MainWindow::RestoreCategory(const MusicCategory &cat)
{
    if (!EnsureInstallation()) return;

    const auto answer = QMessageBox::warning(
        this, tr("ATTENTION"),
        tr("Are you sure to restore \"%1\" musics to original ?").arg(cat.label),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer != QMessageBox::Yes) return;

    StopPlayback();

    // Ne supprime que les pistes connues de l'application : les autres
    // fichiers eventuellement presents (autres mods) sont conserves.
    QStringList failed;
    for (int row = 0; row < cat.table->rowCount(); ++row) {
        const QString path = MusicFilePath(cat, row);
        if (QFile::exists(path) && !QFile::remove(path)) {
            failed << path;
        }
    }
    RefreshMusicStatus();

    if (!failed.isEmpty()) {
        ShowError(tr("Restore"), tr("Could not remove:\n%1").arg(failed.join('\n')));
    }
}

void MainWindow::RefreshMusicStatus()
{
    for (const MusicCategory &cat : musicCategories) {
        int replaced = 0;
        for (int row = 0; row < cat.table->rowCount(); ++row) {
            const bool isReplaced = install.isValid() && QFile::exists(MusicFilePath(cat, row));
            if (isReplaced) ++replaced;

            cat.table->setItem(row, 2, statusItem(isReplaced ? tr("Replaced") : tr("Original"),
                                                  isReplaced ? kColorReplaced : kColorOriginal));
            if (auto *nameItem = cat.table->item(row, 0)) {
                nameItem->setData(ReplacedRole, isReplaced);
            }
        }

        for (int i = 0; i < categoryList->count(); ++i) {
            QListWidgetItem *it = categoryList->item(i);
            if (it->data(RoleCategory).toString() == cat.key) {
                it->setData(RoleReplaced, replaced);
                break;
            }
        }
    }
    UpdateCategorySubtitle();
}

// ============================================================================
//  Voix : encodage commun aux cris et aux bardes
// ============================================================================

void MainWindow::EncodeVoiceFiles(const QStringList &sources, const QStringList &fuzNames,
                                  const QString &destFolder, const QString &title,
                                  const QString &successText, std::function<void()> onDone)
{
    AudioConverter::Steps steps;
    QStringList workFiles;
    for (int i = 0; i < sources.size(); ++i) {
        const QString base = QFileInfo(fuzNames[i]).completeBaseName();   // "xxx_1"
        const QString wav = AudioConverter::workFile(base + ".wav");
        const QString xwm = AudioConverter::workFile(base + ".xwm");
        const QString fuz = AudioConverter::workFile(fuzNames[i]);
        steps << AudioConverter::toWav(sources[i], wav)
              << AudioConverter::wavToXwm(wav, xwm)
              << AudioConverter::xwmToFuz(xwm, fuz);
        workFiles << wav << xwm << fuz;
    }

    QPointer<QMessageBox> waitBox = new QMessageBox(QMessageBox::Information, title,
                                    tr("Encoding %1 files, please wait...").arg(sources.size()),
                                    QMessageBox::NoButton, this);
    waitBox->setAttribute(Qt::WA_DeleteOnClose);
    waitBox->open();

    auto cleanup = [workFiles] {
        for (const QString &f : workFiles) QFile::remove(f);
    };

    AudioConverter::run(this, steps,
        [=] {
            QStringList failed;
            for (const QString &fuzName : fuzNames) {
                if (!AudioConverter::replaceFile(AudioConverter::workFile(fuzName), destFolder + fuzName)) {
                    failed << destFolder + fuzName;
                }
            }
            cleanup();
            if (waitBox) waitBox->close();
            if (failed.isEmpty()) {
                QMessageBox::information(this, title, successText);
            } else {
                ShowError(title, tr("Could not write:\n%1").arg(failed.join('\n')));
            }
            if (onDone) onDone();
        },
        [=](const QString &error) {
            cleanup();
            if (waitBox) waitBox->close();
            ShowError(title, error);
        });
}

// ============================================================================
//  Cris
// ============================================================================

QString MainWindow::ShoutFolder(const QString &subDir) const
{
    return install.voicePath() + subDir + ".esm/" + gender + race + "/";
}

QStringList MainWindow::ShoutFileNames(const ShoutEntry &shout) const
{
    const QJsonObject root = LoadJson("shouts.json");
    const QJsonObject item = root.value(shout.jsonKey).toObject();
    const QJsonObject prefixes = root.value(shout.subDir == "dragonborn" ? "Files_Dragonborn" : "Files").toObject();

    QStringList names;
    for (int i = 0; i < 4; ++i) {
        const QString idx = QString::number(i);
        const QString id = item.value(idx).toString();
        if (id.isEmpty()) return {};   // shouts.json manquant ou incomplet
        names << prefixes.value(idx).toString() + id + kFuzSuffix;
    }
    return names;
}

void MainWindow::RefreshShoutsTable()
{
    if (!shoutsTable) return;

    shoutsTable->setRowCount(shoutEntries.size());
    int replacedCount = 0;

    for (int row = 0; row < shoutEntries.size(); ++row) {
        const ShoutEntry &e = shoutEntries[row];

        // Colonne 1 (masquee) : sous-titre lu par le delegate
        QString origin = tr("Vanilla");
        if (e.subDir == "dawnguard")       origin = tr("Dawnguard");
        else if (e.subDir == "dragonborn") origin = tr("Dragonborn");
        shoutsTable->setItem(row, 1, new QTableWidgetItem(origin + " · " + tr("4 words")));

        int done = 0;
        if (install.isValid()) {
            const QString folder = ShoutFolder(e.subDir);
            for (const QString &f : ShoutFileNames(e)) {
                if (QFile::exists(folder + f)) ++done;
            }
        }

        auto *nameItem = new QTableWidgetItem(e.name);
        nameItem->setData(ReplacedRole, done == 4);
        shoutsTable->setItem(row, 0, nameItem);

        if (done == 4) {
            ++replacedCount;
            shoutsTable->setItem(row, 2, statusItem(tr("Replaced"), kColorReplaced));
        } else if (done > 0) {
            shoutsTable->setItem(row, 2, statusItem(tr("%1 of 4 selected").arg(done), kColorPartial));
        } else {
            shoutsTable->setItem(row, 2, statusItem(tr("Original"), kColorOriginal));
        }

        if (!shoutsTable->cellWidget(row, 3)) {
            auto *actionWidget = new QWidget(shoutsTable);
            auto *actionLayout = new QHBoxLayout(actionWidget);
            actionLayout->setContentsMargins(0, 0, 8, 0);
            actionLayout->addStretch();
            auto *replaceBtn = new QPushButton(browseIcon, QString(), actionWidget);
            replaceBtn->setToolTip(tr("Replace this shout"));
            actionLayout->addWidget(replaceBtn);
            shoutsTable->setCellWidget(row, 3, actionWidget);
            connect(replaceBtn, &QPushButton::clicked, this, [this, row] {
                ReplaceShoutFor(shoutEntries[row]);
            });
        }
    }

    for (int i = 0; i < categoryList->count(); ++i) {
        QListWidgetItem *it = categoryList->item(i);
        if (it->data(RoleCategory).toString() == "shouts") {
            it->setData(RoleCount, shoutEntries.size());
            it->setData(RoleReplaced, replacedCount);
        }
    }
    UpdateCategorySubtitle();
}

void MainWindow::ReplaceShoutFor(const ShoutEntry &shout)
{
    if (!EnsureInstallation()) return;
    if (gender.isEmpty() || race.isEmpty()) {
        QMessageBox::warning(this, tr("Voice not selected"), tr("Select a gender and a race first."));
        return;
    }

    const QStringList fuzNames = ShoutFileNames(shout);
    if (fuzNames.isEmpty()) {
        ShowError(tr("Error"), tr("Could not open shouts.json"));
        return;
    }

    const QString voiceInfo = genderCombo->currentText() + " " + raceCombo->currentText();
    ShoutReplaceDialog dlg(shout.name, voiceInfo, 4, this);
    if (dlg.exec() != QDialog::Accepted || !dlg.allSelected()) return;

    EncodeVoiceFiles(dlg.selectedFiles(), fuzNames, ShoutFolder(shout.subDir),
                     tr("Replace shout"), tr("Shout replaced successfully."),
                     [this] { RefreshShoutsTable(); });
}

void MainWindow::SetupShoutsScreen()
{
    // Les donnees des cris viennent des anciens boutons du .ui :
    //   text() -> libelle, accessibleName() -> sous-dossier, accessibleDescription() -> cle JSON
    const QList<QPushButton*> sourceButtons = {
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
        shoutEntries.append({btn->text(), btn->accessibleName(), btn->accessibleDescription()});
    }

    ui->genderGroupBox->hide();
    ui->raceGroupBox->hide();
    ui->setRaceBtn->hide();
    ui->pushButton_2->hide();
    ui->shoutGroupBox->hide();

    // --- Selecteurs de voix (affiches dans le bandeau) ---
    auto *voiceLabel = new QLabel(tr("Voice"), this);
    voiceLabel->setObjectName("fieldLabel");

    genderCombo = new QComboBox(this);
    genderCombo->setObjectName("voiceCombo");
    for (QRadioButton *rb : {ui->maleRadioButton, ui->femaleRadioButton}) {
        genderCombo->addItem(rb->text(), rb->accessibleName());
    }

    raceCombo = new QComboBox(this);
    raceCombo->setObjectName("voiceCombo");
    for (QRadioButton *rb : {ui->argonianRadioButton, ui->elfhaughtyRadioButton, ui->kahjiitRadioButton,
                             ui->nordicRadioButton, ui->orcRadioButton}) {
        raceCombo->addItem(rb->text(), rb->accessibleName());
    }

    voiceSelectors = new QWidget(this);
    auto *voiceLayout = new QHBoxLayout(voiceSelectors);
    voiceLayout->setContentsMargins(0, 0, 0, 0);
    voiceLayout->setSpacing(8);
    voiceLayout->addWidget(voiceLabel);
    voiceLayout->addWidget(genderCombo);
    voiceLayout->addWidget(raceCombo);
    voiceSelectors->hide();

    // --- Table des cris ---
    shoutsTable = new QTableWidget(ui->shoutTab);
    shoutsTable->setObjectName("shoutsTable");
    shoutsTable->setColumnCount(4);
    shoutsTable->setHorizontalHeaderLabels({tr("Shout Name"), "", tr("Status"), tr("Actions")});
    shoutsTable->verticalHeader()->setVisible(false);
    shoutsTable->verticalHeader()->setDefaultSectionSize(44);
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
    shoutsTable->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    shoutsTable->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    shoutsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // --- Pied de page : restauration pour la voix courante ---
    auto *footerWidget = new QWidget(ui->shoutTab);
    footerWidget->setObjectName("footerBar");
    footerWidget->setFixedHeight(48);
    auto *footerLayout = new QHBoxLayout(footerWidget);
    footerLayout->setContentsMargins(20, 8, 20, 8);
    footerLayout->addStretch();
    auto *restoreAllBtn = new QPushButton(tr("Restore all for this voice"), footerWidget);
    restoreAllBtn->setObjectName("restoreAllShoutsBtn");
    footerLayout->addWidget(restoreAllBtn);

    auto *pageLayout = new QVBoxLayout(ui->shoutTab);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);
    pageLayout->addWidget(shoutsTable, 1);
    pageLayout->addWidget(footerWidget);
    ui->shoutTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(genderCombo, &QComboBox::currentIndexChanged, this, [this](int) {
        gender = genderCombo->currentData().toString();
        RefreshShoutsTable();
    });
    connect(raceCombo, &QComboBox::currentIndexChanged, this, [this](int) {
        race = raceCombo->currentData().toString();
        RefreshShoutsTable();
    });
    connect(restoreAllBtn, &QPushButton::clicked, this, [this] {
        if (!EnsureInstallation()) return;
        const auto answer = QMessageBox::warning(
            this, tr("Restore shouts"),
            tr("Restore all original shouts for %1 %2?").arg(genderCombo->currentText(), raceCombo->currentText()),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer != QMessageBox::Yes) return;

        // Ne supprime que les fichiers des cris geres par l'application
        for (const ShoutEntry &e : shoutEntries) {
            const QString folder = ShoutFolder(e.subDir);
            for (const QString &f : ShoutFileNames(e)) QFile::remove(folder + f);
        }
        RefreshShoutsTable();
    });

    gender = genderCombo->currentData().toString();
    race   = raceCombo->currentData().toString();
}

// ============================================================================
//  Chants des bardes
// ============================================================================

int MainWindow::BardFileCount(const QString &songKey)
{
    if (songKey == "Ragnar the Red") return 6;
    if (songKey == "Tale of the Tongues" || songKey == "The Dragonborn Comes") return 8;
    return 5;
}

QString MainWindow::BardFolder() const
{
    return install.voicePath() + "skyrim.esm/" + genderBards + raceBards + "/";
}

QStringList MainWindow::BardFileNames(const BardEntry &song) const
{
    const QJsonObject item = LoadJson("bards.json").value(song.songKey).toObject();
    QStringList names;
    for (int i = 0; i < song.fileCount; ++i) {
        const QString id = item.value(QString::number(i)).toString();
        if (id.isEmpty()) return {};   // bards.json manquant ou incomplet
        names << kBardPrefix + id + kFuzSuffix;
    }
    return names;
}

void MainWindow::RefreshBardsTable()
{
    if (!bardsTable) return;

    bardsTable->setRowCount(bardEntries.size());
    int replacedCount = 0;

    for (int row = 0; row < bardEntries.size(); ++row) {
        const BardEntry &e = bardEntries[row];

        int done = 0;
        if (install.isValid()) {
            const QString folder = BardFolder();
            for (const QString &f : BardFileNames(e)) {
                if (QFile::exists(folder + f)) ++done;
            }
        }
        const bool fullyReplaced = (done == e.fileCount);
        if (fullyReplaced) ++replacedCount;

        auto *nameItem = new QTableWidgetItem(e.name);
        nameItem->setData(ReplacedRole, fullyReplaced);
        bardsTable->setItem(row, 0, nameItem);
        bardsTable->setItem(row, 1, new QTableWidgetItem(tr("%1 files").arg(e.fileCount)));

        if (fullyReplaced) {
            bardsTable->setItem(row, 2, statusItem(tr("Replaced"), kColorReplaced));
        } else if (done > 0) {
            bardsTable->setItem(row, 2, statusItem(tr("%1 of %2 selected").arg(done).arg(e.fileCount), kColorPartial));
        } else {
            bardsTable->setItem(row, 2, statusItem(tr("Original"), kColorOriginal));
        }

        // Les boutons ne sont crees qu'une fois (auparavant recrees a chaque rafraichissement)
        if (!bardsTable->cellWidget(row, 3)) {
            auto *lyricsWidget = new QWidget(bardsTable);
            auto *lyricsLayout = new QHBoxLayout(lyricsWidget);
            lyricsLayout->setContentsMargins(0, 0, 0, 0);
            lyricsLayout->addStretch();
            auto *lyricsBtn = new QPushButton(tr("Lyrics"), lyricsWidget);
            lyricsBtn->setObjectName("linkButton");
            lyricsLayout->addWidget(lyricsBtn);
            lyricsLayout->addStretch();
            bardsTable->setCellWidget(row, 3, lyricsWidget);

            auto *actionWidget = new QWidget(bardsTable);
            auto *actionLayout = new QHBoxLayout(actionWidget);
            actionLayout->setContentsMargins(0, 0, 8, 0);
            actionLayout->addStretch();
            auto *replaceBtn = new QPushButton(browseIcon, QString(), actionWidget);
            replaceBtn->setToolTip(tr("Replace this song"));
            actionLayout->addWidget(replaceBtn);
            bardsTable->setCellWidget(row, 4, actionWidget);

            connect(replaceBtn, &QPushButton::clicked, this, [this, row] {
                ReplaceBardSongFor(bardEntries[row]);
            });
            connect(lyricsBtn, &QPushButton::clicked, this, [this, row] {
                ShowLyricsFor(bardEntries[row].lyricsKey, bardEntries[row].name);
            });
        }
    }

    for (int i = 0; i < categoryList->count(); ++i) {
        QListWidgetItem *it = categoryList->item(i);
        if (it->data(RoleCategory).toString() == "bards") {
            it->setData(RoleCount, bardEntries.size());
            it->setData(RoleReplaced, replacedCount);
        }
    }
    UpdateCategorySubtitle();
}

void MainWindow::ShowLyricsFor(const QString &lyricsKey, const QString &displayName)
{
    const QJsonObject lyricsSong = LoadJson("lyrics_" + language + ".json").value(lyricsKey).toObject();
    if (lyricsSong.isEmpty()) {
        ShowError(tr("Error"), tr("Could not open the lyrics file."));
        return;
    }

    QString lyricsText;
    for (const QString &key : lyricsSong.keys()) {
        lyricsText += lyricsSong.value(key).toString();
    }

    QMessageBox lyricsBox(this);
    lyricsBox.setTextFormat(Qt::RichText);
    lyricsBox.setWindowTitle(tr("Lyrics for %1").arg(displayName));
    lyricsBox.setText(lyricsText);
    lyricsBox.exec();
}

void MainWindow::ReplaceBardSongFor(const BardEntry &song)
{
    if (!EnsureInstallation()) return;
    if (genderBards.isEmpty() || raceBards.isEmpty()) {
        QMessageBox::warning(this, tr("Voice not selected"), tr("Select a gender first."));
        return;
    }

    const QStringList fuzNames = BardFileNames(song);
    if (fuzNames.isEmpty()) {
        ShowError(tr("Error"), tr("Could not open bards.json"));
        return;
    }

    ShoutReplaceDialog dlg(song.songKey, bardGenderCombo->currentText(), song.fileCount, this);
    dlg.setWindowTitle(tr("Replace bard song"));
    if (dlg.exec() != QDialog::Accepted || !dlg.allSelected()) return;

    EncodeVoiceFiles(dlg.selectedFiles(), fuzNames, BardFolder(),
                     tr("Replace bard song"), tr("Bard song replaced successfully."),
                     [this] { RefreshBardsTable(); });
}

void MainWindow::SetupBardsScreen()
{
    // Les donnees des chants viennent des anciens boutons du .ui :
    //   bouton de remplacement -> text() et accessibleName() (cle bards.json)
    //   bouton Lyrics          -> accessibleName() (cle lyrics_xx.json)
    const QList<QPair<QPushButton*, QPushButton*>> sources = {
        {ui->ageOfOppressionBtn,    ui->oppressionLyricsPushButton},
        {ui->ageOfViolenceBtn,      ui->violenceLyricsPushButton},
        {ui->ragnarTheRedBtn,       ui->ragnarLyricsPushButton},
        {ui->taleOfTheTonguesBtn,   ui->tonguesLyricsPushButton},
        {ui->theDragonbornComesBtn, ui->dragonbornComesLyricsPushButton},
    };
    bardEntries.clear();
    for (const auto &s : sources) {
        const QString key = s.first->accessibleName();
        bardEntries.append({s.first->text(), key, s.second->accessibleName(), BardFileCount(key)});
    }

    ui->bardGenderGroupBox->hide();
    ui->resetBardsBtn->hide();
    ui->lyricsGroupBox->hide();
    ui->bardsSongsGroupBox->hide();

    // --- Selecteur de voix (bandeau) ---
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

    // --- Table des chants ---
    bardsTable = new QTableWidget(ui->bardsTab);
    bardsTable->setObjectName("shoutsTable");   // reutilise le meme style
    bardsTable->setColumnCount(5);
    bardsTable->setHorizontalHeaderLabels({tr("Song"), "", tr("Status"), tr("Lyrics"), tr("Actions")});
    bardsTable->verticalHeader()->setVisible(false);
    bardsTable->verticalHeader()->setDefaultSectionSize(44);
    bardsTable->setShowGrid(false);
    bardsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    bardsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    bardsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bardsTable->setItemDelegate(new TrackDelegate(this));
    bardsTable->hideColumn(1);
    bardsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    for (int col = 2; col <= 4; ++col) {
        bardsTable->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Fixed);
    }
    bardsTable->setColumnWidth(2, 150);
    bardsTable->setColumnWidth(3, 80);
    bardsTable->setColumnWidth(4, 70);
    bardsTable->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    bardsTable->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    bardsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bardsTable->setMinimumWidth(0);

    // --- Pied de page ---
    auto *footerWidget = new QWidget(ui->bardsTab);
    footerWidget->setObjectName("footerBar");
    footerWidget->setFixedHeight(48);
    auto *footerLayout = new QHBoxLayout(footerWidget);
    footerLayout->setContentsMargins(20, 8, 20, 8);
    footerLayout->addStretch();
    auto *restoreAllBtn = new QPushButton(tr("Restore all bard songs"), footerWidget);
    restoreAllBtn->setObjectName("restoreAllShoutsBtn");
    footerLayout->addWidget(restoreAllBtn);

    auto *pageLayout = new QVBoxLayout(ui->bardsTab);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);
    pageLayout->addWidget(bardsTable, 1);
    pageLayout->addWidget(footerWidget);
    ui->bardsTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(bardGenderCombo, &QComboBox::currentIndexChanged, this, [this](int) {
        genderBards = bardGenderCombo->currentData().toString();
        raceBards   = (genderBards == "male") ? "nord" : "eventoned";
        RefreshBardsTable();
    });
    connect(restoreAllBtn, &QPushButton::clicked, this, [this] {
        if (!EnsureInstallation()) return;
        const auto answer = QMessageBox::warning(
            this, tr("Restore bard songs"),
            tr("Restore all original bard songs for %1?").arg(bardGenderCombo->currentText()),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer != QMessageBox::Yes) return;

        const QString folder = BardFolder();
        for (const BardEntry &e : bardEntries) {
            for (const QString &f : BardFileNames(e)) QFile::remove(folder + f);
        }
        RefreshBardsTable();
    });

    genderBards = bardGenderCombo->currentData().toString();
    raceBards   = (genderBards == "male") ? "nord" : "eventoned";
}

// ============================================================================
//  Traduction
// ============================================================================

void MainWindow::SetLanguage(const QString &lang)
{
    language = lang;
    // L'anglais est la langue des sources : il suffit de retirer la traduction.
    qApp->removeTranslator(&translator);
    if (lang != "en" && translator.load(":/translation_" + lang + ".qm")) {
        qApp->installTranslator(&translator);
    }
    ui->retranslateUi(this);
    RetranslateCustomUi();
    setWindowTitle(version);
    QSettings("SkyrimMusicEditor", "SkyrimMusicEditor").setValue("app/language", lang);
}

void MainWindow::on_actionFrench_triggered()  { SetLanguage("fr"); }
void MainWindow::on_actionRussian_triggered() { SetLanguage("ru"); }
void MainWindow::on_actionEnglish_triggered() { SetLanguage("en"); }

void MainWindow::RetranslateCustomUi()
{
    // --- Sidebar ---
    for (int i = 0; i < categoryList->count(); ++i) {
        QListWidgetItem *item = categoryList->item(i);
        const QString key = item->data(RoleCategory).toString();
        if (key == "shouts") item->setText(tr("Shouts"));
        if (key == "bards")  item->setText(tr("Bards"));
    }

    // --- Tables musique ---
    for (const MusicCategory &cat : musicCategories) {
        cat.table->setHorizontalHeaderLabels({tr("Track"), "", tr("Status"), tr("Replace"), tr("Original"), tr("Current")});
    }

    // --- Combos de voix : retraduits d'apres leur valeur ---
    auto retranslateCombo = [](QComboBox *combo, const QMap<QString, QString> &labels) {
        if (!combo) return;
        const QSignalBlocker blocker(combo);
        for (int i = 0; i < combo->count(); ++i) {
            const QString val = combo->itemData(i).toString();
            if (labels.contains(val)) combo->setItemText(i, labels.value(val));
        }
    };
    const QMap<QString, QString> genders = {{"male", tr("Male")}, {"female", tr("Female")}};
    retranslateCombo(genderCombo, genders);
    retranslateCombo(bardGenderCombo, genders);
    retranslateCombo(raceCombo, {{"nord", tr("Nord")}, {"argonian", tr("Argonian")},
                                 {"elfhaughty", tr("Elf")}, {"khajiit", tr("Khajiit")},
                                 {"orc", tr("Orc")}});

    // --- Cris et chants : noms relus depuis les boutons retraduits du .ui ---
    const QList<QPushButton*> shoutButtons = {
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
        shoutEntries[i].name = shoutButtons[i]->text();
    }
    const QList<QPushButton*> bardButtons = {
        ui->ageOfOppressionBtn, ui->ageOfViolenceBtn, ui->ragnarTheRedBtn,
        ui->taleOfTheTonguesBtn, ui->theDragonbornComesBtn,
    };
    for (int i = 0; i < bardButtons.size() && i < bardEntries.size(); ++i) {
        bardEntries[i].name = bardButtons[i]->text();
    }

    if (shoutsTable) shoutsTable->setHorizontalHeaderLabels({tr("Shout Name"), "", tr("Status"), tr("Actions")});
    if (bardsTable)  bardsTable->setHorizontalHeaderLabels({tr("Song"), "", tr("Status"), tr("Lyrics"), tr("Actions")});

    RefreshShoutsTable();
    RefreshBardsTable();
    RefreshMusicStatus();
    UpdateStatusBar();
}

// ============================================================================
//  Bandeau : lecture, dossier Data, lancement du jeu
// ============================================================================

void MainWindow::onPlayerStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) {
        player->play();
    }
}

void MainWindow::PlayFile(const QString &path)
{
    player->stop();
    player->setSource(QUrl::fromLocalFile(path));
}

void MainWindow::StopPlayback()
{
    player->stop();
    player->setSource(QUrl());   // libere le fichier (verrou Windows)
}

void MainWindow::on_stopMusicButton_clicked()
{
    player->pause();
}

void MainWindow::on_pushButton_clicked()
{
    if (!EnsureInstallation()) return;
    QDesktopServices::openUrl(QUrl::fromLocalFile(install.dataPath()));
}

void MainWindow::on_launchSkyrimButton_clicked()
{
    if (!EnsureInstallation()) return;

    // SKSE en priorite s'il est installe, sinon le lanceur du jeu.
    const bool se = install.edition() == SkyrimInstall::Edition::SpecialEdition;
    const QStringList candidates = se
        ? QStringList{"skse64_loader.exe", "SkyrimSELauncher.exe", "SkyrimSE.exe"}
        : QStringList{"skse_loader.exe", "SkyrimLauncher.exe", "TESV.exe"};

    for (const QString &exe : candidates) {
        const QString path = install.root() + "/" + exe;
        if (QFile::exists(path)) {
            if (!QProcess::startDetached(path, {}, install.root())) {
                ShowError(tr("Launch Skyrim"), tr("Could not start %1").arg(path));
            }
            return;
        }
    }

    // Repli : lancement via Steam
    QDesktopServices::openUrl(QUrl(se ? "steam://rungameid/489830" : "steam://rungameid/72850"));
}

// ============================================================================
//  Utilitaires
// ============================================================================

QJsonObject MainWindow::LoadJson(const QString &fileName)
{
    QFile file(AudioConverter::toolsDir() + "/" + fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
    return QJsonDocument::fromJson(file.readAll()).object();
}

void MainWindow::ShowError(const QString &title, const QString &message)
{
    QMessageBox::critical(this, title, message);
}
