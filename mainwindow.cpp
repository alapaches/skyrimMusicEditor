#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "json.hpp"

using json = nlohmann::json;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qApp->installTranslator(&translator);
    QString disclaimerTitle = tr("Bards Song Replacing");
    QMessageBox *disclaimer = new QMessageBox();
    disclaimer->setIcon(QMessageBox::Information);

    disclaimer->setWindowTitle(disclaimerTitle);
    disclaimer->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
    disclaimer->setTextFormat(Qt::RichText);
    disclaimer->setText(tr("v1.0.3 : <br />Before replacing a bard file, please click on the song's lyrics first to know what file you will need"));


    connect(ui->animalAllegianceBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->auraWhisperBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->callDragonBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->callOfValorBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->becomeEtheralBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->clearSkiesBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->disarmBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->dismayBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->dragonrendBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->elementalFuryBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->stormCallBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->unrelentingForceBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->kynePeaceBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->fireBreathBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->frostBreathBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->iceFormBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->throwVoiceBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->slowTimeBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->markedForDeathBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->whirlwindSprintBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));

    connect(ui->drainVitalityBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->soulTearBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->summonDurnehviirBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));

    connect(ui->bendWillBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->battleFuryBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->cycloneBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));
    connect(ui->dragonAspectBtn, SIGNAL(clicked()), this, SLOT(ReplaceShout()));

    connect(ui->ageOfOppressionBtn, SIGNAL(clicked()), this, SLOT(ProcessFileBards()));
    connect(ui->ageOfViolenceBtn, SIGNAL(clicked()), this, SLOT(ProcessFileBards()));
    connect(ui->ragnarTheRedBtn, SIGNAL(clicked()), this, SLOT(ProcessFileBards()));
    connect(ui->taleOfTheTonguesBtn, SIGNAL(clicked()), this, SLOT(ProcessFileBards()));
    connect(ui->theDragonbornComesBtn, SIGNAL(clicked()), this, SLOT(ProcessFileBards()));

    connect(ui->restoreDawnguardShoutsBtn, SIGNAL(clicked()), this, SLOT(RestoreShouts()));
    connect(ui->restoreDragonbornShoutsBtn, SIGNAL(clicked()), this, SLOT(RestoreShouts()));

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

