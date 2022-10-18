/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef NODE_H
#define NODE_H

#include <QGraphicsItemGroup>

namespace GraphView {

class Node : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:
    enum DescriptPosition {
        Left,
        Right
    };

    Node(QGraphicsItem *parent = nullptr);

    QString title() const;
    void setTitle(const QString &newTitle);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &newBackgroundColor);

    QColor borderColor() const;
    void setBorderColor(const QColor &newBorderColor);

    QColor textColor() const;
    void setTextColor(const QColor &newTextColor);

    qreal radius() const;
    void setRadius(qreal newRadius);

    QString descript() const;
    void setDescript(const QString &newDescript);

    QPointF centerPos() const;
    void setCenterPos(const QPointF &pt);

    QPointF edgePoint(const Qt::Edges &edge) const;
    DescriptPosition descriptPosition() const;
    void setDescriptPosition(DescriptPosition newDescriptPosition);

    void *data() const;
    void setData(void *newData);

    bool isSelected() const;
    void setIsSelected(bool newIsSelected);

protected:
    bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;

Q_SIGNALS:
    void clicked();

private:
    void refreshChildPositions();

    QGraphicsEllipseItem *ellipse;
    QGraphicsTextItem *titleTextItem;
    QGraphicsTextItem *descriptTextItem;
    bool mIsSelected{false};
    qreal mRadius;
    DescriptPosition mDescriptPosition{Right};
    void *mData;
    QColor mBackgroundColor;
};

}

#endif // NODE_H
