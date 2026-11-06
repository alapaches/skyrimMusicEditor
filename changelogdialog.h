#ifndef CHANGELOGDIALOG_H
#define CHANGELOGDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QCheckBox>
#include <QSettings>
#include <QFile>
#include <QCoreApplication>

class ChangelogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangelogDialog(const QString &appVersion, const QString &language = "en", QWidget *parent = nullptr)
        : QDialog(parent)
        , appVersion(appVersion)
        , lang(language)
    {
        setWindowTitle(lang == "fr" ? "Nouveautés" : lang == "ru" ? "Что нового" : "What's new");
        setObjectName("changelogDialog");
        setMinimumSize(560, 420);
        resize(620, 480);

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(24, 20, 24, 16);
        mainLayout->setSpacing(12);

        auto *titleLabel = new QLabel(this);
        titleLabel->setObjectName("changelogTitle");
        titleLabel->setText("Skyrim Music Editor — " + appVersion);
        mainLayout->addWidget(titleLabel);

        auto *subtitleLabel = new QLabel(this);
        subtitleLabel->setObjectName("changelogSubtitle");
        subtitleLabel->setText(lang == "fr" ? "Voici les changements depuis la dernière version :"
                               : lang == "ru" ? "Что изменилось с прошлой версии:"
                                              : "Here's what changed since the last version:");
        mainLayout->addWidget(subtitleLabel);

        auto *content = new QTextBrowser(this);
        content->setObjectName("changelogContent");
        content->setOpenExternalLinks(true);
        content->setFrameShape(QFrame::NoFrame);

        QString changelogText;
        QFile file(qApp->applicationDirPath() + "/changelog_" + lang + ".html");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            file.setFileName(qApp->applicationDirPath() + "/changelog_en.html");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
        }
        if (file.isOpen()) {
            changelogText = QString::fromUtf8(file.readAll());
            file.close();
        } else {
            changelogText = defaultChangelog();
        }
        content->setHtml(changelogText);
        mainLayout->addWidget(content, 1);

        auto *footerLayout = new QHBoxLayout();
        footerLayout->setContentsMargins(0, 4, 0, 0);

        dontShowCheck = new QCheckBox(this);
        dontShowCheck->setObjectName("changelogCheck");
        dontShowCheck->setText(lang == "fr" ? "Ne plus afficher pour cette version"
                               : lang == "ru" ? "Не показывать для этой версии"
                                              : "Don't show again for this version");
        footerLayout->addWidget(dontShowCheck);

        footerLayout->addStretch();

        auto *closeBtn = new QPushButton(this);
        closeBtn->setObjectName("changelogCloseBtn");
        closeBtn->setMinimumWidth(100);
        closeBtn->setText(lang == "fr" ? "Compris !" : lang == "ru" ? "Понятно!" : "Got it");
        footerLayout->addWidget(closeBtn);

        mainLayout->addLayout(footerLayout);

        connect(closeBtn, &QPushButton::clicked, this, [this]() {
            if (dontShowCheck->isChecked()) {
                QSettings settings("SkyrimMusicEditor", "SkyrimMusicEditor");
                settings.setValue("changelog/lastSeenVersion", this->appVersion);
            }
            accept();
        });
    }

    // Renvoie true si le changelog doit etre affiche
    static bool shouldShow(const QString &currentVersion)
    {
        QSettings settings("SkyrimMusicEditor", "SkyrimMusicEditor");
        QString lastSeen = settings.value("changelog/lastSeenVersion", "").toString();
        return (lastSeen != currentVersion);
    }

private:
    QString appVersion;
    QString lang;
    QCheckBox *dontShowCheck = nullptr;

    QString defaultChangelog() const
    {
        return R"(
            <h3 style="color: #d4a94a;">Version 3.2</h3>
            <ul>
                <li>Encoding errors are now reported instead of a false "Success"</li>
                <li>Replacing an already replaced file now works</li>
                <li>"Preview" in the replacement dialog now plays the new file</li>
                <li>Restoring Dragonborn music no longer deletes files before confirmation</li>
                <li>Restore only removes files managed by the application</li>
                <li>Game folder is validated; actions ask for it when missing</li>
                <li>"Launch Skyrim" button now works (SKSE supported)</li>
                <li>Temporary files are written to the user temp folder (works when installed in Program Files)</li>
                <li>Lyrics open in the selected language at startup</li>
            </ul>
            <h3 style="color: #d4a94a;">Version 3.0</h3>
            <ul>
                <li>Completely redesigned interface with modern dark theme</li>
                <li>Sidebar navigation replaces tabs</li>
                <li>Shouts screen: list view with status detection</li>
                <li>Bards screen: list view with lyrics integration</li>
                <li>Asynchronous audio encoding (UI no longer freezes)</li>
                <li>Music and shout replacement status detection</li>
                <li>Path verification dialog for Skyrim installations</li>
                <li>Reduced application size</li>
            </ul>
        )";
    }
};

#endif // CHANGELOGDIALOG_H