//    disclaimer->exec();
    ui->tabWidget->setCurrentIndex(0);
    ui->musicTabWidget->setCurrentIndex(0);

    this->setWindowTitle(version);
    if(skyrimRealPath == "") {
        QMessageBox noPath;
        noPath.setWindowTitle("Attention");
        noPath.setIcon(QMessageBox::Warning);
        noPath.setText(tr("Skyrim directory was not detected. Please choose the directory with the button below."));
        noPath.setStandardButtons(QMessageBox::Close);
        QAbstractButton *skyrimPathButton = noPath.addButton(tr("Locate"), QMessageBox::ActionRole);
        connect(skyrimPathButton, &QAbstractButton::clicked, this, [=]() {
            QString changeMusicDirectory = QFileDialog::getExistingDirectory();
            skyrimRealPath = changeMusicDirectory + "/";
        });
        noPath.exec();
    }
    ui->pathLineEdit->setText(skyrimRealPath);

    // Creates our new model and populate
    ui->combatTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->exploreTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->dungeonTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->dreadTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->specialTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->otherTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->revealTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->rewardTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->stingerTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tavernTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->townTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->dawnguardTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->dragonbornTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    playIcon.addPixmap(QPixmap(":/images/images/ico/play.ico"));
    stopIcon.addPixmap(QPixmap(":/images/images/ico/stop.ico"));
    browseIcon.addPixmap(QPixmap(":/images/images/ico/folder.ico"));
    QIcon folderIcon;
    folderIcon.addPixmap(QPixmap(":/images/images/ico/folder.ico"));
    ui->stopMusicButton->setIcon(stopIcon);
    ui->pushButton->setIcon(folderIcon);

    #define Buttons_Combat_Widget_START {
        QPushButton* combatButton1 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton2 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton3 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton4 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton5 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton6 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton7 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton8 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton9 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton10 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton11 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton12 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton13 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton14 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton15 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton16 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton17 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatButton18 = new QPushButton(ui->combatTableWidget);

        QPushButton* combatPlay1 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay2 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay3 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay4 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay5 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay6 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay7 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay8 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay9 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay10 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay11 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay12 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay13 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay14 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay15 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay16 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay17 = new QPushButton(ui->combatTableWidget);
        QPushButton* combatPlay18 = new QPushButton(ui->combatTableWidget);

        combatButton1->setIcon(browseIcon);
        combatButton2->setIcon(browseIcon);
        combatButton3->setIcon(browseIcon);
        combatButton4->setIcon(browseIcon);
        combatButton5->setIcon(browseIcon);
        combatButton6->setIcon(browseIcon);
        combatButton7->setIcon(browseIcon);
        combatButton8->setIcon(browseIcon);
        combatButton9->setIcon(browseIcon);
        combatButton10->setIcon(browseIcon);
        combatButton11->setIcon(browseIcon);
        combatButton12->setIcon(browseIcon);
        combatButton13->setIcon(browseIcon);
        combatButton14->setIcon(browseIcon);
        combatButton15->setIcon(browseIcon);
        combatButton16->setIcon(browseIcon);
        combatButton17->setIcon(browseIcon);
        combatButton18->setIcon(browseIcon);

        combatPlay1->setIcon(playIcon);
        combatPlay2->setIcon(playIcon);
        combatPlay3->setIcon(playIcon);
        combatPlay4->setIcon(playIcon);
        combatPlay5->setIcon(playIcon);
        combatPlay6->setIcon(playIcon);
        combatPlay7->setIcon(playIcon);
        combatPlay8->setIcon(playIcon);
        combatPlay9->setIcon(playIcon);
        combatPlay10->setIcon(playIcon);
        combatPlay11->setIcon(playIcon);
        combatPlay12->setIcon(playIcon);
        combatPlay13->setIcon(playIcon);
        combatPlay14->setIcon(playIcon);
        combatPlay15->setIcon(playIcon);
        combatPlay16->setIcon(playIcon);
        combatPlay17->setIcon(playIcon);
        combatPlay18->setIcon(playIcon);

    #define Buttons_Combat_Widget_END }

    #define Buttons_Dungeon_Widget_START {
        QPushButton* dungeonButton1 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonButton2 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonButton3 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonButton4 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonButton5 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonButton6 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonButton7 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonButton8 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonButton9 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonButton10 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonPlay1 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonPlay2 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonPlay3 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonPlay4 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonPlay5 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonPlay6 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonPlay7 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonPlay8 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonPlay9 = new QPushButton(ui->dungeonTableWidget);
        QPushButton* dungeonPlay10 = new QPushButton(ui->dungeonTableWidget);
        dungeonButton1->setIcon(browseIcon);
        dungeonButton2->setIcon(browseIcon);
        dungeonButton3->setIcon(browseIcon);
        dungeonButton4->setIcon(browseIcon);
        dungeonButton5->setIcon(browseIcon);
        dungeonButton6->setIcon(browseIcon);
        dungeonButton7->setIcon(browseIcon);
        dungeonButton8->setIcon(browseIcon);
        dungeonButton9->setIcon(browseIcon);
        dungeonButton10->setIcon(browseIcon);
        dungeonPlay1->setIcon(playIcon);
        dungeonPlay2->setIcon(playIcon);
        dungeonPlay3->setIcon(playIcon);
        dungeonPlay4->setIcon(playIcon);
        dungeonPlay5->setIcon(playIcon);
        dungeonPlay6->setIcon(playIcon);
        dungeonPlay7->setIcon(playIcon);
        dungeonPlay8->setIcon(playIcon);
        dungeonPlay9->setIcon(playIcon);
        dungeonPlay10->setIcon(playIcon);

    #define Buttons_Dungeon_Widget_END }

    #define Buttons_Dread_Widget_START {
        QPushButton* dreadButton1 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadButton2 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadButton3 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadButton4 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadButton5 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadButton6 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadButton7 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadButton8 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadPlay1 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadPlay2 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadPlay3 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadPlay4 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadPlay5 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadPlay6 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadPlay7 = new QPushButton(ui->dreadTableWidget);
        QPushButton* dreadPlay8 = new QPushButton(ui->dreadTableWidget);
        dreadButton1->setIcon(browseIcon);
        dreadButton2->setIcon(browseIcon);
        dreadButton3->setIcon(browseIcon);
        dreadButton4->setIcon(browseIcon);
        dreadButton5->setIcon(browseIcon);
        dreadButton6->setIcon(browseIcon);
        dreadButton7->setIcon(browseIcon);
        dreadButton8->setIcon(browseIcon);
        dreadPlay1->setIcon(playIcon);
        dreadPlay2->setIcon(playIcon);
        dreadPlay3->setIcon(playIcon);
        dreadPlay4->setIcon(playIcon);
        dreadPlay5->setIcon(playIcon);
        dreadPlay6->setIcon(playIcon);
        dreadPlay7->setIcon(playIcon);
        dreadPlay8->setIcon(playIcon);

    #define Buttons_Dread_Widget_END }

    #define Buttons_Explore_Widget_START {
        QPushButton* exploreButton1 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton2 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton3 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton4 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton5 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton6 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton7 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton8 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton9 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton10 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton11 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton12 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton13 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton14 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton15 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton16 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton17 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton18 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton19 = new QPushButton(ui->exploreTableWidget);
        QPushButton* exploreButton20 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay1 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay2 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay3 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay4 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay5 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay6 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay7 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay8 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay9 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay10 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay11 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay12 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay13 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay14 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay15 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay16 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay17 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay18 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay19 = new QPushButton(ui->exploreTableWidget);
        QPushButton* explorePlay20 = new QPushButton(ui->exploreTableWidget);
        exploreButton1->setIcon(browseIcon);
        exploreButton2->setIcon(browseIcon);
        exploreButton3->setIcon(browseIcon);
        exploreButton4->setIcon(browseIcon);
        exploreButton5->setIcon(browseIcon);
        exploreButton6->setIcon(browseIcon);
        exploreButton7->setIcon(browseIcon);
        exploreButton8->setIcon(browseIcon);
        exploreButton9->setIcon(browseIcon);
        exploreButton10->setIcon(browseIcon);
        exploreButton11->setIcon(browseIcon);
        exploreButton12->setIcon(browseIcon);
        exploreButton13->setIcon(browseIcon);
        exploreButton14->setIcon(browseIcon);
        exploreButton15->setIcon(browseIcon);
        exploreButton16->setIcon(browseIcon);
        exploreButton17->setIcon(browseIcon);
        exploreButton18->setIcon(browseIcon);
        exploreButton19->setIcon(browseIcon);
        exploreButton20->setIcon(browseIcon);
        explorePlay1->setIcon(playIcon);
        explorePlay2->setIcon(playIcon);
        explorePlay3->setIcon(playIcon);
        explorePlay4->setIcon(playIcon);
        explorePlay5->setIcon(playIcon);
        explorePlay6->setIcon(playIcon);
        explorePlay7->setIcon(playIcon);
        explorePlay8->setIcon(playIcon);
        explorePlay9->setIcon(playIcon);
        explorePlay10->setIcon(playIcon);
        explorePlay11->setIcon(playIcon);
        explorePlay12->setIcon(playIcon);
        explorePlay13->setIcon(playIcon);
        explorePlay14->setIcon(playIcon);
        explorePlay15->setIcon(playIcon);
        explorePlay16->setIcon(playIcon);
        explorePlay17->setIcon(playIcon);
        explorePlay18->setIcon(playIcon);
        explorePlay19->setIcon(playIcon);
        explorePlay20->setIcon(playIcon);

    #define Buttons_Explore_Widget_END }

    #define Buttons_Other_Widget_START {
        QPushButton* otherButton1 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherButton2 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherButton3 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherButton4 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherButton5 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherButton6 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherButton7 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherButton8 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherButton9 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherButton10 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherPlay1 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherPlay2 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherPlay3 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherPlay4 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherPlay5 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherPlay6 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherPlay7 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherPlay8 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherPlay9 = new QPushButton(ui->otherTableWidget);
        QPushButton* otherPlay10 = new QPushButton(ui->otherTableWidget);
        otherButton1->setIcon(browseIcon);
        otherButton2->setIcon(browseIcon);
        otherButton3->setIcon(browseIcon);
        otherButton4->setIcon(browseIcon);
        otherButton5->setIcon(browseIcon);
        otherButton6->setIcon(browseIcon);
        otherButton7->setIcon(browseIcon);
        otherButton8->setIcon(browseIcon);
        otherButton9->setIcon(browseIcon);
        otherButton10->setIcon(browseIcon);
        otherPlay1->setIcon(playIcon);
        otherPlay2->setIcon(playIcon);
        otherPlay3->setIcon(playIcon);
        otherPlay4->setIcon(playIcon);
        otherPlay5->setIcon(playIcon);
        otherPlay6->setIcon(playIcon);
        otherPlay7->setIcon(playIcon);
        otherPlay8->setIcon(playIcon);
        otherPlay9->setIcon(playIcon);
        otherPlay10->setIcon(playIcon);
    #define Buttons_Other_Widget_END }

    #define Buttons_Reward_Widget_START {
        QPushButton* rewardButton1 = new QPushButton(ui->rewardTableWidget);
        QPushButton* rewardButton2 = new QPushButton(ui->rewardTableWidget);
        QPushButton* rewardButton3 = new QPushButton(ui->rewardTableWidget);
        QPushButton* rewardButton4 = new QPushButton(ui->rewardTableWidget);
        QPushButton* rewardPlay1 = new QPushButton(ui->rewardTableWidget);
        QPushButton* rewardPlay2 = new QPushButton(ui->rewardTableWidget);
        QPushButton* rewardPlay3 = new QPushButton(ui->rewardTableWidget);
        QPushButton* rewardPlay4 = new QPushButton(ui->rewardTableWidget);
        rewardButton1->setIcon(browseIcon);
        rewardButton2->setIcon(browseIcon);
        rewardButton3->setIcon(browseIcon);
        rewardButton4->setIcon(browseIcon);
        rewardPlay1->setIcon(playIcon);
        rewardPlay2->setIcon(playIcon);
        rewardPlay3->setIcon(playIcon);
        rewardPlay4->setIcon(playIcon);
    #define Buttons_Reward_Widget_END }

    #define Buttons_Reveal_Widget_START {
        QPushButton* revealButton1 = new QPushButton(ui->revealTableWidget);
        QPushButton* revealButton2 = new QPushButton(ui->revealTableWidget);
        QPushButton* revealButton3 = new QPushButton(ui->revealTableWidget);
        QPushButton* revealButton4 = new QPushButton(ui->revealTableWidget);
        QPushButton* revealPlay1 = new QPushButton(ui->revealTableWidget);
        QPushButton* revealPlay2 = new QPushButton(ui->revealTableWidget);
        QPushButton* revealPlay3 = new QPushButton(ui->revealTableWidget);
        QPushButton* revealPlay4 = new QPushButton(ui->revealTableWidget);
        revealButton1->setIcon(browseIcon);
        revealButton2->setIcon(browseIcon);
        revealButton3->setIcon(browseIcon);
        revealButton4->setIcon(browseIcon);
        revealPlay1->setIcon(playIcon);
        revealPlay2->setIcon(playIcon);
        revealPlay3->setIcon(playIcon);
        revealPlay4->setIcon(playIcon);
    #define Buttons_Reveal_Widget_END }

    #define Buttons_Special_Widget_START {
        QPushButton* specialButton1 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton2 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton3 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton4 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton5 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton6 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton7 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton8 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton9 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton10 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton11 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton12 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialButton13 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay1 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay2 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay3 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay4 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay5 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay6 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay7 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay8 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay9 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay10 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay11 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay12 = new QPushButton(ui->specialTableWidget);
        QPushButton* specialPlay13 = new QPushButton(ui->specialTableWidget);
        specialButton1->setIcon(browseIcon);
        specialButton2->setIcon(browseIcon);
        specialButton3->setIcon(browseIcon);
        specialButton4->setIcon(browseIcon);
        specialButton5->setIcon(browseIcon);
        specialButton6->setIcon(browseIcon);
        specialButton7->setIcon(browseIcon);
        specialButton8->setIcon(browseIcon);
        specialButton9->setIcon(browseIcon);
        specialButton10->setIcon(browseIcon);
        specialButton11->setIcon(browseIcon);
        specialButton12->setIcon(browseIcon);
        specialButton13->setIcon(browseIcon);
        specialPlay1->setIcon(playIcon);
        specialPlay2->setIcon(playIcon);
        specialPlay3->setIcon(playIcon);
        specialPlay4->setIcon(playIcon);
        specialPlay5->setIcon(playIcon);
        specialPlay6->setIcon(playIcon);
        specialPlay7->setIcon(playIcon);
        specialPlay8->setIcon(playIcon);
        specialPlay9->setIcon(playIcon);
        specialPlay10->setIcon(playIcon);
        specialPlay11->setIcon(playIcon);
        specialPlay12->setIcon(playIcon);
        specialPlay13->setIcon(playIcon);

    #define Buttons_Special_Widget_END }

    #define Buttons_Stinger_Widget_START {
        QPushButton* stingerButton1 = new QPushButton(ui->stingerTableWidget);
        QPushButton* stingerButton2 = new QPushButton(ui->stingerTableWidget);
        QPushButton* stingerButton3 = new QPushButton(ui->stingerTableWidget);
        QPushButton* stingerButton4 = new QPushButton(ui->stingerTableWidget);
        QPushButton* stingerPlay1 = new QPushButton(ui->stingerTableWidget);
        QPushButton* stingerPlay2 = new QPushButton(ui->stingerTableWidget);
        QPushButton* stingerPlay3 = new QPushButton(ui->stingerTableWidget);
        QPushButton* stingerPlay4 = new QPushButton(ui->stingerTableWidget);
        stingerButton1->setIcon(browseIcon);
        stingerButton2->setIcon(browseIcon);
        stingerButton3->setIcon(browseIcon);
        stingerButton4->setIcon(browseIcon);
        stingerPlay1->setIcon(playIcon);
        stingerPlay2->setIcon(playIcon);
        stingerPlay3->setIcon(playIcon);
        stingerPlay4->setIcon(playIcon);
    #define Buttons_Stinger_Widget_END }

    #define Buttons_Tavern_Widget_START {
        QPushButton* tavernButton1 = new QPushButton(ui->tavernTableWidget);
        QPushButton* tavernButton2 = new QPushButton(ui->tavernTableWidget);
        QPushButton* tavernButton3 = new QPushButton(ui->tavernTableWidget);
        QPushButton* tavernButton4 = new QPushButton(ui->tavernTableWidget);
        QPushButton* tavernButton5 = new QPushButton(ui->tavernTableWidget);
        QPushButton* tavernPlay1 = new QPushButton(ui->tavernTableWidget);
        QPushButton* tavernPlay2 = new QPushButton(ui->tavernTableWidget);
        QPushButton* tavernPlay3 = new QPushButton(ui->tavernTableWidget);
        QPushButton* tavernPlay4 = new QPushButton(ui->tavernTableWidget);
        QPushButton* tavernPlay5 = new QPushButton(ui->tavernTableWidget);
        tavernButton1->setIcon(browseIcon);
        tavernButton2->setIcon(browseIcon);
        tavernButton3->setIcon(browseIcon);
        tavernButton4->setIcon(browseIcon);
        tavernButton5->setIcon(browseIcon);
        tavernPlay1->setIcon(playIcon);
        tavernPlay2->setIcon(playIcon);
        tavernPlay3->setIcon(playIcon);
        tavernPlay4->setIcon(playIcon);
        tavernPlay5->setIcon(playIcon);

    #define Buttons_Tavern_Widget_END }

    #define Buttons_Town_Widget_START {
        QPushButton* townButton1 = new QPushButton(ui->townTableWidget);
        QPushButton* townButton2 = new QPushButton(ui->townTableWidget);
        QPushButton* townButton3 = new QPushButton(ui->townTableWidget);
        QPushButton* townButton4 = new QPushButton(ui->townTableWidget);
        QPushButton* townButton5 = new QPushButton(ui->townTableWidget);
        QPushButton* townButton6 = new QPushButton(ui->townTableWidget);
        QPushButton* townButton7 = new QPushButton(ui->townTableWidget);
        QPushButton* townPlay1 = new QPushButton(ui->townTableWidget);
        QPushButton* townPlay2 = new QPushButton(ui->townTableWidget);
        QPushButton* townPlay3 = new QPushButton(ui->townTableWidget);
        QPushButton* townPlay4 = new QPushButton(ui->townTableWidget);
        QPushButton* townPlay5 = new QPushButton(ui->townTableWidget);
        QPushButton* townPlay6 = new QPushButton(ui->townTableWidget);
        QPushButton* townPlay7 = new QPushButton(ui->townTableWidget);
        townButton1->setIcon(browseIcon);
        townButton2->setIcon(browseIcon);
        townButton3->setIcon(browseIcon);
        townButton4->setIcon(browseIcon);
        townButton5->setIcon(browseIcon);
        townButton6->setIcon(browseIcon);
        townButton7->setIcon(browseIcon);
        townPlay1->setIcon(playIcon);
        townPlay2->setIcon(playIcon);
        townPlay3->setIcon(playIcon);
        townPlay4->setIcon(playIcon);
        townPlay5->setIcon(playIcon);
        townPlay6->setIcon(playIcon);
        townPlay7->setIcon(playIcon);
    #define Buttons_Town_Widget_END }

    #define Buttons_Dawnguard_Widget_START {
            QPushButton* dawnguardButton1 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton2 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton3 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton4 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton5 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton6 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton7 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton8 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton9 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton10 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton11 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton12 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardButton13 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton1 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton2 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton3 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton4 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton5 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton6 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton7 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton8 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton9 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton10 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton11 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton12 = new QPushButton(ui->dawnguardTableWidget);
            QPushButton* dawnguardPlayButton13 = new QPushButton(ui->dawnguardTableWidget);
            dawnguardButton1->setIcon(browseIcon);
            dawnguardButton2->setIcon(browseIcon);
            dawnguardButton3->setIcon(browseIcon);
            dawnguardButton4->setIcon(browseIcon);
            dawnguardButton5->setIcon(browseIcon);
            dawnguardButton6->setIcon(browseIcon);
            dawnguardButton7->setIcon(browseIcon);
            dawnguardButton8->setIcon(browseIcon);
            dawnguardButton9->setIcon(browseIcon);
            dawnguardButton10->setIcon(browseIcon);
            dawnguardButton11->setIcon(browseIcon);
            dawnguardButton12->setIcon(browseIcon);
            dawnguardButton13->setIcon(browseIcon);
            dawnguardPlayButton1->setIcon(playIcon);
            dawnguardPlayButton2->setIcon(playIcon);
            dawnguardPlayButton3->setIcon(playIcon);
            dawnguardPlayButton4->setIcon(playIcon);
            dawnguardPlayButton5->setIcon(playIcon);
            dawnguardPlayButton6->setIcon(playIcon);
            dawnguardPlayButton7->setIcon(playIcon);
            dawnguardPlayButton8->setIcon(playIcon);
            dawnguardPlayButton9->setIcon(playIcon);
            dawnguardPlayButton10->setIcon(playIcon);
            dawnguardPlayButton11->setIcon(playIcon);
            dawnguardPlayButton12->setIcon(playIcon);
            dawnguardPlayButton13->setIcon(playIcon);
    #define Buttons_Dawnguard_Widget_END }

    #define Buttons_Dragonborn_Widget_START {
            QPushButton* dragonbornButton1 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton2 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton3 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton4 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton5 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton6 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton7 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton8 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton9 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton10 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton11 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton12 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton13 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornButton14 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton1 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton2 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton3 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton4 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton5 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton6 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton7 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton8 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton9 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton10 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton11 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton12 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton13 = new QPushButton(ui->dragonbornTableWidget);
            QPushButton* dragonbornPlayButton14 = new QPushButton(ui->dragonbornTableWidget);
            dragonbornButton1->setIcon(browseIcon);
            dragonbornButton2->setIcon(browseIcon);
            dragonbornButton3->setIcon(browseIcon);
            dragonbornButton4->setIcon(browseIcon);
            dragonbornButton5->setIcon(browseIcon);
            dragonbornButton6->setIcon(browseIcon);
            dragonbornButton7->setIcon(browseIcon);
            dragonbornButton8->setIcon(browseIcon);
            dragonbornButton9->setIcon(browseIcon);
            dragonbornButton10->setIcon(browseIcon);
            dragonbornButton11->setIcon(browseIcon);
            dragonbornButton12->setIcon(browseIcon);
            dragonbornButton13->setIcon(browseIcon);
            dragonbornButton14->setIcon(browseIcon);
            dragonbornPlayButton1->setIcon(playIcon);
            dragonbornPlayButton2->setIcon(playIcon);
            dragonbornPlayButton3->setIcon(playIcon);
            dragonbornPlayButton4->setIcon(playIcon);
            dragonbornPlayButton5->setIcon(playIcon);
            dragonbornPlayButton6->setIcon(playIcon);
            dragonbornPlayButton7->setIcon(playIcon);
            dragonbornPlayButton8->setIcon(playIcon);
            dragonbornPlayButton9->setIcon(playIcon);
            dragonbornPlayButton10->setIcon(playIcon);
            dragonbornPlayButton11->setIcon(playIcon);
            dragonbornPlayButton12->setIcon(playIcon);
            dragonbornPlayButton13->setIcon(playIcon);
            dragonbornPlayButton14->setIcon(playIcon);
    #define Buttons_Dragonborn_Widget_END }

    // Insert Buttons Combat
    ui->combatTableWidget->setCellWidget(0,2, combatButton1);
    ui->combatTableWidget->setCellWidget(1,2, combatButton2);
    ui->combatTableWidget->setCellWidget(2,2, combatButton3);
    ui->combatTableWidget->setCellWidget(3,2, combatButton4);
    ui->combatTableWidget->setCellWidget(4,2, combatButton5);
    ui->combatTableWidget->setCellWidget(5,2, combatButton6);
    ui->combatTableWidget->setCellWidget(6,2, combatButton7);
    ui->combatTableWidget->setCellWidget(7,2, combatButton8);
    ui->combatTableWidget->setCellWidget(8,2, combatButton9);
    ui->combatTableWidget->setCellWidget(9,2, combatButton10);
    ui->combatTableWidget->setCellWidget(10,2, combatButton11);
    ui->combatTableWidget->setCellWidget(11,2, combatButton12);
    ui->combatTableWidget->setCellWidget(12,2, combatButton13);
    ui->combatTableWidget->setCellWidget(13,2, combatButton14);
    ui->combatTableWidget->setCellWidget(14,2, combatButton15);
    ui->combatTableWidget->setCellWidget(15,2, combatButton16);
    ui->combatTableWidget->setCellWidget(16,2, combatButton17);
    ui->combatTableWidget->setCellWidget(17,2, combatButton18);

    ui->combatTableWidget->setCellWidget(0,3,combatPlay1);
    ui->combatTableWidget->setCellWidget(1,3,combatPlay2);
    ui->combatTableWidget->setCellWidget(2,3,combatPlay3);
    ui->combatTableWidget->setCellWidget(3,3,combatPlay4);
    ui->combatTableWidget->setCellWidget(4,3,combatPlay5);
    ui->combatTableWidget->setCellWidget(5,3,combatPlay6);
    ui->combatTableWidget->setCellWidget(6,3,combatPlay7);
    ui->combatTableWidget->setCellWidget(7,3,combatPlay8);
    ui->combatTableWidget->setCellWidget(8,3,combatPlay9);
    ui->combatTableWidget->setCellWidget(9,3,combatPlay10);
    ui->combatTableWidget->setCellWidget(10,3,combatPlay11);
    ui->combatTableWidget->setCellWidget(11,3,combatPlay12);
    ui->combatTableWidget->setCellWidget(12,3,combatPlay13);
    ui->combatTableWidget->setCellWidget(13,3,combatPlay14);
    ui->combatTableWidget->setCellWidget(14,3,combatPlay15);
    ui->combatTableWidget->setCellWidget(15,3,combatPlay16);
    ui->combatTableWidget->setCellWidget(16,3,combatPlay17);
    ui->combatTableWidget->setCellWidget(17,3,combatPlay18);

    // Insert Buttons Dungeon
    ui->dungeonTableWidget->setCellWidget(0,2, dungeonButton1);
    ui->dungeonTableWidget->setCellWidget(1,2, dungeonButton2);
    ui->dungeonTableWidget->setCellWidget(2,2, dungeonButton3);
    ui->dungeonTableWidget->setCellWidget(3,2, dungeonButton4);
    ui->dungeonTableWidget->setCellWidget(4,2, dungeonButton5);
    ui->dungeonTableWidget->setCellWidget(5,2, dungeonButton6);
    ui->dungeonTableWidget->setCellWidget(6,2, dungeonButton7);
    ui->dungeonTableWidget->setCellWidget(7,2, dungeonButton8);
    ui->dungeonTableWidget->setCellWidget(8,2, dungeonButton9);
    ui->dungeonTableWidget->setCellWidget(9,2, dungeonButton10);

    ui->dungeonTableWidget->setCellWidget(0,3,dungeonPlay1);
    ui->dungeonTableWidget->setCellWidget(1,3,dungeonPlay2);
    ui->dungeonTableWidget->setCellWidget(2,3,dungeonPlay3);
    ui->dungeonTableWidget->setCellWidget(3,3,dungeonPlay4);
    ui->dungeonTableWidget->setCellWidget(4,3,dungeonPlay5);
    ui->dungeonTableWidget->setCellWidget(5,3,dungeonPlay6);
    ui->dungeonTableWidget->setCellWidget(6,3,dungeonPlay7);
    ui->dungeonTableWidget->setCellWidget(7,3,dungeonPlay8);
    ui->dungeonTableWidget->setCellWidget(8,3,dungeonPlay9);
    ui->dungeonTableWidget->setCellWidget(9,3,dungeonPlay10);

    // Insert Buttons Dread
    ui->dreadTableWidget->setCellWidget(0,2, dreadButton1);
    ui->dreadTableWidget->setCellWidget(1,2, dreadButton2);
    ui->dreadTableWidget->setCellWidget(2,2, dreadButton3);
    ui->dreadTableWidget->setCellWidget(3,2, dreadButton4);
    ui->dreadTableWidget->setCellWidget(4,2, dreadButton5);
    ui->dreadTableWidget->setCellWidget(5,2, dreadButton6);
    ui->dreadTableWidget->setCellWidget(6,2, dreadButton7);
    ui->dreadTableWidget->setCellWidget(7,2, dreadButton8);

    ui->dreadTableWidget->setCellWidget(0,3,dreadPlay1);
    ui->dreadTableWidget->setCellWidget(1,3,dreadPlay2);
    ui->dreadTableWidget->setCellWidget(2,3,dreadPlay3);
    ui->dreadTableWidget->setCellWidget(3,3,dreadPlay4);
    ui->dreadTableWidget->setCellWidget(4,3,dreadPlay5);
    ui->dreadTableWidget->setCellWidget(5,3,dreadPlay6);
    ui->dreadTableWidget->setCellWidget(6,3,dreadPlay7);
    ui->dreadTableWidget->setCellWidget(7,3,dreadPlay8);

    // Insert Buttons Explore
    ui->exploreTableWidget->setCellWidget(0,2, exploreButton1);
    ui->exploreTableWidget->setCellWidget(1,2, exploreButton2);
    ui->exploreTableWidget->setCellWidget(2,2, exploreButton3);
    ui->exploreTableWidget->setCellWidget(3,2, exploreButton4);
    ui->exploreTableWidget->setCellWidget(4,2, exploreButton5);
    ui->exploreTableWidget->setCellWidget(5,2, exploreButton6);
    ui->exploreTableWidget->setCellWidget(6,2, exploreButton7);
    ui->exploreTableWidget->setCellWidget(7,2, exploreButton8);
    ui->exploreTableWidget->setCellWidget(8,2, exploreButton9);
    ui->exploreTableWidget->setCellWidget(9,2, exploreButton10);
    ui->exploreTableWidget->setCellWidget(10,2, exploreButton11);
    ui->exploreTableWidget->setCellWidget(11,2, exploreButton12);
    ui->exploreTableWidget->setCellWidget(12,2, exploreButton13);
    ui->exploreTableWidget->setCellWidget(13,2, exploreButton14);
    ui->exploreTableWidget->setCellWidget(14,2, exploreButton15);
    ui->exploreTableWidget->setCellWidget(15,2, exploreButton16);
    ui->exploreTableWidget->setCellWidget(16,2, exploreButton17);
    ui->exploreTableWidget->setCellWidget(17,2, exploreButton18);
    ui->exploreTableWidget->setCellWidget(18,2, exploreButton19);
    ui->exploreTableWidget->setCellWidget(19,2, exploreButton20);

    ui->exploreTableWidget->setCellWidget(0,3,explorePlay1);
    ui->exploreTableWidget->setCellWidget(1,3,explorePlay2);
    ui->exploreTableWidget->setCellWidget(2,3,explorePlay3);
    ui->exploreTableWidget->setCellWidget(3,3,explorePlay4);
    ui->exploreTableWidget->setCellWidget(4,3,explorePlay5);
    ui->exploreTableWidget->setCellWidget(5,3,explorePlay6);
    ui->exploreTableWidget->setCellWidget(6,3,explorePlay7);
    ui->exploreTableWidget->setCellWidget(7,3,explorePlay8);
    ui->exploreTableWidget->setCellWidget(8,3,explorePlay9);
    ui->exploreTableWidget->setCellWidget(9,3,explorePlay10);
    ui->exploreTableWidget->setCellWidget(10,3,explorePlay11);
    ui->exploreTableWidget->setCellWidget(11,3,explorePlay12);
    ui->exploreTableWidget->setCellWidget(12,3,explorePlay13);
    ui->exploreTableWidget->setCellWidget(13,3,explorePlay14);
    ui->exploreTableWidget->setCellWidget(14,3,explorePlay15);
    ui->exploreTableWidget->setCellWidget(15,3,explorePlay16);
    ui->exploreTableWidget->setCellWidget(16,3,explorePlay17);
    ui->exploreTableWidget->setCellWidget(17,3,explorePlay18);
    ui->exploreTableWidget->setCellWidget(18,3,explorePlay19);
    ui->exploreTableWidget->setCellWidget(19,3,explorePlay20);


    // Insert Buttons Other
    ui->otherTableWidget->setCellWidget(0,2, otherButton1);
    ui->otherTableWidget->setCellWidget(1,2, otherButton2);
    ui->otherTableWidget->setCellWidget(2,2, otherButton3);
    ui->otherTableWidget->setCellWidget(3,2, otherButton4);
    ui->otherTableWidget->setCellWidget(4,2, otherButton5);
    ui->otherTableWidget->setCellWidget(5,2, otherButton6);
    ui->otherTableWidget->setCellWidget(6,2, otherButton7);
    ui->otherTableWidget->setCellWidget(7,2, otherButton8);
    ui->otherTableWidget->setCellWidget(8,2, otherButton9);
    ui->otherTableWidget->setCellWidget(9,2, otherButton10);

    ui->otherTableWidget->setCellWidget(0,3,otherPlay1);
    ui->otherTableWidget->setCellWidget(1,3,otherPlay2);
    ui->otherTableWidget->setCellWidget(2,3,otherPlay3);
    ui->otherTableWidget->setCellWidget(3,3,otherPlay4);
    ui->otherTableWidget->setCellWidget(4,3,otherPlay5);
    ui->otherTableWidget->setCellWidget(5,3,otherPlay6);
    ui->otherTableWidget->setCellWidget(6,3,otherPlay7);
    ui->otherTableWidget->setCellWidget(7,3,otherPlay8);
    ui->otherTableWidget->setCellWidget(8,3,otherPlay9);
    ui->otherTableWidget->setCellWidget(9,3,otherPlay10);

    // Insert Buttons Reward
    ui->rewardTableWidget->setCellWidget(0,2, rewardButton1);
    ui->rewardTableWidget->setCellWidget(1,2, rewardButton2);
    ui->rewardTableWidget->setCellWidget(2,2, rewardButton3);
    ui->rewardTableWidget->setCellWidget(3,2, rewardButton4);

    ui->rewardTableWidget->setCellWidget(0,3,rewardPlay1);
    ui->rewardTableWidget->setCellWidget(1,3,rewardPlay2);
    ui->rewardTableWidget->setCellWidget(2,3,rewardPlay3);
    ui->rewardTableWidget->setCellWidget(3,3,rewardPlay4);

    // Insert Buttons Reveal
    ui->revealTableWidget->setCellWidget(0,2, revealButton1);
    ui->revealTableWidget->setCellWidget(1,2, revealButton2);
    ui->revealTableWidget->setCellWidget(2,2, revealButton3);
    ui->revealTableWidget->setCellWidget(3,2, revealButton4);

    ui->revealTableWidget->setCellWidget(0,3,revealPlay1);
    ui->revealTableWidget->setCellWidget(1,3,revealPlay2);
    ui->revealTableWidget->setCellWidget(2,3,revealPlay3);
    ui->revealTableWidget->setCellWidget(3,3,revealPlay4);

    // Insert Buttons Special
    ui->specialTableWidget->setCellWidget(0,2, specialButton1);
    ui->specialTableWidget->setCellWidget(1,2, specialButton2);
    ui->specialTableWidget->setCellWidget(2,2, specialButton3);
    ui->specialTableWidget->setCellWidget(3,2, specialButton4);
    ui->specialTableWidget->setCellWidget(4,2, specialButton5);
    ui->specialTableWidget->setCellWidget(5,2, specialButton6);
    ui->specialTableWidget->setCellWidget(6,2, specialButton7);
    ui->specialTableWidget->setCellWidget(7,2, specialButton8);
    ui->specialTableWidget->setCellWidget(8,2, specialButton9);
    ui->specialTableWidget->setCellWidget(9,2, specialButton10);
    ui->specialTableWidget->setCellWidget(10,2, specialButton11);
    ui->specialTableWidget->setCellWidget(11,2, specialButton12);
    ui->specialTableWidget->setCellWidget(12,2, specialButton13);

    ui->specialTableWidget->setCellWidget(0,3,specialPlay1);
    ui->specialTableWidget->setCellWidget(1,3,specialPlay2);
    ui->specialTableWidget->setCellWidget(2,3,specialPlay3);
    ui->specialTableWidget->setCellWidget(3,3,specialPlay4);
    ui->specialTableWidget->setCellWidget(4,3,specialPlay5);
    ui->specialTableWidget->setCellWidget(5,3,specialPlay6);
    ui->specialTableWidget->setCellWidget(6,3,specialPlay7);
    ui->specialTableWidget->setCellWidget(7,3,specialPlay8);
    ui->specialTableWidget->setCellWidget(8,3,specialPlay9);
    ui->specialTableWidget->setCellWidget(9,3,specialPlay10);
    ui->specialTableWidget->setCellWidget(10,3,specialPlay11);
    ui->specialTableWidget->setCellWidget(11,3,specialPlay12);
    ui->specialTableWidget->setCellWidget(12,3,specialPlay13);

    // Insert Buttons Stinger
    ui->stingerTableWidget->setCellWidget(0,2, stingerButton1);
    ui->stingerTableWidget->setCellWidget(1,2, stingerButton2);
    ui->stingerTableWidget->setCellWidget(2,2, stingerButton3);
    ui->stingerTableWidget->setCellWidget(3,2, stingerButton4);

    ui->stingerTableWidget->setCellWidget(0,3,stingerPlay1);
    ui->stingerTableWidget->setCellWidget(1,3,stingerPlay2);
    ui->stingerTableWidget->setCellWidget(2,3,stingerPlay3);
    ui->stingerTableWidget->setCellWidget(3,3,stingerPlay4);

    // Insert Buttons Tavern
    ui->tavernTableWidget->setCellWidget(0,2, tavernButton1);
    ui->tavernTableWidget->setCellWidget(1,2, tavernButton2);
    ui->tavernTableWidget->setCellWidget(2,2, tavernButton3);
    ui->tavernTableWidget->setCellWidget(3,2, tavernButton4);
    ui->tavernTableWidget->setCellWidget(4,2, tavernButton5);

    ui->tavernTableWidget->setCellWidget(0,3,tavernPlay1);
    ui->tavernTableWidget->setCellWidget(1,3,tavernPlay2);
    ui->tavernTableWidget->setCellWidget(2,3,tavernPlay3);
    ui->tavernTableWidget->setCellWidget(3,3,tavernPlay4);
    ui->tavernTableWidget->setCellWidget(4,3,tavernPlay5);

    // Insert Buttons Town
    ui->townTableWidget->setCellWidget(0,2, townButton1);
    ui->townTableWidget->setCellWidget(1,2, townButton2);
    ui->townTableWidget->setCellWidget(2,2, townButton3);
    ui->townTableWidget->setCellWidget(3,2, townButton4);
    ui->townTableWidget->setCellWidget(4,2, townButton5);
    ui->townTableWidget->setCellWidget(5,2, townButton6);
    ui->townTableWidget->setCellWidget(6,2, townButton7);

    ui->townTableWidget->setCellWidget(0,3,townPlay1);
    ui->townTableWidget->setCellWidget(1,3,townPlay2);
    ui->townTableWidget->setCellWidget(2,3,townPlay3);
    ui->townTableWidget->setCellWidget(3,3,townPlay4);
    ui->townTableWidget->setCellWidget(4,3,townPlay5);
    ui->townTableWidget->setCellWidget(5,3,townPlay6);
    ui->townTableWidget->setCellWidget(6,3,townPlay7);

    // Insert Buttons Dawnguard
    ui->dawnguardTableWidget->setCellWidget(0,2, dawnguardButton1);
    ui->dawnguardTableWidget->setCellWidget(1,2, dawnguardButton2);
    ui->dawnguardTableWidget->setCellWidget(2,2, dawnguardButton3);
    ui->dawnguardTableWidget->setCellWidget(3,2, dawnguardButton4);
    ui->dawnguardTableWidget->setCellWidget(4,2, dawnguardButton5);
    ui->dawnguardTableWidget->setCellWidget(5,2, dawnguardButton6);
    ui->dawnguardTableWidget->setCellWidget(6,2, dawnguardButton7);
    ui->dawnguardTableWidget->setCellWidget(7,2, dawnguardButton8);
    ui->dawnguardTableWidget->setCellWidget(8,2, dawnguardButton9);
    ui->dawnguardTableWidget->setCellWidget(9,2, dawnguardButton10);
    ui->dawnguardTableWidget->setCellWidget(10,2, dawnguardButton11);
    ui->dawnguardTableWidget->setCellWidget(11,2, dawnguardButton12);
    ui->dawnguardTableWidget->setCellWidget(12,2, dawnguardButton13);
    ui->dawnguardTableWidget->setCellWidget(0,3, dawnguardPlayButton1);
    ui->dawnguardTableWidget->setCellWidget(1,3, dawnguardPlayButton2);
    ui->dawnguardTableWidget->setCellWidget(2,3, dawnguardPlayButton3);
    ui->dawnguardTableWidget->setCellWidget(3,3, dawnguardPlayButton4);
    ui->dawnguardTableWidget->setCellWidget(4,3, dawnguardPlayButton5);
    ui->dawnguardTableWidget->setCellWidget(5,3, dawnguardPlayButton6);
    ui->dawnguardTableWidget->setCellWidget(6,3, dawnguardPlayButton7);
    ui->dawnguardTableWidget->setCellWidget(7,3, dawnguardPlayButton8);
    ui->dawnguardTableWidget->setCellWidget(8,3, dawnguardPlayButton9);
    ui->dawnguardTableWidget->setCellWidget(9,3, dawnguardPlayButton10);
    ui->dawnguardTableWidget->setCellWidget(10,3, dawnguardPlayButton11);
    ui->dawnguardTableWidget->setCellWidget(11,3, dawnguardPlayButton12);
    ui->dawnguardTableWidget->setCellWidget(12,3, dawnguardPlayButton13);

    // Insert Buttons Dragonborn
    ui->dragonbornTableWidget->setCellWidget(0,2, dragonbornButton1);
    ui->dragonbornTableWidget->setCellWidget(1,2, dragonbornButton2);
    ui->dragonbornTableWidget->setCellWidget(2,2, dragonbornButton3);
    ui->dragonbornTableWidget->setCellWidget(3,2, dragonbornButton4);
    ui->dragonbornTableWidget->setCellWidget(4,2, dragonbornButton5);
    ui->dragonbornTableWidget->setCellWidget(5,2, dragonbornButton6);
    ui->dragonbornTableWidget->setCellWidget(6,2, dragonbornButton7);
    ui->dragonbornTableWidget->setCellWidget(7,2, dragonbornButton8);
    ui->dragonbornTableWidget->setCellWidget(8,2, dragonbornButton9);
    ui->dragonbornTableWidget->setCellWidget(9,2, dragonbornButton10);
    ui->dragonbornTableWidget->setCellWidget(10,2, dragonbornButton11);
    ui->dragonbornTableWidget->setCellWidget(11,2, dragonbornButton12);
    ui->dragonbornTableWidget->setCellWidget(12,2, dragonbornButton13);
    ui->dragonbornTableWidget->setCellWidget(13,2, dragonbornButton14);

    ui->dragonbornTableWidget->setCellWidget(0,3, dragonbornPlayButton1);
    ui->dragonbornTableWidget->setCellWidget(1,3, dragonbornPlayButton2);
    ui->dragonbornTableWidget->setCellWidget(2,3, dragonbornPlayButton3);
    ui->dragonbornTableWidget->setCellWidget(3,3, dragonbornPlayButton4);
    ui->dragonbornTableWidget->setCellWidget(4,3, dragonbornPlayButton5);
    ui->dragonbornTableWidget->setCellWidget(5,3, dragonbornPlayButton6);
    ui->dragonbornTableWidget->setCellWidget(6,3, dragonbornPlayButton7);
    ui->dragonbornTableWidget->setCellWidget(7,3, dragonbornPlayButton8);
    ui->dragonbornTableWidget->setCellWidget(8,3, dragonbornPlayButton9);
    ui->dragonbornTableWidget->setCellWidget(9,3, dragonbornPlayButton10);
    ui->dragonbornTableWidget->setCellWidget(10,3, dragonbornPlayButton11);
    ui->dragonbornTableWidget->setCellWidget(11,3, dragonbornPlayButton12);
    ui->dragonbornTableWidget->setCellWidget(12,3, dragonbornPlayButton13);
    ui->dragonbornTableWidget->setCellWidget(13,3, dragonbornPlayButton14);

    //Signal Combat Buttons
    connect(combatButton1, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton2, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton3, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton4, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton5, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton6, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton7, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton8, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton9, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton10, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton11, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton12, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton13, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton14, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton15, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton16, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton17, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));
    connect(combatButton18, SIGNAL(clicked()), &ButtonSignalMapperCombat, SLOT(map()));

    connect(combatPlay1, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay2, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay3, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay4, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay5, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay6, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay7, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay8, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay9, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay10, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay11, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay12, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay13, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay14, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay15, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay16, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay17, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));
    connect(combatPlay18, SIGNAL(clicked()), &ButtonSignalMapperAudioCombat, SLOT(map()));

    //Signal Dungeon Buttons
    connect(dungeonButton1, SIGNAL(clicked()), &ButtonSignalMapperDungeon, SLOT(map()));
    connect(dungeonButton2, SIGNAL(clicked()), &ButtonSignalMapperDungeon, SLOT(map()));
    connect(dungeonButton3, SIGNAL(clicked()), &ButtonSignalMapperDungeon, SLOT(map()));
    connect(dungeonButton4, SIGNAL(clicked()), &ButtonSignalMapperDungeon, SLOT(map()));
    connect(dungeonButton5, SIGNAL(clicked()), &ButtonSignalMapperDungeon, SLOT(map()));
    connect(dungeonButton6, SIGNAL(clicked()), &ButtonSignalMapperDungeon, SLOT(map()));
    connect(dungeonButton7, SIGNAL(clicked()), &ButtonSignalMapperDungeon, SLOT(map()));
    connect(dungeonButton8, SIGNAL(clicked()), &ButtonSignalMapperDungeon, SLOT(map()));
    connect(dungeonButton9, SIGNAL(clicked()), &ButtonSignalMapperDungeon, SLOT(map()));
    connect(dungeonButton10, SIGNAL(clicked()), &ButtonSignalMapperDungeon, SLOT(map()));

    connect(dungeonPlay1, SIGNAL(clicked()), &ButtonSignalMapperAudioDungeon, SLOT(map()));
    connect(dungeonPlay2, SIGNAL(clicked()), &ButtonSignalMapperAudioDungeon, SLOT(map()));
    connect(dungeonPlay3, SIGNAL(clicked()), &ButtonSignalMapperAudioDungeon, SLOT(map()));
    connect(dungeonPlay5, SIGNAL(clicked()), &ButtonSignalMapperAudioDungeon, SLOT(map()));
    connect(dungeonPlay4, SIGNAL(clicked()), &ButtonSignalMapperAudioDungeon, SLOT(map()));
    connect(dungeonPlay6, SIGNAL(clicked()), &ButtonSignalMapperAudioDungeon, SLOT(map()));
    connect(dungeonPlay7, SIGNAL(clicked()), &ButtonSignalMapperAudioDungeon, SLOT(map()));
    connect(dungeonPlay8, SIGNAL(clicked()), &ButtonSignalMapperAudioDungeon, SLOT(map()));
    connect(dungeonPlay9, SIGNAL(clicked()), &ButtonSignalMapperAudioDungeon, SLOT(map()));
    connect(dungeonPlay10, SIGNAL(clicked()), &ButtonSignalMapperAudioDungeon, SLOT(map()));

    //Signal Dread Buttons
    connect(dreadButton1, SIGNAL(clicked()), &ButtonSignalMapperDread, SLOT(map()));
    connect(dreadButton2, SIGNAL(clicked()), &ButtonSignalMapperDread, SLOT(map()));
    connect(dreadButton3, SIGNAL(clicked()), &ButtonSignalMapperDread, SLOT(map()));
    connect(dreadButton4, SIGNAL(clicked()), &ButtonSignalMapperDread, SLOT(map()));
    connect(dreadButton5, SIGNAL(clicked()), &ButtonSignalMapperDread, SLOT(map()));
    connect(dreadButton6, SIGNAL(clicked()), &ButtonSignalMapperDread, SLOT(map()));
    connect(dreadButton7, SIGNAL(clicked()), &ButtonSignalMapperDread, SLOT(map()));
    connect(dreadButton8, SIGNAL(clicked()), &ButtonSignalMapperDread, SLOT(map()));

    connect(dreadPlay1, SIGNAL(clicked()), &ButtonSignalMapperAudioDread, SLOT(map()));
    connect(dreadPlay2, SIGNAL(clicked()), &ButtonSignalMapperAudioDread, SLOT(map()));
    connect(dreadPlay3, SIGNAL(clicked()), &ButtonSignalMapperAudioDread, SLOT(map()));
    connect(dreadPlay4, SIGNAL(clicked()), &ButtonSignalMapperAudioDread, SLOT(map()));
    connect(dreadPlay5, SIGNAL(clicked()), &ButtonSignalMapperAudioDread, SLOT(map()));
    connect(dreadPlay6, SIGNAL(clicked()), &ButtonSignalMapperAudioDread, SLOT(map()));
    connect(dreadPlay7, SIGNAL(clicked()), &ButtonSignalMapperAudioDread, SLOT(map()));
    connect(dreadPlay8, SIGNAL(clicked()), &ButtonSignalMapperAudioDread, SLOT(map()));


    //Signal Explore Buttons
    connect(exploreButton1, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton2, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton3, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton4, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton5, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton6, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton7, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton8, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton9, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton10, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton11, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton12, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton13, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton14, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton15, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton16, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton17, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton18, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton19, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));
    connect(exploreButton20, SIGNAL(clicked()), &ButtonSignalMapperExplore, SLOT(map()));

    connect(explorePlay1, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay2, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay3, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay4, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay5, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay6, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay7, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay8, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay9, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay10, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay11, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay12, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay13, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay14, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay15, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay16, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay17, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay18, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay19, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));
    connect(explorePlay20, SIGNAL(clicked()), &ButtonSignalMapperAudioExplore, SLOT(map()));

    //Signal Other Buttons
    connect(otherButton1, SIGNAL(clicked()), &ButtonSignalMapperOther, SLOT(map()));
    connect(otherButton2, SIGNAL(clicked()), &ButtonSignalMapperOther, SLOT(map()));
    connect(otherButton3, SIGNAL(clicked()), &ButtonSignalMapperOther, SLOT(map()));
    connect(otherButton4, SIGNAL(clicked()), &ButtonSignalMapperOther, SLOT(map()));
    connect(otherButton5, SIGNAL(clicked()), &ButtonSignalMapperOther, SLOT(map()));
    connect(otherButton6, SIGNAL(clicked()), &ButtonSignalMapperOther, SLOT(map()));
    connect(otherButton7, SIGNAL(clicked()), &ButtonSignalMapperOther, SLOT(map()));
    connect(otherButton8, SIGNAL(clicked()), &ButtonSignalMapperOther, SLOT(map()));
    connect(otherButton9, SIGNAL(clicked()), &ButtonSignalMapperOther, SLOT(map()));
    connect(otherButton10, SIGNAL(clicked()), &ButtonSignalMapperOther, SLOT(map()));

    connect(otherPlay1, SIGNAL(clicked()), &ButtonSignalMapperAudioOther, SLOT(map()));
    connect(otherPlay2, SIGNAL(clicked()), &ButtonSignalMapperAudioOther, SLOT(map()));
    connect(otherPlay3, SIGNAL(clicked()), &ButtonSignalMapperAudioOther, SLOT(map()));
    connect(otherPlay4, SIGNAL(clicked()), &ButtonSignalMapperAudioOther, SLOT(map()));
    connect(otherPlay5, SIGNAL(clicked()), &ButtonSignalMapperAudioOther, SLOT(map()));
    connect(otherPlay6, SIGNAL(clicked()), &ButtonSignalMapperAudioOther, SLOT(map()));
    connect(otherPlay7, SIGNAL(clicked()), &ButtonSignalMapperAudioOther, SLOT(map()));
    connect(otherPlay8, SIGNAL(clicked()), &ButtonSignalMapperAudioOther, SLOT(map()));
    connect(otherPlay9, SIGNAL(clicked()), &ButtonSignalMapperAudioOther, SLOT(map()));
    connect(otherPlay10, SIGNAL(clicked()), &ButtonSignalMapperAudioOther, SLOT(map()));

    //Signal Reward Buttons
    connect(rewardButton1, SIGNAL(clicked()), &ButtonSignalMapperReward, SLOT(map()));
    connect(rewardButton2, SIGNAL(clicked()), &ButtonSignalMapperReward, SLOT(map()));
    connect(rewardButton3, SIGNAL(clicked()), &ButtonSignalMapperReward, SLOT(map()));
    connect(rewardButton4, SIGNAL(clicked()), &ButtonSignalMapperReward, SLOT(map()));

    connect(rewardPlay1, SIGNAL(clicked()), &ButtonSignalMapperAudioReward, SLOT(map()));
    connect(rewardPlay2, SIGNAL(clicked()), &ButtonSignalMapperAudioReward, SLOT(map()));
    connect(rewardPlay3, SIGNAL(clicked()), &ButtonSignalMapperAudioReward, SLOT(map()));
    connect(rewardPlay4, SIGNAL(clicked()), &ButtonSignalMapperAudioReward, SLOT(map()));

    //Signal Reveal Buttons
    connect(revealButton1, SIGNAL(clicked()), &ButtonSignalMapperReveal, SLOT(map()));
    connect(revealButton2, SIGNAL(clicked()), &ButtonSignalMapperReveal, SLOT(map()));
    connect(revealButton3, SIGNAL(clicked()), &ButtonSignalMapperReveal, SLOT(map()));
    connect(revealButton4, SIGNAL(clicked()), &ButtonSignalMapperReveal, SLOT(map()));

    connect(revealPlay1, SIGNAL(clicked()), &ButtonSignalMapperAudioReveal, SLOT(map()));
    connect(revealPlay2, SIGNAL(clicked()), &ButtonSignalMapperAudioReveal, SLOT(map()));
    connect(revealPlay3, SIGNAL(clicked()), &ButtonSignalMapperAudioReveal, SLOT(map()));
    connect(revealPlay4, SIGNAL(clicked()), &ButtonSignalMapperAudioReveal, SLOT(map()));

    //Signal Special Buttons
    connect(specialButton1, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton2, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton3, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton4, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton5, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton6, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton7, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton8, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton9, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton10, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton11, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton12, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));
    connect(specialButton13, SIGNAL(clicked()), &ButtonSignalMapperSpecial, SLOT(map()));

    connect(specialPlay1, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay2, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay3, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay4, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay5, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay6, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay7, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay8, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay9, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay10, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay11, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay12, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));
    connect(specialPlay13, SIGNAL(clicked()), &ButtonSignalMapperAudioSpecial, SLOT(map()));

    //Signal Stinger Buttons
    connect(stingerButton1, SIGNAL(clicked()), &ButtonSignalMapperStinger, SLOT(map()));
    connect(stingerButton2, SIGNAL(clicked()), &ButtonSignalMapperStinger, SLOT(map()));
    connect(stingerButton3, SIGNAL(clicked()), &ButtonSignalMapperStinger, SLOT(map()));
    connect(stingerButton4, SIGNAL(clicked()), &ButtonSignalMapperStinger, SLOT(map()));

    connect(stingerPlay1, SIGNAL(clicked()), &ButtonSignalMapperAudioStinger, SLOT(map()));
    connect(stingerPlay2, SIGNAL(clicked()), &ButtonSignalMapperAudioStinger, SLOT(map()));
    connect(stingerPlay3, SIGNAL(clicked()), &ButtonSignalMapperAudioStinger, SLOT(map()));
    connect(stingerPlay4, SIGNAL(clicked()), &ButtonSignalMapperAudioStinger, SLOT(map()));

    //Signal Tavern Buttons
    connect(tavernButton1, SIGNAL(clicked()), &ButtonSignalMapperTavern, SLOT(map()));
    connect(tavernButton2, SIGNAL(clicked()), &ButtonSignalMapperTavern, SLOT(map()));
    connect(tavernButton3, SIGNAL(clicked()), &ButtonSignalMapperTavern, SLOT(map()));
    connect(tavernButton4, SIGNAL(clicked()), &ButtonSignalMapperTavern, SLOT(map()));
    connect(tavernButton5, SIGNAL(clicked()), &ButtonSignalMapperTavern, SLOT(map()));

    connect(tavernPlay1, SIGNAL(clicked()), &ButtonSignalMapperAudioTavern, SLOT(map()));
    connect(tavernPlay2, SIGNAL(clicked()), &ButtonSignalMapperAudioTavern, SLOT(map()));
    connect(tavernPlay3, SIGNAL(clicked()), &ButtonSignalMapperAudioTavern, SLOT(map()));
    connect(tavernPlay4, SIGNAL(clicked()), &ButtonSignalMapperAudioTavern, SLOT(map()));
    connect(tavernPlay5, SIGNAL(clicked()), &ButtonSignalMapperAudioTavern, SLOT(map()));

    //Signal Town Buttons
    connect(townButton1, SIGNAL(clicked()), &ButtonSignalMapperTown, SLOT(map()));
    connect(townButton2, SIGNAL(clicked()), &ButtonSignalMapperTown, SLOT(map()));
    connect(townButton3, SIGNAL(clicked()), &ButtonSignalMapperTown, SLOT(map()));
    connect(townButton4, SIGNAL(clicked()), &ButtonSignalMapperTown, SLOT(map()));
    connect(townButton5, SIGNAL(clicked()), &ButtonSignalMapperTown, SLOT(map()));
    connect(townButton6, SIGNAL(clicked()), &ButtonSignalMapperTown, SLOT(map()));
    connect(townButton7, SIGNAL(clicked()), &ButtonSignalMapperTown, SLOT(map()));

    connect(townPlay1, SIGNAL(clicked()), &ButtonSignalMapperAudioTown, SLOT(map()));
    connect(townPlay2, SIGNAL(clicked()), &ButtonSignalMapperAudioTown, SLOT(map()));
    connect(townPlay3, SIGNAL(clicked()), &ButtonSignalMapperAudioTown, SLOT(map()));
    connect(townPlay4, SIGNAL(clicked()), &ButtonSignalMapperAudioTown, SLOT(map()));
    connect(townPlay5, SIGNAL(clicked()), &ButtonSignalMapperAudioTown, SLOT(map()));
    connect(townPlay6, SIGNAL(clicked()), &ButtonSignalMapperAudioTown, SLOT(map()));
    connect(townPlay7, SIGNAL(clicked()), &ButtonSignalMapperAudioTown, SLOT(map()));

    //Signal Dawnguard Buttons
    connect(dawnguardButton1, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton2, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton3, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton4, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton5, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton6, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton7, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton8, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton9, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton10, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton11, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton12, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));
    connect(dawnguardButton13, SIGNAL(clicked()), &ButtonSignalMapperDawnguard, SLOT(map()));

    connect(dawnguardPlayButton1, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton2, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton3, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton4, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton5, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton6, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton7, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton8, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton9, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton10, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton11, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton12, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));
    connect(dawnguardPlayButton13, SIGNAL(clicked()), &ButtonSignalMapperAudioDawnguard, SLOT(map()));

    //Signal Explore Buttons
    connect(dragonbornButton1, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton2, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton3, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton4, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton5, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton6, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton7, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton8, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton9, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton10, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton11, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton12, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton13, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));
    connect(dragonbornButton14, SIGNAL(clicked()), &ButtonSignalMapperDragonborn, SLOT(map()));

    connect(dragonbornPlayButton1, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton2, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton3, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton4, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton5, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton6, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton7, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton8, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton9, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton10, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton11, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton12, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton13, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));
    connect(dragonbornPlayButton14, SIGNAL(clicked()), &ButtonSignalMapperAudioDragonborn, SLOT(map()));

    //Mapper Combat
    ButtonSignalMapperCombat.setMapping(combatButton1, 0);
    ButtonSignalMapperCombat.setMapping(combatButton2, 1);
    ButtonSignalMapperCombat.setMapping(combatButton3, 2);
    ButtonSignalMapperCombat.setMapping(combatButton4, 3);
    ButtonSignalMapperCombat.setMapping(combatButton5, 4);
    ButtonSignalMapperCombat.setMapping(combatButton6, 5);
    ButtonSignalMapperCombat.setMapping(combatButton7, 6);
    ButtonSignalMapperCombat.setMapping(combatButton8, 7);
    ButtonSignalMapperCombat.setMapping(combatButton9, 8);
    ButtonSignalMapperCombat.setMapping(combatButton10, 9);
    ButtonSignalMapperCombat.setMapping(combatButton11, 10);
    ButtonSignalMapperCombat.setMapping(combatButton12, 11);
    ButtonSignalMapperCombat.setMapping(combatButton13, 12);
    ButtonSignalMapperCombat.setMapping(combatButton14, 13);
    ButtonSignalMapperCombat.setMapping(combatButton15, 14);
    ButtonSignalMapperCombat.setMapping(combatButton16, 15);
    ButtonSignalMapperCombat.setMapping(combatButton17, 16);
    ButtonSignalMapperCombat.setMapping(combatButton18, 17);

    ButtonSignalMapperAudioCombat.setMapping(combatPlay1, 0);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay2, 1);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay3, 2);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay4, 3);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay5, 4);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay6, 5);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay7, 6);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay8, 7);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay9, 8);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay10, 9);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay11, 10);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay12, 11);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay13, 12);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay14, 13);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay15, 14);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay16, 15);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay17, 16);
    ButtonSignalMapperAudioCombat.setMapping(combatPlay18, 17);

    //Mapper Dungeon
    ButtonSignalMapperDungeon.setMapping(dungeonButton1, 0);
    ButtonSignalMapperDungeon.setMapping(dungeonButton2, 1);
    ButtonSignalMapperDungeon.setMapping(dungeonButton3, 2);
    ButtonSignalMapperDungeon.setMapping(dungeonButton4, 3);
    ButtonSignalMapperDungeon.setMapping(dungeonButton5, 4);
    ButtonSignalMapperDungeon.setMapping(dungeonButton6, 5);
    ButtonSignalMapperDungeon.setMapping(dungeonButton7, 6);
    ButtonSignalMapperDungeon.setMapping(dungeonButton8, 7);
    ButtonSignalMapperDungeon.setMapping(dungeonButton9, 8);
    ButtonSignalMapperDungeon.setMapping(dungeonButton10, 9);

    ButtonSignalMapperAudioDungeon.setMapping(dungeonPlay1, 0);
    ButtonSignalMapperAudioDungeon.setMapping(dungeonPlay2, 1);
    ButtonSignalMapperAudioDungeon.setMapping(dungeonPlay3, 2);
    ButtonSignalMapperAudioDungeon.setMapping(dungeonPlay4, 3);
    ButtonSignalMapperAudioDungeon.setMapping(dungeonPlay5, 4);
    ButtonSignalMapperAudioDungeon.setMapping(dungeonPlay6, 5);
    ButtonSignalMapperAudioDungeon.setMapping(dungeonPlay7, 6);
    ButtonSignalMapperAudioDungeon.setMapping(dungeonPlay8, 7);
    ButtonSignalMapperAudioDungeon.setMapping(dungeonPlay9, 8);
    ButtonSignalMapperAudioDungeon.setMapping(dungeonPlay10, 9);

    //Mapper Dread
    ButtonSignalMapperDread.setMapping(dreadButton1, 0);
    ButtonSignalMapperDread.setMapping(dreadButton2, 1);
    ButtonSignalMapperDread.setMapping(dreadButton3, 2);
    ButtonSignalMapperDread.setMapping(dreadButton4, 3);
    ButtonSignalMapperDread.setMapping(dreadButton5, 4);
    ButtonSignalMapperDread.setMapping(dreadButton6, 5);
    ButtonSignalMapperDread.setMapping(dreadButton7, 6);
    ButtonSignalMapperDread.setMapping(dreadButton8, 7);

    ButtonSignalMapperAudioDread.setMapping(dreadPlay1, 0);
    ButtonSignalMapperAudioDread.setMapping(dreadPlay2, 1);
    ButtonSignalMapperAudioDread.setMapping(dreadPlay3, 2);
    ButtonSignalMapperAudioDread.setMapping(dreadPlay4, 3);
    ButtonSignalMapperAudioDread.setMapping(dreadPlay5, 4);
    ButtonSignalMapperAudioDread.setMapping(dreadPlay6, 5);
    ButtonSignalMapperAudioDread.setMapping(dreadPlay7, 6);
    ButtonSignalMapperAudioDread.setMapping(dreadPlay8, 7);

    //Mapper Explore
    ButtonSignalMapperExplore.setMapping(exploreButton1, 0);
    ButtonSignalMapperExplore.setMapping(exploreButton2, 1);
    ButtonSignalMapperExplore.setMapping(exploreButton3, 2);
    ButtonSignalMapperExplore.setMapping(exploreButton4, 3);
    ButtonSignalMapperExplore.setMapping(exploreButton5, 4);
    ButtonSignalMapperExplore.setMapping(exploreButton6, 5);
    ButtonSignalMapperExplore.setMapping(exploreButton7, 6);
    ButtonSignalMapperExplore.setMapping(exploreButton8, 7);
    ButtonSignalMapperExplore.setMapping(exploreButton9, 8);
    ButtonSignalMapperExplore.setMapping(exploreButton10, 9);
    ButtonSignalMapperExplore.setMapping(exploreButton11, 10);
    ButtonSignalMapperExplore.setMapping(exploreButton12, 11);
    ButtonSignalMapperExplore.setMapping(exploreButton13, 12);
    ButtonSignalMapperExplore.setMapping(exploreButton14, 13);
    ButtonSignalMapperExplore.setMapping(exploreButton15, 14);
    ButtonSignalMapperExplore.setMapping(exploreButton16, 15);
    ButtonSignalMapperExplore.setMapping(exploreButton17, 16);
    ButtonSignalMapperExplore.setMapping(exploreButton18, 17);
    ButtonSignalMapperExplore.setMapping(exploreButton19, 18);
    ButtonSignalMapperExplore.setMapping(exploreButton20, 19);

    ButtonSignalMapperAudioExplore.setMapping(explorePlay1, 0);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay2, 1);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay3, 2);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay4, 3);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay5, 4);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay6, 5);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay7, 6);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay8, 7);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay9, 8);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay10, 9);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay11, 10);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay12, 11);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay13, 12);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay14, 13);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay15, 14);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay16, 15);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay17, 16);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay18, 17);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay19, 18);
    ButtonSignalMapperAudioExplore.setMapping(explorePlay20, 19);

    //Mapper Other
    ButtonSignalMapperOther.setMapping(otherButton1, 0);
    ButtonSignalMapperOther.setMapping(otherButton2, 1);
    ButtonSignalMapperOther.setMapping(otherButton3, 2);
    ButtonSignalMapperOther.setMapping(otherButton4, 3);
    ButtonSignalMapperOther.setMapping(otherButton5, 4);
    ButtonSignalMapperOther.setMapping(otherButton6, 5);
    ButtonSignalMapperOther.setMapping(otherButton7, 6);
    ButtonSignalMapperOther.setMapping(otherButton8, 7);
    ButtonSignalMapperOther.setMapping(otherButton9, 8);
    ButtonSignalMapperOther.setMapping(otherButton10, 9);

    ButtonSignalMapperAudioOther.setMapping(otherPlay1, 0);
    ButtonSignalMapperAudioOther.setMapping(otherPlay2, 1);
    ButtonSignalMapperAudioOther.setMapping(otherPlay3, 2);
    ButtonSignalMapperAudioOther.setMapping(otherPlay4, 3);
    ButtonSignalMapperAudioOther.setMapping(otherPlay5, 4);
    ButtonSignalMapperAudioOther.setMapping(otherPlay6, 5);
    ButtonSignalMapperAudioOther.setMapping(otherPlay7, 6);
    ButtonSignalMapperAudioOther.setMapping(otherPlay8, 7);
    ButtonSignalMapperAudioOther.setMapping(otherPlay9, 8);
    ButtonSignalMapperAudioOther.setMapping(otherPlay10, 9);

    //Mapper Reward
    ButtonSignalMapperReward.setMapping(rewardButton1, 0);
    ButtonSignalMapperReward.setMapping(rewardButton2, 1);
    ButtonSignalMapperReward.setMapping(rewardButton3, 2);
    ButtonSignalMapperReward.setMapping(rewardButton4, 3);

    ButtonSignalMapperAudioReward.setMapping(rewardPlay1, 0);
    ButtonSignalMapperAudioReward.setMapping(rewardPlay2, 1);
    ButtonSignalMapperAudioReward.setMapping(rewardPlay3, 2);
    ButtonSignalMapperAudioReward.setMapping(rewardPlay4, 3);

    //Mapper Reveal
    ButtonSignalMapperReveal.setMapping(revealButton1, 0);
    ButtonSignalMapperReveal.setMapping(revealButton2, 1);
    ButtonSignalMapperReveal.setMapping(revealButton3, 2);
    ButtonSignalMapperReveal.setMapping(revealButton4, 3);

    ButtonSignalMapperAudioReveal.setMapping(revealPlay1, 0);
    ButtonSignalMapperAudioReveal.setMapping(revealPlay2, 1);
    ButtonSignalMapperAudioReveal.setMapping(revealPlay3, 2);
    ButtonSignalMapperAudioReveal.setMapping(revealPlay4, 3);

    //Mapper Reveal
    ButtonSignalMapperStinger.setMapping(stingerButton1, 0);
    ButtonSignalMapperStinger.setMapping(stingerButton2, 1);
    ButtonSignalMapperStinger.setMapping(stingerButton3, 2);
    ButtonSignalMapperStinger.setMapping(stingerButton4, 3);

    ButtonSignalMapperAudioStinger.setMapping(stingerPlay1, 0);
    ButtonSignalMapperAudioStinger.setMapping(stingerPlay2, 1);
    ButtonSignalMapperAudioStinger.setMapping(stingerPlay3, 2);
    ButtonSignalMapperAudioStinger.setMapping(stingerPlay4, 3);

    //Mapper Special
    ButtonSignalMapperSpecial.setMapping(specialButton1, 0);
    ButtonSignalMapperSpecial.setMapping(specialButton2, 1);
    ButtonSignalMapperSpecial.setMapping(specialButton3, 2);
    ButtonSignalMapperSpecial.setMapping(specialButton4, 3);
    ButtonSignalMapperSpecial.setMapping(specialButton5, 4);
    ButtonSignalMapperSpecial.setMapping(specialButton6, 5);
    ButtonSignalMapperSpecial.setMapping(specialButton7, 6);
    ButtonSignalMapperSpecial.setMapping(specialButton8, 7);
    ButtonSignalMapperSpecial.setMapping(specialButton9, 8);
    ButtonSignalMapperSpecial.setMapping(specialButton10, 9);
    ButtonSignalMapperSpecial.setMapping(specialButton11, 10);
    ButtonSignalMapperSpecial.setMapping(specialButton12, 11);
    ButtonSignalMapperSpecial.setMapping(specialButton13, 12);

    ButtonSignalMapperAudioSpecial.setMapping(specialPlay1, 0);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay2, 1);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay3, 2);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay4, 3);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay5, 4);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay6, 5);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay7, 6);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay8, 7);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay9, 8);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay10, 9);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay11, 10);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay12, 11);
    ButtonSignalMapperAudioSpecial.setMapping(specialPlay13, 12);

    //Mapper Tavern
    ButtonSignalMapperTavern.setMapping(tavernButton1, 0);
    ButtonSignalMapperTavern.setMapping(tavernButton2, 1);
    ButtonSignalMapperTavern.setMapping(tavernButton3, 2);
    ButtonSignalMapperTavern.setMapping(tavernButton4, 3);
    ButtonSignalMapperTavern.setMapping(tavernButton5, 4);

    ButtonSignalMapperAudioTavern.setMapping(tavernPlay1, 0);
    ButtonSignalMapperAudioTavern.setMapping(tavernPlay2, 1);
    ButtonSignalMapperAudioTavern.setMapping(tavernPlay3, 2);
    ButtonSignalMapperAudioTavern.setMapping(tavernPlay4, 3);
    ButtonSignalMapperAudioTavern.setMapping(tavernPlay5, 4);

    //Mapper Town
    ButtonSignalMapperTown.setMapping(townButton1, 0);
    ButtonSignalMapperTown.setMapping(townButton2, 1);
    ButtonSignalMapperTown.setMapping(townButton3, 2);
    ButtonSignalMapperTown.setMapping(townButton4, 3);
    ButtonSignalMapperTown.setMapping(townButton5, 4);
    ButtonSignalMapperTown.setMapping(townButton6, 5);
    ButtonSignalMapperTown.setMapping(townButton7, 6);

    ButtonSignalMapperAudioTown.setMapping(townPlay1, 0);
    ButtonSignalMapperAudioTown.setMapping(townPlay2, 1);
    ButtonSignalMapperAudioTown.setMapping(townPlay3, 2);
    ButtonSignalMapperAudioTown.setMapping(townPlay4, 3);
    ButtonSignalMapperAudioTown.setMapping(townPlay5, 4);
    ButtonSignalMapperAudioTown.setMapping(townPlay6, 5);
    ButtonSignalMapperAudioTown.setMapping(townPlay7, 6);

    //Mapper Dawnguard
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton1, 0);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton2, 1);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton3, 2);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton4, 3);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton5, 4);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton6, 5);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton7, 6);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton8, 7);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton9, 8);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton10, 9);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton11, 10);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton12, 11);
    ButtonSignalMapperDawnguard.setMapping(dawnguardButton13, 12);

    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton1, 0);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton2, 1);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton3, 2);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton4, 3);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton5, 4);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton6, 5);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton7, 6);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton8, 7);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton9, 8);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton10, 9);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton11, 10);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton12, 11);
    ButtonSignalMapperAudioDawnguard.setMapping(dawnguardPlayButton13, 12);

    //Mapper Dragonborn
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton1, 0);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton2, 1);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton3, 2);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton4, 3);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton5, 4);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton6, 5);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton7, 6);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton8, 7);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton9, 8);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton10, 9);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton11, 10);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton12, 11);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton13, 12);
    ButtonSignalMapperDragonborn.setMapping(dragonbornButton14, 13);

    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton1, 0);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton2, 1);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton3, 2);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton4, 3);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton5, 4);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton6, 5);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton7, 6);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton8, 7);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton9, 8);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton10, 9);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton11, 10);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton12, 11);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton13, 12);
    ButtonSignalMapperAudioDragonborn.setMapping(dragonbornPlayButton14, 13);


    connect(&ButtonSignalMapperCombat, SIGNAL(mapped(int)), this, SLOT(CombatButtonClicked(int)));
    connect(&ButtonSignalMapperAudioCombat, SIGNAL(mapped(int)), this, SLOT(CombatPlayButtonClicked(int)));

    connect(&ButtonSignalMapperDungeon, SIGNAL(mapped(int)), this, SLOT(DungeonButtonClicked(int)));
    connect(&ButtonSignalMapperAudioDungeon, SIGNAL(mapped(int)), this, SLOT(DungeonPlayButtonClicked(int)));

    connect(&ButtonSignalMapperDread, SIGNAL(mapped(int)), this, SLOT(DreadButtonClicked(int)));
    connect(&ButtonSignalMapperAudioDread, SIGNAL(mapped(int)), this, SLOT(DreadPlayButtonClicked(int)));

    connect(&ButtonSignalMapperExplore, SIGNAL(mapped(int)), this, SLOT(ExploreButtonClicked(int)));
    connect(&ButtonSignalMapperAudioExplore, SIGNAL(mapped(int)), this, SLOT(ExplorePlayButtonClicked(int)));

    connect(&ButtonSignalMapperOther, SIGNAL(mapped(int)), this, SLOT(OtherButtonClicked(int)));
    connect(&ButtonSignalMapperAudioOther, SIGNAL(mapped(int)), this, SLOT(OtherPlayButtonClicked(int)));

    connect(&ButtonSignalMapperReward, SIGNAL(mapped(int)), this, SLOT(RewardButtonClicked(int)));
    connect(&ButtonSignalMapperAudioReward, SIGNAL(mapped(int)), this, SLOT(RewardPlayButtonClicked(int)));

    connect(&ButtonSignalMapperReveal, SIGNAL(mapped(int)), this, SLOT(RevealButtonClicked(int)));
    connect(&ButtonSignalMapperAudioReveal, SIGNAL(mapped(int)), this, SLOT(RevealPlayButtonClicked(int)));

    connect(&ButtonSignalMapperStinger, SIGNAL(mapped(int)), this, SLOT(StingerButtonClicked(int)));
    connect(&ButtonSignalMapperAudioStinger, SIGNAL(mapped(int)), this, SLOT(StingerPlayButtonClicked(int)));

    connect(&ButtonSignalMapperSpecial, SIGNAL(mapped(int)), this, SLOT(SpecialButtonClicked(int)));
    connect(&ButtonSignalMapperAudioSpecial, SIGNAL(mapped(int)), this, SLOT(SpecialPlayButtonClicked(int)));

    connect(&ButtonSignalMapperTavern, SIGNAL(mapped(int)), this, SLOT(TavernButtonClicked(int)));
    connect(&ButtonSignalMapperAudioTavern, SIGNAL(mapped(int)), this, SLOT(TavernPlayButtonClicked(int)));

    connect(&ButtonSignalMapperTown, SIGNAL(mapped(int)), this, SLOT(TownButtonClicked(int)));
    connect(&ButtonSignalMapperAudioTown, SIGNAL(mapped(int)), this, SLOT(TownPlayButtonClicked(int)));

    connect(&ButtonSignalMapperDawnguard, SIGNAL(mapped(int)), this, SLOT(DawnguardButtonClicked(int)));
    connect(&ButtonSignalMapperAudioDawnguard, SIGNAL(mapped(int)), this, SLOT(DawnguardPlayButtonClicked(int)));

    connect(&ButtonSignalMapperDragonborn, SIGNAL(mapped(int)), this, SLOT(DragonbornButtonClicked(int)));
    connect(&ButtonSignalMapperAudioDragonborn, SIGNAL(mapped(int)), this, SLOT(DragonbornPlayButtonClicked(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
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
    if(player->state() == QMediaPlayer::PlayingState) {
        player->stop();
        player->setMedia(QUrl("qrc:/audio/audio/combat/"+name+".mp3"));
        player->play();
    } else {
        player->setMedia(QUrl("qrc:/audio/audio/combat/"+name+".mp3"));
        player->play();
    }
}

void MainWindow::DungeonPlayButtonClicked(int row)
{
    QString name = ui->dungeonTableWidget->item(row,0)->text();
    QString replace = name;
    if(replace.left(4) == "Bene") {
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/dungeon/Beneath the Ice.mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/dungeon/Beneath the Ice.mp3"));
            player->play();
        }
    } else {
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/dungeon/"+name+".mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/dungeon/"+name+".mp3"));
            player->play();
        }
    }
}

