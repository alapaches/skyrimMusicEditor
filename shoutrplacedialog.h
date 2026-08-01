#ifndef SHOUTREPLCEDIALOG_H
#define SHOUTREPLCEDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QString>
#include <QList>
#include <QEvent>

class ShoutReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    struct WordSlot {
        QString label;
        QString filePath;
    };

    explicit ShoutReplaceDialog(const QString &shoutName,
                                const QString &voiceInfo,
                                int wordCount = 4,
                                QWidget *parent = nullptr)
        : QDialog(parent)
        , shoutName(shoutName)
        , wordCount(wordCount)
    {
        setObjectName("shoutReplaceDialog");
        setWindowTitle(tr("Replace shout"));
        setMinimumWidth(480);
        setMaximumWidth(520);

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // --- Header ---
        auto *headerWidget = new QWidget(this);
        headerWidget->setObjectName("srdHeader");
        auto *headerLayout = new QVBoxLayout(headerWidget);
        headerLayout->setContentsMargins(24, 20, 24, 0);
        headerLayout->setSpacing(4);

        auto *titleLabel = new QLabel(tr("Replace shout"), headerWidget);
        titleLabel->setObjectName("srdTitle");
        headerLayout->addWidget(titleLabel);

        auto *subtitleLabel = new QLabel(shoutName + " · " + voiceInfo, headerWidget);
        subtitleLabel->setObjectName("srdSubtitle");
        headerLayout->addWidget(subtitleLabel);

        mainLayout->addWidget(headerWidget);

        // --- wordSlots area ---
        auto *slotsWidget = new QWidget(this);
        auto *slotsOuterLayout = new QVBoxLayout(slotsWidget);
        slotsOuterLayout->setContentsMargins(24, 16, 24, 16);
        slotsOuterLayout->setSpacing(12);

        progressLabel = new QLabel(this);
        progressLabel->setObjectName("srdProgress");
        slotsOuterLayout->addWidget(progressLabel);

        slotsLayout = new QVBoxLayout();
        slotsLayout->setSpacing(8);

        QStringList labels;
        if (wordCount == 4) {
            labels = {
                tr("First word (v1)"),
                tr("First word (v2)"),
                tr("Second word"),
                tr("Second + third word")
            };
        } else {
            for (int i = 0; i < wordCount; ++i) {
                labels << tr("File %1").arg(i + 1);
            }
        }

        for (int i = 0; i < wordCount; ++i) {
            WordSlot slot;
            slot.label = labels[i];
            wordSlots.append(slot);
            slotsLayout->addWidget(CreateSlotWidget(i));
        }

        slotsOuterLayout->addLayout(slotsLayout);
        mainLayout->addWidget(slotsWidget);

        // --- Footer ---
        auto *footerWidget = new QWidget(this);
        footerWidget->setObjectName("srdFooter");
        auto *footerLayout = new QHBoxLayout(footerWidget);
        footerLayout->setContentsMargins(24, 0, 24, 20);
        footerLayout->addStretch();

        cancelBtn = new QPushButton(tr("Cancel"), footerWidget);
        cancelBtn->setObjectName("srdCancelBtn");
        footerLayout->addWidget(cancelBtn);

        replaceBtn = new QPushButton(tr("Replace"), footerWidget);
        replaceBtn->setObjectName("srdReplaceBtn");
        replaceBtn->setEnabled(false);
        footerLayout->addWidget(replaceBtn);

        mainLayout->addWidget(footerWidget);

        connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
        connect(replaceBtn, &QPushButton::clicked, this, &QDialog::accept);

        UpdateProgress();
    }

    QStringList selectedFiles() const
    {
        QStringList result;
        for (const WordSlot &s : wordSlots) {
            result << s.filePath;
        }
        return result;
    }

    bool allSelected() const
    {
        for (const WordSlot &s : wordSlots) {
            if (s.filePath.isEmpty()) return false;
        }
        return true;
    }

