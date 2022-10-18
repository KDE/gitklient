/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "node.h"

#include <QPen>
#include <QDebug>
#include <QBrush>
#include <QEvent>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>

namespace GraphView {

Node::Node(QGraphicsItem *parent)
    : QObject()
    , QGraphicsItemGroup(parent)
{
    ellipse = new QGraphicsEllipseItem(this);

    titleTextItem = new QGraphicsTextItem(this);
    descriptTextItem = new QGraphicsTextItem(this);

    addToGroup(ellipse);
    addToGroup(descriptTextItem);
    addToGroup(titleTextItem);

    refreshChildPositions();

    setZValue(30);

    setFiltersChildEvents(true);
    ellipse->setCursor(Qt::PointingHandCursor);
}

QString Node::title() const
{
    return descriptTextItem->toPlainText();
}

void Node::setTitle(const QString &newTitle)
{
    titleTextItem->setPlainText(newTitle);
    refreshChildPositions();
}

QColor Node::backgroundColor() const
{
    return mBackgroundColor;
}

void Node::setBackgroundColor(const QColor &newBackgroundColor)
{
    mBackgroundColor = newBackgroundColor;
    ellipse->setBrush(newBackgroundColor);
}

QColor Node::borderColor() const
{
    return ellipse->pen().color();
}

void Node::setBorderColor(const QColor &newBorderColor)
{
    ellipse->setPen(newBorderColor);
}

QColor Node::textColor() const
{
    return titleTextItem->defaultTextColor();
}

void Node::setTextColor(const QColor &newTextColor)
{
    titleTextItem->setDefaultTextColor(newTextColor);
    descriptTextItem->setDefaultTextColor(newTextColor);
}

qreal Node::radius() const
{
    return mRadius;
}

void Node::setRadius(qreal newRadius)
{
    mRadius = newRadius;
    refreshChildPositions();
}

QString Node::descript() const
{
    return descriptTextItem->toPlainText();
}

void Node::setDescript(const QString &newDescript)
{
    descriptTextItem->setPlainText(newDescript);
    refreshChildPositions();
}

QPointF Node::centerPos() const
{
    return {pos().x() + ellipse->pos().x() + mRadius, pos().y() + ellipse->pos().y() + mRadius};
}

void Node::setCenterPos(const QPointF &pt)
{
    auto p = ellipse->pos();
    setPos({pt.x() - p.x(), pt.y() - p.y()});
    refreshChildPositions();
}

QPointF Node::edgePoint(const Qt::Edges &edge) const
{
    QPointF pt;
    if (edge & Qt::LeftEdge)
        pt.setX(pos().x() + ellipse->pos().x());
    else if (edge & Qt::RightEdge)
        pt.setX(pos().x() + ellipse->pos().x() + ellipse->boundingRect().width());
    else
        pt.setX(pos().x() + ellipse->pos().x() + ellipse->boundingRect().width() / 2);

    if (edge & Qt::TopEdge)
        pt.setY(pos().y() + ellipse->pos().y());
    else if (edge & Qt::BottomEdge)
        pt.setY(pos().y() + ellipse->pos().y() + ellipse->boundingRect().height());
    else
        pt.setY(pos().y() + ellipse->pos().y() + ellipse->boundingRect().height() / 2);

    return pt;
}

Node::DescriptPosition Node::descriptPosition() const
{
    return mDescriptPosition;
}

void Node::setDescriptPosition(DescriptPosition newDescriptPosition)
{
    mDescriptPosition = newDescriptPosition;
    refreshChildPositions();
}

void Node::refreshChildPositions()
{
    prepareGeometryChange();

    auto circle = ellipse->boundingRect();
    switch (mDescriptPosition) {
    case Right:
        ellipse->setRect({0, 0, mRadius * 2, mRadius * 2});
        descriptTextItem->setPos(
            {mRadius * 2, (mRadius * 2 - descriptTextItem->boundingRect().height()) / 2});
        break;
    case Left:
        descriptTextItem->setPos({-descriptTextItem->boundingRect().width(),
                                  (mRadius * 2 - descriptTextItem->boundingRect().height()) / 2});
        ellipse->setRect({0, 0, mRadius * 2, mRadius * 2});
        break;

    default:
        break;
    }
    auto titleRect = titleTextItem->boundingRect();
    titleTextItem->setPos(
        {(circle.width() - titleRect.width()) / 2, (circle.height() - titleRect.height()) / 2});
}

bool Node::isSelected() const
{
    return mIsSelected;
}

void Node::setIsSelected(bool newIsSelected)
{
    mIsSelected = newIsSelected;
    ellipse->setBrush(mIsSelected ? Qt::yellow : mBackgroundColor);
}

bool Node::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if (watched==ellipse) {
        switch (event->type()) {
        case QEvent::GraphicsSceneMousePress:{
            auto me = static_cast<QGraphicsSceneMouseEvent *>(event);
            me->accept();
            Q_EMIT clicked();
            break;
        }
        default:
            break;
        }
    }
    return QGraphicsItem::sceneEventFilter(watched, event);
}

void *Node::data() const
{
    return mData;
}

void Node::setData(void *newData)
{
    mData = newData;
}

}