void MainWindow::DreadPlayButtonClicked(int row)
{
    QString name = ui->dreadTableWidget->item(row,0)->text();
    if(player->state() == QMediaPlayer::PlayingState) {
        player->stop();
        player->setMedia(QUrl("qrc:/audio/audio/dread/"+name+".mp3"));
        player->play();
    } else {
        player->setMedia(QUrl("qrc:/audio/audio/dread/"+name+".mp3"));
        player->play();
    }
}

void MainWindow::ExplorePlayButtonClicked(int row)
{
    QString name = ui->exploreTableWidget->item(row,0)->text();
    QString replace = name;
    if(replace.left(4) == "From") {        
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/explore/From Past to Present.mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/explore/From Past to Present.mp3"));
            player->play();
        }
    } else {
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/explore/"+name+".mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/explore/"+name+".mp3"));
            player->play();
        }
    }
}

void MainWindow::OtherPlayButtonClicked(int row)
{
    QString name = ui->otherTableWidget->item(row,0)->text();
    if(player->state() == QMediaPlayer::PlayingState) {
        player->stop();
        player->setMedia(QUrl("qrc:/audio/audio/other/"+name+".mp3"));
        player->play();
    } else {
        player->setMedia(QUrl("qrc:/audio/audio/other/"+name+".mp3"));
        player->play();
    }
}

