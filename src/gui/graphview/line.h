/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef LINEITEM_H
#define LINEITEM_H

#include <QGraphicsItem>

namespace GraphView {

class Line : public QGraphicsItem
{
public:
    enum LineType{
        Curved,
        Direct,
        TwoLines
    };
    Line(QGraphicsItem *parent = nullptr);
    Line(const QLineF &line, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    const QLineF &line() const;
    void setLine(const QLineF &newLine);

    const QColor &color() const;
    void setColor(const QColor &newColor);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation newOrientation);

    LineType lineType() const;
    void setLineType(LineType newLineType);

    Qt::PenStyle penStyle() const;
    void setPenStyle(Qt::PenStyle newPenStyle);

private:
    enum LineDirection { ToTopLeft, ToTopRight, ToBottomLeft, ToBottomRight };


    QLineF mLine;
    QSizeF mSize;
    QPolygonF mArrowPoly;
    LineDirection mLineDirection;
    QColor mColor;
    Qt::Orientation mOrientation{Qt::Vertical};
    LineType mLineType{Curved};
    Qt::PenStyle mPenStyle{Qt::SolidLine};
};

}

#endif // LINEITEM_H
