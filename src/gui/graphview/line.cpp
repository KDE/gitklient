/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "line.h"

#include "scene.h"

#include <QDebug>
#include <QPainter>

namespace GraphView {

Line::Line(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    setZValue(2);
}

Line::Line(const QLineF &line, QGraphicsItem *parent) : QGraphicsItem(parent)
{
    setLine(line);
    setZValue(20);
}

QRectF Line::boundingRect() const
{
    return {0, 0, qMax(1., qAbs(mLine.dx())), qMax(1., qAbs(mLine.dy()))};
}

void Line::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);

    painter->setPen(QPen(mColor, 2, mPenStyle));
    constexpr int ellipseSize = 3;

    switch (mLineType) {
    case Curved: {
        QPointF pts[7];

        switch (mOrientation) {
        case Qt::Horizontal:
            if (mLineDirection == ToTopRight || mLineDirection == ToBottomLeft) {
                pts[0] = {0, mSize.height()};

                pts[1] = {mSize.width() / 2, mSize.height()};
                pts[2] = {mSize.width() / 2, mSize.height() / 2};
                pts[3] = {mSize.width() / 2, mSize.height() / 2};

                pts[4] = {mSize.width() / 2, 0};
                pts[5] = {mSize.width(), 0};
                pts[6] = {mSize.width(), 0};

                break;
            } else {
                pts[0] = {0, 0};

                pts[1] = {mSize.width() / 2, 0};
                pts[2] = {mSize.width() / 2, mSize.height() / 2};
                pts[3] = {mSize.width() / 2, mSize.height() / 2};

                pts[4] = {mSize.width() / 2, mSize.height()};
                pts[5] = {mSize.width(), mSize.height()};
                pts[6] = {mSize.width(), mSize.height()};
            }
        case Qt::Vertical:
            if (mLineDirection == ToTopRight || mLineDirection == ToBottomLeft) {
                pts[0] = {0, mSize.height()};

                pts[1] = {0, mSize.height() / 2};
                pts[2] = {mSize.width() / 2, mSize.height() / 2};
                pts[3] = {mSize.width() / 2, mSize.height() / 2};

                pts[4] = {mSize.width(), mSize.height() / 2};
                pts[5] = {mSize.width(), 0};
                pts[6] = {mSize.width(), 0};

                break;
            } else {
                pts[0] = {0, 0};

                pts[1] = {0, mSize.height() / 2};
                pts[2] = {mSize.width() / 2, mSize.height() / 2};
                pts[3] = {mSize.width() / 2, mSize.height() / 2};

                pts[4] = {mSize.width(), mSize.height() / 2};
                pts[5] = {mSize.width(), mSize.height()};
                pts[6] = {mSize.width(), mSize.height()};
            }
        }

        QPainterPath p;
        p.moveTo(pts[0]);
        p.cubicTo(pts[1], pts[2], pts[3]);
        painter->setBrush(Qt::transparent);
        p.cubicTo(pts[3], pts[4], pts[6]);
        painter->drawPath(p);
        break;
    }
    case Direct: {
        if (mLineDirection == ToTopRight || mLineDirection == ToBottomLeft) {
            painter->drawLine(0, mSize.height(), mSize.width(), 0);
        } else {
            painter->drawLine(0, 0, mSize.height(), mSize.width());
        }
        break;
    }
    case TwoLines: {
        if (mLineDirection == ToTopRight || mLineDirection == ToBottomLeft) {
            painter->drawLine(mSize.width() - 1, 0, mSize.width() - 1, mSize.height());
            painter->drawLine(0, mSize.height() - 1, mSize.width() - 1, mSize.height() - 1);
        }else{
            painter->drawLine(0, 0, 0, mSize.height());
            painter->drawLine(0, mSize.height() - 1, mSize.width() - 1, mSize.height() - 1);
        }
        if (mLine.dx() > mLine.dy()) {
        } else {
        }
    }
    }
}

const QLineF &Line::line() const
{
    return mLine;
}

void Line::setLine(const QLineF &newLine)
{
    mLine = newLine;

    mSize = {qAbs(mLine.dx()), qAbs(mLine.dy())};
    setPos({qMin(mLine.x1(), mLine.x2()), qMin(mLine.y1(), mLine.y2())});

    mArrowPoly.append({mSize.width(), mSize.height() - 10});
    mArrowPoly.append({mSize.width(), mSize.height()});
    mArrowPoly.append({mSize.width() - 10, mSize.height()});

    if (newLine.dx() < 0)
        mLineDirection = newLine.dy() < 0 ? ToTopLeft : ToBottomLeft;
    else
        mLineDirection = newLine.dy() < 0 ? ToTopRight : ToBottomRight;
}

const QColor &Line::color() const
{
    return mColor;
}

void Line::setColor(const QColor &newColor)
{
    mColor = newColor;
}

Qt::Orientation Line::orientation() const
{
    return mOrientation;
}

void Line::setOrientation(Qt::Orientation newOrientation)
{
    mOrientation = newOrientation;
}

Line::LineType Line::lineType() const
{
    return mLineType;
}

void Line::setLineType(LineType newLineType)
{
    mLineType = newLineType;
}

Qt::PenStyle Line::penStyle() const
{
    return mPenStyle;
}

void Line::setPenStyle(Qt::PenStyle newPenStyle)
{
    mPenStyle = newPenStyle;
}

}
