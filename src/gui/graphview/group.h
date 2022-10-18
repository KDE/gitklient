/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef GRAPHGROUPITEM_H
#define GRAPHGROUPITEM_H

#include <QGraphicsItem>

namespace GraphView {

class Node;
class GraphNodeItem;
class Group : public QGraphicsItem
{
public:
    Group(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setNodes(const QList<Node *> &items);

    const QString &title() const;
    void setTitle(const QString &newTitle);

    const QColor &backgroundColor() const;
    void setBackgroundColor(const QColor &newBackgroundColor);

    int padding() const;
    void setPadding(int newPadding);

    const QColor &textColor() const;
    void setTextColor(const QColor &newTextColor);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation newOrientation);

private:
    QSizeF mSize;
    QString mTitle;
    QColor mBackgroundColor;
    QColor mTextColor;
    int mPadding{20};
    Qt::Orientation mOrientation{Qt::Vertical};
};

}

#endif // GRAPHGROUPITEM_H
