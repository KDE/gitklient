/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef GRAPHSCENE_H
#define GRAPHSCENE_H

#include <QColor>
#include <QGraphicsScene>

namespace GraphView {

class Line;
class Group;
class Node;
class ItemList;

struct SceneProperties {
    static constexpr int cellSize{50};
    static constexpr int nodeSize{60};
    static constexpr int nodeRadius{15};
    static constexpr int lineEllipseSize{3};
    static constexpr int groupPadding{10};
    constexpr static QColor groupBackgroundColor{230, 230, 250};
    constexpr static QColor nodeBackgroundColor{160, 160, 250};
    constexpr static QColor nodeBorderColor{90, 90, 160};
    constexpr static QColor textColor{90, 90, 160};
    constexpr static QColor relationColor{170, 170, 170};
};

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    Scene(QObject *parent = nullptr);

    Line *addArrow(Node *from, Node *to);
    Line *addRelation(Node *from, Node *to);
    Node *addNode(const QPointF &pt, const QString &title);
    Node *addNode(int row, int col, const QString &title);
    Group *addGroup(const QList<Node *> &list, const QString &title);
    QList<Line *> addArrows(const QList<Node *> &nodes);

    const SceneProperties &properties() const;

    enum PosType { Current, Other, Common };

    QPoint pos(int index, PosType posType);

    ItemList *currentStorage() const;
    void setCurrentStorage(ItemList *newCurrentStorage);

private Q_SLOTS:
    void node_clicked();

private:
    SceneProperties mProperties;
    Qt::Orientation mOrientation{Qt::Vertical};
    ItemList *mCurrentStorage{nullptr};
};

}

#endif // GRAPHSCENE_H
