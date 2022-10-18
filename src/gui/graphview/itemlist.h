/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef ITEMSGROUP_H
#define ITEMSGROUP_H


#include <QGraphicsItem>
#include <QList>

namespace GraphView {

class ItemList
{
private:
    QList<QGraphicsItem*> list;

public:
    ItemList();

    template <class T>
    void append(const QList<T*> &from)
    {
        for (auto &i : from)
            list.append(i);
    }

    template <class T>
    void append(T* object)
    {
        list << object;
    }

    void setVisible(bool visible);
    void clear();
};

}

#endif // ITEMSGROUP_H
