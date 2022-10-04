/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "authorsmodel.h"
#include <KLocalizedString>

namespace Git
{

AuthorsModel::AuthorsModel(Manager *git, QObject *parent)
    : AbstractGitItemsModel{git, parent}
{
}

int AuthorsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData.size();
}

int AuthorsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QVariant AuthorsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case 0:
        return i18n("Name");
    case 1:
        return i18n("Email");
    }
    return {};
}

QVariant AuthorsModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return {};

    switch (index.column()) {
    case 0:
        return mData.at(index.row())->name;
    case 1:
        return mData.at(index.row())->email;
    }

    return {};
}

Author *AuthorsModel::findOrCreate(const QString &name, const QString &email)
{
    for (auto &a : mData)
        if (a->email == email)
            return a;
    auto author = new Author;
    author->name = name;
    author->email = email;
    beginInsertRows(QModelIndex(), mData.size(), mData.size());
    mData.append(author);
    endInsertRows();
    return author;
}

void AuthorsModel::fill()
{
}

} // namespace Git