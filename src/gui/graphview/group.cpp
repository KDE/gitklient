/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "group.h"
#include "node.h"
#include "scene.h"
#include "node.h"

#include <QDebug>
#include <QPainter>

namespace GraphView {

Group::Group(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    setZValue(10);
}

QRectF Group::boundingRect() const
{
    return {{0, 0}, mSize};
}

void Group::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->fillRect(boundingRect(), mBackgroundColor);
    painter->drawRect(boundingRect());

    if (mOrientation == Qt::Horizontal) {
        painter->translate(mSize.width() - 10, 0);
        painter->rotate(90);
        QRect titleRect{0, 0, static_cast<int>(mSize.height()), painter->fontMetrics().height()};

        painter->setPen(mTextColor);
        painter->drawText(titleRect, Qt::AlignCenter, mTitle);
    } else {
        QRect titleRect{0, 0, static_cast<int>(mSize.width()), painter->fontMetrics().height()};
        titleRect.moveTop(mSize.height() - titleRect.height() - 10);

        painter->setPen(mTextColor);
        painter->drawText(titleRect, Qt::AlignCenter, mTitle);
    }
}

void Group::setNodes(const QList<Node *> &items)
{
    prepareGeometryChange();
    qreal x = std::numeric_limits<qreal>::max();
    qreal y = std::numeric_limits<qreal>::max();
    qreal r = 0;
    qreal b = 0;

    for (const auto &node : items) {
        auto c = node->centerPos();
        x = qMin(x, c.x() - (node->radius()));
        y = qMin(y, c.y() - (node->radius()));
        r = qMax(r, c.x() + (node->radius()));
        b = qMax(b, c.y() + (node->radius()));
    }

    setPos({x - mPadding, y - mPadding});
    if (mOrientation == Qt::Horizontal)
        mSize = {r - x + 2 * mPadding + 30, b - y + 2 * mPadding};
    else
        mSize = {r - x + 2 * mPadding, b - y + 2 * mPadding + 30};
}

const QString &Group::title() const
{
    return mTitle;
}

void Group::setTitle(const QString &newTitle)
{
    mTitle = newTitle;
    update();
}

const QColor &Group::backgroundColor() const
{
    return mBackgroundColor;
}

void Group::setBackgroundColor(const QColor &newBackgroundColor)
{
    mBackgroundColor = newBackgroundColor;
    update();
}

int Group::padding() const
{
    return mPadding;
}

void Group::setPadding(int newPadding)
{
    if (mPadding == newPadding)
        return;

    prepareGeometryChange();
    mPadding = newPadding;
    update();
}

const QColor &Group::textColor() const
{
    return mTextColor;
}

void Group::setTextColor(const QColor &newTextColor)
{
    mTextColor = newTextColor;
    update();
}

Qt::Orientation Group::orientation() const
{
    return mOrientation;
}

void Group::setOrientation(Qt::Orientation newOrientation)
{
    prepareGeometryChange();
    mOrientation = newOrientation;
    update();
}

}