void MainWindow::RewardPlayButtonClicked(int row)
{
    QString name = ui->rewardTableWidget->item(row,0)->text();
    if(player->state() == QMediaPlayer::PlayingState) {
        player->stop();
        player->setMedia(QUrl("qrc:/audio/audio/reward/"+name+".mp3"));
        player->play();
    } else {
        player->setMedia(QUrl("qrc:/audio/audio/reward/"+name+".mp3"));
        player->play();
    }
}

void MainWindow::RevealPlayButtonClicked(int row)
{
    QString name = ui->revealTableWidget->item(row,0)->text();
    if(player->state() == QMediaPlayer::PlayingState) {
        player->stop();
        player->setMedia(QUrl("qrc:/audio/audio/reveal/"+name+".mp3"));
        player->play();
    } else {
        player->setMedia(QUrl("qrc:/audio/audio/reveal/"+name+".mp3"));
        player->play();
    }
}

void MainWindow::StingerPlayButtonClicked(int row)
{
    QString name = ui->stingerTableWidget->item(row,0)->text();
    if(player->state() == QMediaPlayer::PlayingState) {
        player->stop();
        player->setMedia(QUrl("qrc:/audio/audio/stinger/"+name+".mp3"));
        player->play();
    } else {
        player->setMedia(QUrl("qrc:/audio/audio/stinger/"+name+".mp3"));
        player->play();
    }
}

