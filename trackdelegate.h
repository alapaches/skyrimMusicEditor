#ifndef TRACKDELEGATE_H
#define TRACKDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>

// Role portant le flag "cette ligne a ete remplacee"
static const int ReplacedRole = Qt::UserRole + 10;

class TrackDelegate : public QStyledItemDelegate
{
public:
    explicit TrackDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        // Le flag est porte par la colonne 0 de la meme ligne
        bool replaced = index.sibling(index.row(), 0).data(ReplacedRole).toBool();

        painter->save();

        // Fond : selection > survol > ligne remplacee > rien
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, QColor("#26262b"));
        } else if (option.state & QStyle::State_MouseOver) {
            painter->fillRect(option.rect, QColor("#1d1d21"));
        } else if (replaced) {
            painter->fillRect(option.rect, QColor("#1c1f1c"));
        }

        // Barre d'accent verte sur le bord gauche de la ligne
        if (replaced && index.column() == 0) {
            QRect accent(option.rect.left(), option.rect.top(), 3, option.rect.height());
            painter->fillRect(accent, QColor("#7bc98a"));
        }

        painter->restore();

        if (index.column() != 0) {
            // Colonnes texte (statut...) : dessin standard par-dessus notre fond
            QStyleOptionViewItem opt = option;
            initStyleOption(&opt, index);
            opt.backgroundBrush = Qt::NoBrush;
            opt.state &= ~QStyle::State_Selected;
            opt.state &= ~QStyle::State_MouseOver;
            QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
            return;
        }

        // Colonne 0 : titre + nom de fichier sur deux niveaux
        QString title = index.data(Qt::DisplayRole).toString();
        QString filename;
        QModelIndex fileIndex = index.sibling(index.row(), 1);
        if (fileIndex.isValid()) {
            filename = fileIndex.data(Qt::DisplayRole).toString();
        }

        QRect r = option.rect.adjusted(replaced ? 13 : 10, 6, -10, -6);

        painter->save();

        QFont titleFont = option.font;
        titleFont.setPointSize(10);
        titleFont.setBold(replaced);
        painter->setFont(titleFont);
        painter->setPen(QColor(replaced ? "#f0f0f2" : "#e8e8ea"));
        QRect titleRect(r.left(), r.top(), r.width(), r.height() / 2);
        painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, title);

        if (!filename.isEmpty()) {
            QFont fileFont("Consolas", 8);
            painter->setFont(fileFont);
            painter->setPen(QColor("#6e6e78"));
            QRect fileRect(r.left(), r.top() + r.height() / 2, r.width(), r.height() / 2);
            painter->drawText(fileRect, Qt::AlignLeft | Qt::AlignVCenter, filename);
        }

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override
    {
        QSize s = QStyledItemDelegate::sizeHint(option, index);
        if (index.column() == 0) {
            s.setHeight(44);
        }
        return s;
    }
};

class CategoryDelegate : public QStyledItemDelegate {
public:
    explicit CategoryDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        bool isHeader = index.data(Qt::UserRole).toInt() == -1;
        if (!isHeader) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        painter->save();
        painter->fillRect(option.rect, QColor("#121214")); // même fond que la sidebar

        QFont font = option.font;
        font.setBold(true);
        font.setPointSize(font.pointSize() - 1);
        font.setLetterSpacing(QFont::AbsoluteSpacing, 1);
        painter->setFont(font);
        painter->setPen(QColor("#a08cff"));

        QRect textRect = option.rect.adjusted(10, 0, -10, 0);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,
                          index.data(Qt::DisplayRole).toString());
        painter->restore();
    }
};

#endif // TRACKDELEGATE_H
