/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "itemlist.h"

namespace GraphView {

ItemList::ItemList()
{

}

void ItemList::setVisible(bool visible)
{
    for (auto &i : list)
        i->setVisible(visible);
}

void ItemList::clear()
{
    list.clear();
}

}