void MainWindow::SpecialPlayButtonClicked(int row)
{
    QString name = ui->specialTableWidget->item(row,0)->text();
    QString replace = name;
    if(replace.left(7) == "Failure") {
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/special/failure/"+name+".mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/special/failure/"+name+".mp3"));
            player->play();
        }
    } else {
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/special/"+name+".mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/special/"+name+".mp3"));
            player->play();
        }
    }
}

void MainWindow::TavernPlayButtonClicked(int row)
{
    QString name = ui->tavernTableWidget->item(row,0)->text();
    if(player->state() == QMediaPlayer::PlayingState) {
        player->stop();
        player->setMedia(QUrl("qrc:/audio/audio/tavern/"+name+".mp3"));
        player->play();
    } else {
        player->setMedia(QUrl("qrc:/audio/audio/tavern/"+name+".mp3"));
        player->play();
    }
}

void MainWindow::TownPlayButtonClicked(int row)
{
    QString name = ui->townTableWidget->item(row,0)->text();
    QString replace = name;
    if(replace.left(4) == "From") {
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/explore/From Past to Present.mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/explore/From Past to Present.mp3"));
            player->play();
        }
    } else {
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/town/"+name+".mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/town/"+name+".mp3"));
            player->play();
        }
    }
}

void MainWindow::DawnguardPlayButtonClicked(int row)
{
    QString name = ui->dawnguardTableWidget->item(row,0)->text();
    QString replace = name;
    if(replace.left(3) == "The") {
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/dlc01/"+name+".mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/dlc01/"+name+".mp3"));
            player->play();
        }
    } else if(replace.left(4) == "Cast" || replace.left(4) == "Soul") {
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/dlc01/dungeon/"+name+".mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/dlc01/dungeon/"+name+".mp3"));
            player->play();
        }
    } else if(replace.left(4) == "Forg") {
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/dlc01/explore/falmervalley/"+name+".mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/dlc01/explore/falmervalley/"+name+".mp3"));
            player->play();
        }
    } else {
        if(player->state() == QMediaPlayer::PlayingState) {
            player->stop();
            player->setMedia(QUrl("qrc:/audio/audio/dlc01/explore/soulcairn/palette/"+name+".mp3"));
            player->play();
        } else {
            player->setMedia(QUrl("qrc:/audio/audio/dlc01/explore/soulcairn/palette/"+name+".mp3"));
            player->play();
        }
    }
}