private:
    QString shoutName;
    int wordCount;
    QList<WordSlot> wordSlots;
    QVBoxLayout *slotsLayout = nullptr;
    QLabel *progressLabel = nullptr;
    QPushButton *replaceBtn = nullptr;
    QPushButton *cancelBtn = nullptr;

    QWidget* CreateSlotWidget(int index)
    {
        auto *widget = new QWidget(this);
        widget->setObjectName(wordSlots[index].filePath.isEmpty() ? "srdSlotEmpty" : "srdSlotFilled");
        widget->setCursor(Qt::PointingHandCursor);
        widget->setFixedHeight(52);

        auto *layout = new QHBoxLayout(widget);
        layout->setContentsMargins(14, 10, 14, 10);
        layout->setSpacing(12);

        auto *iconLabel = new QLabel(widget);
        iconLabel->setObjectName("srdSlotIcon");
        iconLabel->setFixedSize(24, 24);
        iconLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(iconLabel);

        auto *textBlock = new QVBoxLayout();
        textBlock->setSpacing(1);

        auto *nameLabel = new QLabel(wordSlots[index].label, widget);
        nameLabel->setObjectName("srdSlotName");
        textBlock->addWidget(nameLabel);

        auto *fileLabel = new QLabel(tr("Click to select file"), widget);
        fileLabel->setObjectName("srdSlotFile");
        textBlock->addWidget(fileLabel);

        layout->addLayout(textBlock, 1);

        auto *actionLabel = new QLabel(widget);
        actionLabel->setObjectName("srdSlotAction");
        layout->addWidget(actionLabel);

        UpdateSlotAppearance(widget, index);

        // Click handler on the whole widget
        widget->installEventFilter(this);
        widget->setProperty("slotIndex", index);

        return widget;
    }

    void UpdateSlotAppearance(QWidget *widget, int index)
    {
        bool filled = !wordSlots[index].filePath.isEmpty();

        auto *iconLabel = widget->findChild<QLabel*>("srdSlotIcon");
        auto *nameLabel = widget->findChild<QLabel*>("srdSlotName");
        auto *fileLabel = widget->findChild<QLabel*>("srdSlotFile");
        auto *actionLabel = widget->findChild<QLabel*>("srdSlotAction");

        if (filled) {
            widget->setStyleSheet(
                "#srdSlotIcon { background-color: #2a3a2a; border-radius: 12px; color: #7bc98a; font-size: 14px; }"
                "QWidget { background-color: #26262b; border-radius: 8px; border: none; }"
                );
            iconLabel->setText("\u2713");
            nameLabel->setStyleSheet("color: #f0f0f2; font-size: 13px; background: transparent;");
            nameLabel->setText(wordSlots[index].label);

            QFileInfo fi(wordSlots[index].filePath);
            fileLabel->setStyleSheet("color: #6e6e78; font-size: 11px; background: transparent;");
            fileLabel->setText(fi.fileName());

            actionLabel->setStyleSheet("color: #6e6e78; font-size: 12px; background: transparent;");
            actionLabel->setText(tr("change"));
        } else {
            widget->setStyleSheet(
                "#srdSlotIcon { background-color: #26262b; border-radius: 12px; color: #6e6e78; font-size: 14px; }"
                "QWidget { background-color: #1d1d21; border: 1px dashed #32323a; border-radius: 8px; }"
                );
            iconLabel->setText("\u2191");
            nameLabel->setStyleSheet("color: #a1a1aa; font-size: 13px; background: transparent;");
            nameLabel->setText(wordSlots[index].label);

            fileLabel->setStyleSheet("color: #6e6e78; font-size: 11px; background: transparent;");
            fileLabel->setText(tr("Click to select file"));

            actionLabel->setStyleSheet("background: transparent;");
            actionLabel->setText("");
        }
    }

    void UpdateProgress()
    {
        int filled = 0;
        for (const WordSlot &s : wordSlots) {
            if (!s.filePath.isEmpty()) ++filled;
        }
        progressLabel->setText(tr("Select a file for each word (%1 of %2)").arg(filled).arg(wordCount));
        replaceBtn->setEnabled(filled == wordCount);

        if (filled == wordCount) {
            replaceBtn->setStyleSheet(
                "background-color: #d4a94a; color: #17171a; border: none; "
                "border-radius: 6px; padding: 8px 16px; font-weight: bold;"
                );
        } else {
            replaceBtn->setStyleSheet(
                "background-color: #32323a; color: #4a4a52; border: none; "
                "border-radius: 6px; padding: 8px 16px;"
                );
        }
    }

    void SelectFileFor(int index)
    {
        QString file = QFileDialog::getOpenFileName(
            this,
            wordSlots[index].label,
            QString(),
            tr("Audio (*.wav *.mp3 *.wma)")
            );
        if (file.isEmpty()) return;

        wordSlots[index].filePath = file;

        QWidget *slotWidget = static_cast<QWidget*>(slotsLayout->itemAt(index)->widget());
        UpdateSlotAppearance(slotWidget, index);
        UpdateProgress();
    }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        if (event->type() == QEvent::MouseButtonRelease) {
            QWidget *widget = qobject_cast<QWidget*>(obj);
            if (widget) {
                QVariant v = widget->property("slotIndex");
                if (v.isValid()) {
                    SelectFileFor(v.toInt());
                    return true;
                }
            }
        }
        return QDialog::eventFilter(obj, event);
    }
};

#endif // SHOUTREPLCEDIALOG_H