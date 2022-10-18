/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "scene.h"

#include "group.h"
#include "line.h"
#include "node.h"
#include "itemlist.h"

namespace GraphView {

Scene::Scene(QObject *parent) : QGraphicsScene(parent)
{

}

Line *Scene::addArrow(Node *from, Node *to)
{
    QLineF line;
    switch (mOrientation) {
    case Qt::Horizontal:
        line = {from->edgePoint(Qt::RightEdge), to->edgePoint(Qt::LeftEdge)};
        break;
    case Qt::Vertical:
        line = {from->edgePoint(Qt::BottomEdge), to->edgePoint(Qt::TopEdge)};
        break;
    }

    auto l = new Line(line);
    l->setOrientation(mOrientation);
    l->setLine(line);
    l->setColor(mProperties.nodeBorderColor);
//    l->setLineType(Line::TwoLines);

    if (mCurrentStorage)
        mCurrentStorage->append(l);

    addItem(l);
    return l;
}

Line *Scene::addRelation(Node *from, Node *to)
{
    QLineF line{from->centerPos(), to->centerPos()};

    auto l = new Line(line);
    l->setLineType(Line::Direct);
    l->setOrientation(mOrientation);
    l->setLine(line);
    l->setColor(mProperties.relationColor);
    l->setPenStyle(Qt::DashLine);

    if (mCurrentStorage)
        mCurrentStorage->append(l);

    addItem(l);
    return l;
}

Node *Scene::addNode(const QPointF &pt, const QString &title)
{
    auto n = new Node;
    n->setTitle(title);
    n->setDescriptPosition(Node::Left);
    n->setRadius(mProperties.nodeRadius);
    n->setBackgroundColor(mProperties.nodeBackgroundColor);
    n->setBorderColor(mProperties.nodeBorderColor);
    n->setTextColor(mProperties.textColor);
    n->setCenterPos(pt);

    if (mCurrentStorage)
        mCurrentStorage->append(n);

    addItem(n);

    return n;
}

Node *Scene::addNode(int row, int col, const QString &title)
{
    auto node = addNode({static_cast<qreal>(row * mProperties.cellSize),
                         static_cast<qreal>(col * mProperties.cellSize)},
                        title);

    node->setDescriptPosition(row ? Node::Right : Node::Left);
    if (mCurrentStorage)
        mCurrentStorage->append(node);
    return node;
}

Group *Scene::addGroup(const QList<Node *> &list, const QString &title)
{
    auto g = new Group;
    g->setOrientation(mOrientation);
    g->setPadding(mProperties.groupPadding);
    g->setNodes(list);
    g->setTitle(title);
    g->setBackgroundColor(mProperties.groupBackgroundColor);
    g->setTextColor(mProperties.textColor);
    addItem(g);

    if (mCurrentStorage)
        mCurrentStorage->append(g);
    return g;
}

QList<Line*> Scene::addArrows(const QList<Node *> &nodes)
{
    QList<Line*> ret;
    if (nodes.size() <= 1)
        return {};
    Node *prevNode{nullptr};
    for (const auto &n : nodes) {
        if (prevNode)
            ret << addArrow(prevNode, n);
        prevNode = n;
    }
    return ret;
}

void Scene::node_clicked()
{
//    auto node = qobject_cast<Node*>(sender());

//    if (mSelectedNode)
//        mSelectedNode->setIsSelected(false);

//    mSelectedNode = node;
}

ItemList *Scene::currentStorage() const
{
    return mCurrentStorage;
}

void Scene::setCurrentStorage(ItemList *newCurrentStorage)
{
    mCurrentStorage = newCurrentStorage;
}

QPoint Scene::pos(int index, PosType posType)
{
    switch (mOrientation) {
    case Qt::Horizontal:
        switch (posType) {
        case Current:
            return {index * mProperties.cellSize, 0};
            break;
        case Other:
            return {index * mProperties.cellSize, mProperties.cellSize};
            break;

        case Common:
            return {index * mProperties.cellSize, mProperties.cellSize / 2};
            break;
        }
        break;
    case Qt::Vertical:
        switch (posType) {
        case Current:
            return {0, index * mProperties.cellSize};
            break;
        case Other:
            return {mProperties.cellSize, index * mProperties.cellSize};
            break;

        case Common:
            return {mProperties.cellSize / 2, index * mProperties.cellSize};
            break;
        }
        break;
    }

    return {};
}

const SceneProperties &Scene::properties() const
{
    return mProperties;
}

}