void MainWindow::DragonbornPlayButtonClicked(int row)
{
    QString name = ui->dragonbornTableWidget->item(row,0)->text();
    QString replace = name;
    if(player->state() == QMediaPlayer::PlayingState) {
        player->stop();
        player->setMedia(QUrl("qrc:/audio/audio/dragonborn/audio/"+name+".mp3"));
        player->play();
    } else {
        player->setMedia(QUrl("qrc:/audio/audio/dragonborn/audio/"+name+".mp3"));
        player->play();
    }
}

void MainWindow::ProcessFile(QString type, QString reference)
{
    QString src = qApp->applicationDirPath();
    QString completeFile = QFileDialog::getOpenFileName(this, "Open File", QString(), "Audio (*.wav *.mp3 *.wma)");

    QDir pathDir;
    QString finalPath;
    QString xwmConverter = qApp->applicationDirPath()+"/encode.exe";
    QStringList argumentsXwm;
    QProcess *xwmencoder = new QProcess(this);

    if(type == "other" || type == "dragonborn") {
        pathDir = skyrimMusicRealPath;
        finalPath = skyrimMusicRealPath;
    } else if(type == "dlc01") {
        QString filenameDebut = reference.mid(4,7);
        if(filenameDebut == "dungeon") {
            pathDir = skyrimMusicRealPath+type+"/dungeon";
            finalPath = skyrimMusicRealPath + type + "/" + "dungeon" + "/";
        } else if(filenameDebut == "eclipse") {
            pathDir = skyrimMusicRealPath+type;
            finalPath = skyrimMusicRealPath + type + "/";
        } else if(filenameDebut == "explore") {
            pathDir = skyrimMusicRealPath+type+"/explore/falmervalley";
            finalPath = skyrimMusicRealPath + type + "/" + "explore" + "/" + "falmervalley" + "/";
        } else if(filenameDebut == "soulcai") {
            pathDir = skyrimMusicRealPath+type+"/explore/soulcairn/palette";
            finalPath = skyrimMusicRealPath + type + "/" + "explore" + "/" + "soulcairn" + "/" + "palette" + "/";
        }
    } else if(type == "dungeon") {
        QString fileSubCategory = reference.mid(12,2);
        if(fileSubCategory == "ic") {
            pathDir = skyrimMusicRealPath+type+"/ice";
            finalPath = skyrimMusicRealPath + type + "/" + "ice" + "/";
        } else if(fileSubCategory == "ca") {
            pathDir = skyrimMusicRealPath+type+"/cave";
            finalPath = skyrimMusicRealPath + type + "/" + "cave" + "/";
        } else if(fileSubCategory == "fo") {
            pathDir = skyrimMusicRealPath+type+"/fort";
            finalPath = skyrimMusicRealPath + type + "/" + "fort" + "/";
        } else {
            pathDir = skyrimMusicRealPath+type;
            finalPath = skyrimMusicRealPath + type + "/";
        }
    } else if(type == "special") {
        QString fileSubCategory = reference.mid(4,5);
        if(fileSubCategory == "failu") {
            pathDir = skyrimMusicRealPath+type+"/failure";
            finalPath = skyrimMusicRealPath + type + "/" + "failure" + "/";
        } else {
            pathDir = skyrimMusicRealPath+type;
            finalPath = skyrimMusicRealPath + type + "/";
        }

    } else {
        pathDir = skyrimMusicRealPath+type;
        finalPath = skyrimMusicRealPath + type + "/";
    }

    if(completeFile != "") {
        QFileInfo file(completeFile);
        QDir::setCurrent(file.absoluteFilePath());
        QString referenceWav = reference;
        QString refWav = referenceWav.replace(".xwm",".wav");
        QString fileExtension = completeFile.right(3);

        if(fileExtension != "wav") {
            QString wavConverter = qApp->applicationDirPath()+"/ffmpeg.exe";
            QStringList argumentsWav;
            QProcess *wavEncoder = new QProcess(this);
            argumentsWav << "-i" << completeFile << qApp->applicationDirPath() +"/" + refWav;
            wavEncoder->start(wavConverter,argumentsWav);
            wavEncoder->waitForFinished();
            argumentsXwm << qApp->applicationDirPath()+"/"+refWav << qApp->applicationDirPath()+"/"+reference;
            xwmencoder->start(xwmConverter,argumentsXwm);
            xwmencoder->waitForFinished();
        } else {
            QFile originalFile(completeFile);
            originalFile.copy(completeFile,src + "/" + refWav);
            argumentsXwm << completeFile << qApp->applicationDirPath()+"/"+reference;
            xwmencoder->start(xwmConverter,argumentsXwm);
            xwmencoder->waitForFinished();
        }

        QString skyrimDataPath = skyrimRealPath+"/Data/";
        QDir musicDir = skyrimMusicRealPath;
        QProcess *folderCreator = new QProcess(this);
        if(!musicDir.exists()) {
            QDir::setCurrent(skyrimDataPath);
            folderCreator->execute("cmd /c mkdir music");
        }
        if(!pathDir.exists()) {
            QDir::setCurrent(skyrimMusicRealPath);
            folderCreator->execute("cmd /c mkdir "+type);
            folderCreator->waitForFinished();
            if(type == "dlc01") {
                QDir::setCurrent(skyrimMusicRealPath + type + "/");
                folderCreator->execute("cmd /c mkdir dungeon");
                folderCreator->execute("cmd /c mkdir explore");
                QDir::setCurrent(skyrimMusicRealPath + type + "/explore/");
                folderCreator->execute("cmd /c mkdir falmervalley");
                folderCreator->execute("cmd /c mkdir soulcairn");
                QDir::setCurrent(skyrimMusicRealPath + type + "/explore/soulcairn/");
                folderCreator->execute("cmd /c mkdir palette");

            } else if(type == "dungeon") {
                QDir::setCurrent(skyrimMusicRealPath+type+"/");
                folderCreator->execute("cmd /c mkdir cave");
                folderCreator->execute("cmd /c mkdir fort");
                folderCreator->execute("cmd /c mkdir ice");
            } else if(type == "special") {
                QDir::setCurrent(skyrimMusicRealPath+type+"/");
                folderCreator->execute("cmd /c mkdir failure");
            }
        }
        folderCreator->waitForFinished();


        QFile finalFile(reference);
        QFile wavFile(refWav);
        wavFile.copy(src + "/" + refWav, src + "/" + "converted" + "/" + refWav);
        QMessageBox *preview = new QMessageBox(this);
        preview->setIcon(QMessageBox::Warning);
        preview->setWindowTitle("Attention");
        preview->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
        preview->setText(tr("If you want to get a preview of your new file, you may click \"Preview\". Otherwise, you can just click \"Replace\""));
        QAbstractButton *previewButton = preview->addButton(tr("Preview"), QMessageBox::ActionRole);
        QAbstractButton *stopMusic = preview->addButton(tr("Stop Music"), QMessageBox::ActionRole);
        QAbstractButton *replaceButton = preview->addButton(tr("Replace"), QMessageBox::ActionRole);
        previewButton->disconnect();
        stopMusic->disconnect();
        connect(previewButton, &QAbstractButton::clicked, this, [=](){
            if(player->state() == QMediaPlayer::PlayingState) {
                player->stop();
                player->setMedia(QUrl::fromLocalFile(qApp->applicationDirPath()+"/"+refWav));
                player->play();
            } else {
                player->setMedia(QUrl::fromLocalFile(qApp->applicationDirPath()+"/"+refWav));
                player->play();
            }
        });
        connect(stopMusic, &QAbstractButton::clicked, this, [=](){
            if(player->state() == QMediaPlayer::PlayingState) {
                player->stop();
            }
        });
        preview->exec();
        if(preview->clickedButton() == replaceButton) {
            player->stop();
            player->setMedia(NULL);
            QString boolWav = BoolToString(QFile::exists(qApp->applicationDirPath()+"/"+refWav));
            finalFile.copy(src + "/" + reference, finalPath + reference);
            finalFile.remove(src + "/" + reference);
            if(boolWav == "true") {
                wavFile.remove(src + "/" + refWav);
            }
            QMessageBox *confirm = new QMessageBox();
            QString confirmTitle = tr("Music Replacement");
            confirm->setWindowTitle(confirmTitle);
            confirm->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
            confirm->setText("Success !");
            confirm->open();
            QDir::setCurrent(qApp->applicationDirPath());
        }
    }
}

void MainWindow::Help(QString folder)
{

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
    if(player->state() == QMediaPlayer::PlayingState) {
        player->stop();
    }
}

void MainWindow::on_pushButton_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(skyrimRealPath));
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
        skyrimMusicRealPath = skyrimRealPath + "/Data/music/";
    }
}

void MainWindow::ReplaceShout()
{
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    QString shoutName = buttonSender->text();
    QString shoutSubDir = buttonSender->accessibleName();
    QString realShout = buttonSender->accessibleDescription();
    QString val;
    QJsonValue fileValue;
    QFile file(qApp->applicationDirPath() + "/" + "shouts.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject sett2 = d.object();
    QJsonValue value = sett2.value(QString(realShout));
    if(shoutSubDir == "dragonborn") {
        fileValue = sett2.value(QString("Files_Dragonborn"));
    } else {
        fileValue = sett2.value(QString("Files"));
    }
    QJsonObject item = value.toObject();
    QJsonObject fileItem = fileValue.toObject();
    QMessageBox *test = new QMessageBox();
    QString title = tr("Shout Encoding");
    test->setIcon(QMessageBox::Warning);
    test->setWindowTitle(title);
    test->setTextFormat(Qt::RichText);
    test->setText(tr("You may now select replacements for the shout \"<b>%1</b>\"").arg(shoutName));
    QAbstractButton *firstWordv1Button = test->addButton(tr("First Word (v1)"), QMessageBox::ActionRole);
    QAbstractButton *firstWordv2Button = test->addButton(tr("First Word (v2)"), QMessageBox::ActionRole);
    QAbstractButton *secondWordButton = test->addButton(tr("Second Word"), QMessageBox::ActionRole);
    QAbstractButton *thirdWordButton = test->addButton(tr("Second + Third Word"), QMessageBox::ActionRole);
    QAbstractButton *replaceButton = test->addButton(tr("Replace"), QMessageBox::ActionRole);
    QAbstractButton *closeButton = test->addButton(tr("Close"), QMessageBox::ActionRole);
    firstWordv1Button->disconnect();
    firstWordv2Button->disconnect();
    secondWordButton->disconnect();
    thirdWordButton->disconnect();
    replaceButton->disconnect();
    connect(firstWordv1Button, &QAbstractButton::clicked, this, [=](){
        firstWordV1File = QFileDialog::getOpenFileName(this, "Open File", QString(), "Audio (*.wav *.mp3 *.wma)");
    });
    connect(firstWordv2Button, &QAbstractButton::clicked, this, [=](){
        firstWordV2File = QFileDialog::getOpenFileName(this, "Open File", QString(), "Audio (*.wav *.mp3 *.wma)");
    });
    connect(secondWordButton, &QAbstractButton::clicked, this, [=](){
        secondWordFile = QFileDialog::getOpenFileName(this, "Open File", QString(), "Audio (*.wav *.mp3 *.wma)");
    });
    connect(thirdWordButton, &QAbstractButton::clicked, this, [=](){
        secondAndThirdWordFile = QFileDialog::getOpenFileName(this, "Open File", QString(), "Audio (*.wav *.mp3 *.wma)");
    });
    connect(replaceButton, &QAbstractButton::clicked, this, [=](){
        if(firstWordV1File != "" && firstWordV2File != "" && secondWordFile != "" && secondAndThirdWordFile != "") {
            test->removeButton(firstWordv1Button);
            test->removeButton(firstWordv2Button);
            test->removeButton(secondWordButton);
            test->removeButton(thirdWordButton);
            test->removeButton(replaceButton);
            test->setStandardButtons(QMessageBox::Close);
            test->setIcon(QMessageBox::Information);
            test->setText("Please wait...");
            Delay(1);
            WavEncoder(firstWordV1File,item,fileItem,"0");
            WavEncoder(firstWordV2File,item,fileItem,"1");
            WavEncoder(secondWordFile,item,fileItem,"2");
            WavEncoder(secondAndThirdWordFile,item,fileItem,"3");
            FuzEncoder(item,fileItem,"0");
            FuzEncoder(item,fileItem,"1");
            FuzEncoder(item,fileItem,"2");
            FuzEncoder(item,fileItem,"3");
            QString folderName = gender+race;
            QDir directory(skyrimShoutsRealPath+shoutSubDir+".esm/"+folderName);
            if(!directory.exists())
            {
                directory.mkpath(".");
            }
            QString file1 = fileItem["0"].toString()+item["0"].toString()+fileEndWithExtension;
            QString file2 = fileItem["1"].toString()+item["1"].toString()+fileEndWithExtension;
            QString file3 = fileItem["2"].toString()+item["2"].toString()+fileEndWithExtension;
            QString file4 = fileItem["3"].toString()+item["3"].toString()+fileEndWithExtension;
            QString fileWav1 = fileItem["0"].toString()+item["0"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav2 = fileItem["1"].toString()+item["1"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav3 = fileItem["2"].toString()+item["2"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav4 = fileItem["3"].toString()+item["3"].toString()+fileEndWithoutExtension+".wav";
            QString fileXwm1 = fileItem["0"].toString()+item["0"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm2 = fileItem["1"].toString()+item["1"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm3 = fileItem["2"].toString()+item["2"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm4 = fileItem["3"].toString()+item["3"].toString()+fileEndWithoutExtension+".xwm";
            QString stringPath = skyrimShoutsRealPath+shoutSubDir+".esm/"+folderName+"/";
            QFile finalFile1(file1);
            QFile finalFile2(file2);
            QFile finalFile3(file3);
            QFile finalFile4(file4);
            QFile finalFileWav1(fileWav1);
            QFile finalFileWav2(fileWav2);
            QFile finalFileWav3(fileWav3);
            QFile finalFileWav4(fileWav4);
            QFile finalFileXwm1(fileXwm1);
            QFile finalFileXwm2(fileXwm2);
            QFile finalFileXwm3(fileXwm3);
            QFile finalFileXwm4(fileXwm4);
            finalFile1.copy(qApp->applicationDirPath() + QDir::separator() + file1, stringPath + file1);
            finalFile2.copy(qApp->applicationDirPath() + QDir::separator() + file2, stringPath + file2);
            finalFile3.copy(qApp->applicationDirPath() + QDir::separator() + file3, stringPath + file3);
            finalFile4.copy(qApp->applicationDirPath() + QDir::separator() + file4, stringPath + file4);
            finalFileWav1.remove(qApp->applicationDirPath() + QDir::separator() + fileWav1);
            finalFileWav2.remove(qApp->applicationDirPath() + QDir::separator() + fileWav2);
            finalFileWav3.remove(qApp->applicationDirPath() + QDir::separator() + fileWav3);
            finalFileWav4.remove(qApp->applicationDirPath() + QDir::separator() + fileWav4);
            finalFileXwm1.remove(qApp->applicationDirPath() + QDir::separator() + fileXwm1);
            finalFileXwm2.remove(qApp->applicationDirPath() + QDir::separator() + fileXwm2);
            finalFileXwm3.remove(qApp->applicationDirPath() + QDir::separator() + fileXwm3);
            finalFileXwm4.remove(qApp->applicationDirPath() + QDir::separator() + fileXwm4);
            finalFile1.remove(qApp->applicationDirPath() + QDir::separator() + file1);
            finalFile2.remove(qApp->applicationDirPath() + QDir::separator() + file2);
            finalFile3.remove(qApp->applicationDirPath() + QDir::separator() + file3);
            finalFile4.remove(qApp->applicationDirPath() + QDir::separator() + file4);
            test->setText(tr("Operation is a success !"));
        } else {
            test->setText(tr("You MUST select the 4 files before replacing your shout"));
        }
    });
    test->exec();
}

void MainWindow::FuzEncoder(QJsonObject item, QJsonObject fileItem, QString index)
{
    QProcess *fuzEncoder = new QProcess(this);
    QString fuzConverter = qApp->applicationDirPath()+"/fuz_extractor.exe";
    QStringList argumentsFuz;
    QString id = item[index].toString();
    QString fileDebut = fileItem[index].toString();
    QString reference = fileDebut + id + fileEndWithoutExtension+".xwm";
    QFile test(qApp->applicationDirPath()+"/"+reference);
//    Delay(1);
    argumentsFuz << "-c" << qApp->applicationDirPath() + "/" + fileDebut + id + fileEndWithExtension << qApp->applicationDirPath() + "/" + reference << qApp->applicationDirPath() + "/lipfile.lip";
    fuzEncoder->start(fuzConverter,argumentsFuz);
    fuzEncoder->waitForFinished();
    argumentsFuz.clear();
}

void MainWindow::WavEncoder(QString fileToEncode, QJsonObject item, QJsonObject fileItem, QString index)
{
    QString wavConverter = qApp->applicationDirPath()+"/ffmpeg.exe";
    QStringList argumentsWav;
    QProcess *wavEncoder = new QProcess(this);
    QString fileExtension = fileToEncode.right(3);
    qDebug() << fileToEncode;
    if(fileExtension == "wav") {
        XwmEncoder(fileToEncode,item,fileItem,index);
    } else {
        QString id = item[index].toString();
        QString fileDebut = fileItem[index].toString();
        QString reference = fileDebut + id + fileEndWithoutExtension+".xwm";
        QFileInfo file(fileToEncode);
        QDir::setCurrent(file.absoluteFilePath());
        QString referenceWav = reference;
        QString refWav = referenceWav.replace(".xwm",".wav");
        argumentsWav << "-i" << fileToEncode << qApp->applicationDirPath() +"/" + refWav;
        wavEncoder->start(wavConverter,argumentsWav);
        wavEncoder->waitForFinished();
        argumentsWav.clear();
        XwmEncoder(refWav,item,fileItem,index);
    }
}

void MainWindow::XwmEncoder(QString final, QJsonObject item, QJsonObject fileItem, QString index)
{
    QProcess *xwmEncoder = new QProcess(this);
    QString xwmConverter = qApp->applicationDirPath()+"/encode.exe";
    QStringList argumentsXwm;
    QString id = item[index].toString();
    QString fileDebut = fileItem[index].toString();
    QString reference = fileDebut + id + fileEndWithoutExtension+".xwm";
    argumentsXwm << final << qApp->applicationDirPath()+"/"+reference;
    qDebug() << argumentsXwm;
    xwmEncoder->start(xwmConverter,argumentsXwm);
    xwmEncoder->waitForFinished();
    argumentsXwm.clear();
}

void MainWindow::on_setRaceBtn_clicked()
{
    QAbstractButton *radioGenderBtn = ui->genderButtonGroup->checkedButton();
    QAbstractButton *radioRaceBtn = ui->raceButtonGroup->checkedButton();
    QMessageBox alert;
    if(radioGenderBtn == nullptr) {
        alert.setWindowTitle("ALERT");
        alert.setIcon(QMessageBox::Warning);
        alert.setText(tr("Please select a gender first."));
        alert.exec();
    } else {
        gender = radioGenderBtn->accessibleName();
        if(radioRaceBtn != nullptr) {
            race = radioRaceBtn->accessibleName();
            ui->shoutGroupBox->setEnabled(true);
        } else {
            alert.setWindowTitle("ALERT");
            alert.setIcon(QMessageBox::Warning);
            alert.setText(tr("Please select a race first."));
            alert.exec();
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->shoutGroupBox->setDisabled(true);
    gender = "";
    race = "";
    QButtonGroup *genders = ui->genderButtonGroup;
    QButtonGroup *races = ui->raceButtonGroup;
    QAbstractButton* checkedGenders = genders->checkedButton();
    QAbstractButton* checkedRaces = races->checkedButton();
    if (checkedGenders)
    {
        genders->setExclusive(false);
        checkedGenders->setChecked(false);
        genders->setExclusive(true);
    }
    if (checkedRaces)
    {
        races->setExclusive(false);
        checkedRaces->setChecked(false);
        races->setExclusive(true);
    }
}

void MainWindow::on_RestoreShoutsBtn_clicked()
{
    QString folderName = gender+race;
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    QString typeShout = buttonSender->accessibleName();
    QDir directory(skyrimShoutsRealPath+typeShout + ".esm/"+folderName);
    QMessageBox *restoreShoutsBox = new QMessageBox();
    restoreShoutsBox->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
    restoreShoutsBox->setWindowTitle("ATTENTION");
    restoreShoutsBox->setIcon(QMessageBox::Warning);
    restoreShoutsBox->setText(tr("Are you sure to restore the original shouts for selected gender and race ?"));
    restoreShoutsBox->setStandardButtons(QMessageBox::Yes);
    restoreShoutsBox->addButton(QMessageBox::No);
    restoreShoutsBox->setDefaultButton(QMessageBox::No);
    if(restoreShoutsBox->exec() == QMessageBox::Yes) {
        if(directory.exists()) {
            directory.removeRecursively();
        }
    }
}

void MainWindow::Delay(int seconds)
{
    QTime dieTime= QTime::currentTime().addSecs(seconds);
    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void MainWindow::RestoreShouts()
{
    QString folderName = gender+race;
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    QString typeShout = buttonSender->accessibleName();
    QString shoutsFolderString = skyrimShoutsRealPath+typeShout + ".esm/"+folderName;
    QDir directory(shoutsFolderString);
    QMessageBox *restoreShoutsBox = new QMessageBox();
    restoreShoutsBox->setWindowIcon(QIcon(":/images/images/ico/mainIcon.ico"));
    restoreShoutsBox->setWindowTitle("ATTENTION");
    restoreShoutsBox->setIcon(QMessageBox::Warning);
    restoreShoutsBox->setText(tr("Are you sure to restore the original shouts for selected gender and race ?"));
    restoreShoutsBox->setStandardButtons(QMessageBox::Yes);
    restoreShoutsBox->addButton(QMessageBox::No);
    restoreShoutsBox->setDefaultButton(QMessageBox::No);
    if(restoreShoutsBox->exec() == QMessageBox::Yes) {
        if(directory.exists()) {
            directory.removeRecursively();
        }
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
    if(!directory.exists())
    {
        directory.mkpath(".");
    }
    if(songName == "Ragnar the Red") {
        QString sixthFile = QFileDialog::getOpenFileName(this, tr("File 6"), QString(), "Audio (*.wav *.mp3 *.wma)");
        if(firstFile == "" || secondFile == "" || thirdFile == "" || fourthFile == "" || fifthFile == "" || sixthFile == "") {
            bardBox->setText(tr("All files must be valid in order to continue !"));
            bardBox->exec();
        } else {
            bardBox->setText(tr("You will now wait for a few seconds. Press Ok to proceed"));
            bardBox->exec();
            WavBardsEncoder(firstFile,item,"0");
            WavBardsEncoder(secondFile,item,"1");
            WavBardsEncoder(thirdFile,item,"2");
            WavBardsEncoder(fourthFile,item,"3");
            WavBardsEncoder(fifthFile,item,"4");
            WavBardsEncoder(sixthFile,item,"5");
            FuzBardsEncoder(item,"0");
            FuzBardsEncoder(item,"1");
            FuzBardsEncoder(item,"2");
            FuzBardsEncoder(item,"3");
            FuzBardsEncoder(item,"4");
            FuzBardsEncoder(item,"5");
            QString file1 = bardDebutFile+item["0"].toString()+fileEndWithExtension;
            QString file2 = bardDebutFile+item["1"].toString()+fileEndWithExtension;
            QString file3 = bardDebutFile+item["2"].toString()+fileEndWithExtension;
            QString file4 = bardDebutFile+item["3"].toString()+fileEndWithExtension;
            QString file5 = bardDebutFile+item["4"].toString()+fileEndWithExtension;
            QString file6 = bardDebutFile+item["5"].toString()+fileEndWithExtension;
            QString fileWav1 = bardDebutFile+item["0"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav2 = bardDebutFile+item["1"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav3 = bardDebutFile+item["2"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav4 = bardDebutFile+item["3"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav5 = bardDebutFile+item["4"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav6 = bardDebutFile+item["5"].toString()+fileEndWithoutExtension+".wav";
            QString fileXwm1 = bardDebutFile+item["0"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm2 = bardDebutFile+item["1"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm3 = bardDebutFile+item["2"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm4 = bardDebutFile+item["3"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm5 = bardDebutFile+item["4"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm6 = bardDebutFile+item["5"].toString()+fileEndWithoutExtension+".xwm";
            QString stringPath = skyrimShoutsRealPath+"skyrim.esm/"+folderName+"/";
            QFile finalFile1(file1);
            QFile finalFile2(file2);
            QFile finalFile3(file3);
            QFile finalFile4(file4);
            QFile finalFile5(file5);
            QFile finalFile6(file6);
            QFile finalFileWav1(fileWav1);
            QFile finalFileWav2(fileWav2);
            QFile finalFileWav3(fileWav3);
            QFile finalFileWav4(fileWav4);
            QFile finalFileWav5(fileWav5);
            QFile finalFileWav6(fileWav6);
            QFile finalFileXwm1(fileXwm1);
            QFile finalFileXwm2(fileXwm2);
            QFile finalFileXwm3(fileXwm3);
            QFile finalFileXwm4(fileXwm4);
            QFile finalFileXwm5(fileXwm5);
            QFile finalFileXwm6(fileXwm6);
            finalFile1.copy(qApp->applicationDirPath() + "/" + file1, stringPath + file1);
            finalFile2.copy(qApp->applicationDirPath() + "/" + file2, stringPath + file2);
            finalFile3.copy(qApp->applicationDirPath() + "/" + file3, stringPath + file3);
            finalFile4.copy(qApp->applicationDirPath() + "/" + file4, stringPath + file4);
            finalFile5.copy(qApp->applicationDirPath() + "/" + file5, stringPath + file5);
            finalFile6.copy(qApp->applicationDirPath() + "/" + file6, stringPath + file6);
            finalFileWav1.remove(qApp->applicationDirPath() + "/" + fileWav1);
            finalFileWav2.remove(qApp->applicationDirPath() + "/" + fileWav2);
            finalFileWav3.remove(qApp->applicationDirPath() + "/" + fileWav3);
            finalFileWav4.remove(qApp->applicationDirPath() + "/" + fileWav4);
            finalFileWav5.remove(qApp->applicationDirPath() + "/" + fileWav5);
            finalFileWav6.remove(qApp->applicationDirPath() + "/" + fileWav6);
            finalFileXwm1.remove(qApp->applicationDirPath() + "/" + fileXwm1);
            finalFileXwm2.remove(qApp->applicationDirPath() + "/" + fileXwm2);
            finalFileXwm3.remove(qApp->applicationDirPath() + "/" + fileXwm3);
            finalFileXwm4.remove(qApp->applicationDirPath() + "/" + fileXwm4);
            finalFileXwm5.remove(qApp->applicationDirPath() + "/" + fileXwm5);
            finalFileXwm6.remove(qApp->applicationDirPath() + "/" + fileXwm6);
            finalFile1.remove(qApp->applicationDirPath() + "/" + file1);
            finalFile2.remove(qApp->applicationDirPath() + "/" + file2);
            finalFile3.remove(qApp->applicationDirPath() + "/" + file3);
            finalFile4.remove(qApp->applicationDirPath() + "/" + file4);
            finalFile5.remove(qApp->applicationDirPath() + "/" + file5);
            finalFile6.remove(qApp->applicationDirPath() + "/" + file6);
            bardBox->setText(tr("Bard song replaced !"));
            bardBox->exec();
        }
    } else if(songName == "Tale of the Tongues" || songName == "The Dragonborn Comes") {
        QString sixthFile = QFileDialog::getOpenFileName(this, tr("File 6"), QString(), "Audio (*.wav *.mp3 *.wma)");
        QString seventhFile = QFileDialog::getOpenFileName(this, tr("File 7"), QString(), "Audio (*.wav *.mp3 *.wma)");
        QString eigthFile = QFileDialog::getOpenFileName(this, tr("File 8"), QString(), "Audio (*.wav *.mp3 *.wma)");
        if(firstFile == "" || secondFile == "" || thirdFile == "" || fourthFile == "" || fifthFile == "" || sixthFile == "" || seventhFile == "" || eigthFile == "") {
            bardBox->setText(tr("All files must be valid in order to continue !"));
            bardBox->exec();
        } else {
            bardBox->setText(tr("You will now wait for a few seconds. Press Ok to proceed"));
            bardBox->exec();
            WavBardsEncoder(firstFile,item,"0");
            WavBardsEncoder(secondFile,item,"1");
            WavBardsEncoder(thirdFile,item,"2");
            WavBardsEncoder(fourthFile,item,"3");
            WavBardsEncoder(fifthFile,item,"4");
            WavBardsEncoder(sixthFile,item,"5");
            WavBardsEncoder(seventhFile,item,"6");
            WavBardsEncoder(eigthFile,item,"7");
            FuzBardsEncoder(item,"0");
            FuzBardsEncoder(item,"1");
            FuzBardsEncoder(item,"2");
            FuzBardsEncoder(item,"3");
            FuzBardsEncoder(item,"4");
            FuzBardsEncoder(item,"5");
            FuzBardsEncoder(item,"6");
            FuzBardsEncoder(item,"7");
            QString file1 = bardDebutFile+item["0"].toString()+fileEndWithExtension;
            QString file2 = bardDebutFile+item["1"].toString()+fileEndWithExtension;
            QString file3 = bardDebutFile+item["2"].toString()+fileEndWithExtension;
            QString file4 = bardDebutFile+item["3"].toString()+fileEndWithExtension;
            QString file5 = bardDebutFile+item["4"].toString()+fileEndWithExtension;
            QString file6 = bardDebutFile+item["5"].toString()+fileEndWithExtension;
            QString file7 = bardDebutFile+item["6"].toString()+fileEndWithExtension;
            QString file8 = bardDebutFile+item["7"].toString()+fileEndWithExtension;
            QString fileWav1 = bardDebutFile+item["0"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav2 = bardDebutFile+item["1"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav3 = bardDebutFile+item["2"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav4 = bardDebutFile+item["3"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav5 = bardDebutFile+item["4"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav6 = bardDebutFile+item["5"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav7 = bardDebutFile+item["6"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav8 = bardDebutFile+item["7"].toString()+fileEndWithoutExtension+".wav";
            QString fileXwm1 = bardDebutFile+item["0"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm2 = bardDebutFile+item["1"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm3 = bardDebutFile+item["2"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm4 = bardDebutFile+item["3"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm5 = bardDebutFile+item["4"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm6 = bardDebutFile+item["5"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm7 = bardDebutFile+item["6"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm8 = bardDebutFile+item["7"].toString()+fileEndWithoutExtension+".xwm";
            QString stringPath = skyrimShoutsRealPath+"skyrim.esm/"+folderName+"/";
            QFile finalFile1(file1);
            QFile finalFile2(file2);
            QFile finalFile3(file3);
            QFile finalFile4(file4);
            QFile finalFile5(file5);
            QFile finalFile6(file6);
            QFile finalFile7(file7);
            QFile finalFile8(file8);
            QFile finalFileWav1(fileWav1);
            QFile finalFileWav2(fileWav2);
            QFile finalFileWav3(fileWav3);
            QFile finalFileWav4(fileWav4);
            QFile finalFileWav5(fileWav5);
            QFile finalFileWav6(fileWav6);
            QFile finalFileWav7(fileWav7);
            QFile finalFileWav8(fileWav8);
            QFile finalFileXwm1(fileXwm1);
            QFile finalFileXwm2(fileXwm2);
            QFile finalFileXwm3(fileXwm3);
            QFile finalFileXwm4(fileXwm4);
            QFile finalFileXwm5(fileXwm5);
            QFile finalFileXwm6(fileXwm6);
            QFile finalFileXwm7(fileXwm7);
            QFile finalFileXwm8(fileXwm8);
            finalFile1.copy(qApp->applicationDirPath() + "/" + file1, stringPath + file1);
            finalFile2.copy(qApp->applicationDirPath() + "/" + file2, stringPath + file2);
            finalFile3.copy(qApp->applicationDirPath() + "/" + file3, stringPath + file3);
            finalFile4.copy(qApp->applicationDirPath() + "/" + file4, stringPath + file4);
            finalFile5.copy(qApp->applicationDirPath() + "/" + file5, stringPath + file5);
            finalFile6.copy(qApp->applicationDirPath() + "/" + file6, stringPath + file6);
            finalFile7.copy(qApp->applicationDirPath() + "/" + file7, stringPath + file7);
            finalFile8.copy(qApp->applicationDirPath() + "/" + file8, stringPath + file8);
            finalFileWav1.remove(qApp->applicationDirPath() + "/" + fileWav1);
            finalFileWav2.remove(qApp->applicationDirPath() + "/" + fileWav2);
            finalFileWav3.remove(qApp->applicationDirPath() + "/" + fileWav3);
            finalFileWav4.remove(qApp->applicationDirPath() + "/" + fileWav4);
            finalFileWav5.remove(qApp->applicationDirPath() + "/" + fileWav5);
            finalFileWav6.remove(qApp->applicationDirPath() + "/" + fileWav6);
            finalFileWav7.remove(qApp->applicationDirPath() + "/" + fileWav7);
            finalFileWav8.remove(qApp->applicationDirPath() + "/" + fileWav8);
            finalFileXwm1.remove(qApp->applicationDirPath() + "/" + fileXwm1);
            finalFileXwm2.remove(qApp->applicationDirPath() + "/" + fileXwm2);
            finalFileXwm3.remove(qApp->applicationDirPath() + "/" + fileXwm3);
            finalFileXwm4.remove(qApp->applicationDirPath() + "/" + fileXwm4);
            finalFileXwm5.remove(qApp->applicationDirPath() + "/" + fileXwm5);
            finalFileXwm6.remove(qApp->applicationDirPath() + "/" + fileXwm6);
            finalFileXwm7.remove(qApp->applicationDirPath() + "/" + fileXwm7);
            finalFileXwm8.remove(qApp->applicationDirPath() + "/" + fileXwm8);
            finalFile1.remove(qApp->applicationDirPath() + "/" + file1);
            finalFile2.remove(qApp->applicationDirPath() + "/" + file2);
            finalFile3.remove(qApp->applicationDirPath() + "/" + file3);
            finalFile4.remove(qApp->applicationDirPath() + "/" + file4);
            finalFile5.remove(qApp->applicationDirPath() + "/" + file5);
            finalFile6.remove(qApp->applicationDirPath() + "/" + file6);
            finalFile7.remove(qApp->applicationDirPath() + "/" + file7);
            finalFile8.remove(qApp->applicationDirPath() + "/" + file8);
            bardBox->setText(tr("Bard song replaced !"));
            bardBox->exec();
        }
    } else {
        if(firstFile == "" || secondFile == "" || thirdFile == "" || fourthFile == "" || fifthFile == "") {
            bardBox->setText(tr("All files must be valid in order to continue !"));
            bardBox->exec();
        } else {
            bardBox->setText(tr("You will now wait for a few seconds. Press Ok to proceed"));
            bardBox->exec();
            WavBardsEncoder(firstFile,item,"0");
            WavBardsEncoder(secondFile,item,"1");
            WavBardsEncoder(thirdFile,item,"2");
            WavBardsEncoder(fourthFile,item,"3");
            WavBardsEncoder(fifthFile,item,"4");
            FuzBardsEncoder(item,"0");
            FuzBardsEncoder(item,"1");
            FuzBardsEncoder(item,"2");
            FuzBardsEncoder(item,"3");
            FuzBardsEncoder(item,"4");

            QString file1 = bardDebutFile+item["0"].toString()+fileEndWithExtension;
            QString file2 = bardDebutFile+item["1"].toString()+fileEndWithExtension;
            QString file3 = bardDebutFile+item["2"].toString()+fileEndWithExtension;
            QString file4 = bardDebutFile+item["3"].toString()+fileEndWithExtension;
            QString file5 = bardDebutFile+item["4"].toString()+fileEndWithExtension;
            QString fileWav1 = bardDebutFile+item["0"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav2 = bardDebutFile+item["1"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav3 = bardDebutFile+item["2"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav4 = bardDebutFile+item["3"].toString()+fileEndWithoutExtension+".wav";
            QString fileWav5 = bardDebutFile+item["4"].toString()+fileEndWithoutExtension+".wav";
            QString fileXwm1 = bardDebutFile+item["0"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm2 = bardDebutFile+item["1"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm3 = bardDebutFile+item["2"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm4 = bardDebutFile+item["3"].toString()+fileEndWithoutExtension+".xwm";
            QString fileXwm5 = bardDebutFile+item["4"].toString()+fileEndWithoutExtension+".xwm";
            QString stringPath = skyrimShoutsRealPath+"skyrim.esm/"+folderName+"/";
            QFile finalFile1(file1);
            QFile finalFile2(file2);
            QFile finalFile3(file3);
            QFile finalFile4(file4);
            QFile finalFile5(file5);
            QFile finalFileWav1(fileWav1);
            QFile finalFileWav2(fileWav2);
            QFile finalFileWav3(fileWav3);
            QFile finalFileWav4(fileWav4);
            QFile finalFileWav5(fileWav5);
            QFile finalFileXwm1(fileXwm1);
            QFile finalFileXwm2(fileXwm2);
            QFile finalFileXwm3(fileXwm3);
            QFile finalFileXwm4(fileXwm4);
            QFile finalFileXwm5(fileXwm5);
            finalFile1.copy(qApp->applicationDirPath() + "/" + file1, stringPath + file1);
            finalFile2.copy(qApp->applicationDirPath() + "/" + file2, stringPath + file2);
            finalFile3.copy(qApp->applicationDirPath() + "/" + file3, stringPath + file3);
            finalFile4.copy(qApp->applicationDirPath() + "/" + file4, stringPath + file4);
            finalFile5.copy(qApp->applicationDirPath() + "/" + file5, stringPath + file5);
            finalFileWav1.remove(qApp->applicationDirPath() + "/" + fileWav1);
            finalFileWav2.remove(qApp->applicationDirPath() + "/" + fileWav2);
            finalFileWav3.remove(qApp->applicationDirPath() + "/" + fileWav3);
            finalFileWav4.remove(qApp->applicationDirPath() + "/" + fileWav4);
            finalFileWav5.remove(qApp->applicationDirPath() + "/" + fileWav5);
            finalFileXwm1.remove(qApp->applicationDirPath() + "/" + fileXwm1);
            finalFileXwm2.remove(qApp->applicationDirPath() + "/" + fileXwm2);
            finalFileXwm3.remove(qApp->applicationDirPath() + "/" + fileXwm3);
            finalFileXwm4.remove(qApp->applicationDirPath() + "/" + fileXwm4);
            finalFileXwm5.remove(qApp->applicationDirPath() + "/" + fileXwm5);
            finalFile1.remove(qApp->applicationDirPath() + "/" + file1);
            finalFile2.remove(qApp->applicationDirPath() + "/" + file2);
            finalFile3.remove(qApp->applicationDirPath() + "/" + file3);
            finalFile4.remove(qApp->applicationDirPath() + "/" + file4);
            finalFile5.remove(qApp->applicationDirPath() + "/" + file5);
            bardBox->setText(tr("Bard song replaced !"));
            bardBox->exec();
        }
    }
}

void MainWindow::WavBardsEncoder(QString fileToEncode, QJsonObject item, QString index)
{
    QString wavConverter = qApp->applicationDirPath()+"/ffmpeg.exe";
    QStringList argumentsWav;
    QProcess *wavEncoder = new QProcess(this);
    QString fileExtension = fileToEncode.right(3);
    if(fileExtension == "wav") {
        XwmBardsEncoder(fileToEncode,item,index);
    } else {
        QString id = item[index].toString();
        QString reference = bardDebutFile + id + fileEndWithoutExtension+".xwm";
        QFileInfo file(fileToEncode);
        QDir::setCurrent(file.absoluteFilePath());
        QString referenceWav = reference;
        QString refWav = referenceWav.replace(".xwm",".wav");
        argumentsWav << "-i" << fileToEncode << qApp->applicationDirPath() +"/" + refWav;
        wavEncoder->start(wavConverter,argumentsWav);
        wavEncoder->waitForFinished();
        argumentsWav.clear();
        XwmBardsEncoder(refWav,item,index);
    }
}

void MainWindow::XwmBardsEncoder(QString final, QJsonObject item, QString index)
{
    QProcess *xwmEncoder = new QProcess(this);
    QString xwmConverter = qApp->applicationDirPath()+"/encode.exe";
    QStringList argumentsXwm;
    QString id = item[index].toString();
    QString reference = bardDebutFile + id + fileEndWithoutExtension+".xwm";
    argumentsXwm << qApp->applicationDirPath()+"/"+final << qApp->applicationDirPath()+"/"+reference;
    xwmEncoder->start(xwmConverter,argumentsXwm);
    xwmEncoder->waitForFinished();
    argumentsXwm.clear();
}

void MainWindow::FuzBardsEncoder(QJsonObject item, QString index)
{
    QProcess *fuzEncoder = new QProcess(this);
    QString fuzConverter = qApp->applicationDirPath()+"/fuz_extractor.exe";
    QStringList argumentsFuz;
    QString id = item[index].toString();
    QString reference = bardDebutFile + id + fileEndWithoutExtension+".xwm";
    argumentsFuz << "-c" << qApp->applicationDirPath() + "/" + bardDebutFile + id + fileEndWithExtension << qApp->applicationDirPath() + "/" + reference << qApp->applicationDirPath() + "/lipfile.lip";
    fuzEncoder->start(fuzConverter,argumentsFuz);
    fuzEncoder->waitForFinished();
    argumentsFuz.clear();
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
    ui->retranslateUi(this);
    this->setWindowTitle(version);
}

void MainWindow::on_actionRussian_triggered()
{
    language = "ru";
    translator.load(":/translation_ru.qm");
    ui->retranslateUi(this);
    this->setWindowTitle(version);
}

void MainWindow::on_actionEnglish_triggered()
{
    language = "en";
    translator.load(":/translation_en.qm");
    ui->retranslateUi(this);
    this->setWindowTitle(version);
}
