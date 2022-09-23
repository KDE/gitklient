/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "historymodel.h"
#include "git/gitlog.h"
#include <KLocalizedString>

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    reload();
    //    _logs = Git::Manager::instance()->logs();
}

const QString &HistoryModel::branch() const
{
    return mBranch;
}

void HistoryModel::setBranch(const QString &newBranch)
{
    if (mBranch == newBranch)
        return;

    mBranch = newBranch;
    mLogs.setBranch(mBranch);
    reload();
}

void HistoryModel::reload()
{
    beginResetModel();
    if (!mLogs.empty()) {
        qDeleteAll(mLogs);
        mLogs.clear();
    }

    mLogs.load();

    endResetModel();
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mLogs.size();
}

int HistoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mBranch.isEmpty() ? 1 : 3;
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mLogs.size())
        return {};
    if (role != Qt::DisplayRole)
        return {};

    auto log = mLogs.at(index.row());

    if (mBranch.isEmpty()) {
        switch (index.column()) {
        case 0:
            return "";
        case 1:
            return log->subject();
        }
    } else {
        switch (index.column()) {
        case 0:
            return log->subject();
        case 1:
            return log->commitDate();
        case 2:
            return log->authorName();
        }
    }

    return {};
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Vertical)
        return section + 1;

    if (mBranch.isEmpty()) {
        switch (section) {
        case 0:
            return i18n("Graph");
        case 1:
            return i18n("Message");
        }
    } else {
        switch (section) {
        case 0:
            return i18n("Message");
        case 1:
            return i18n("Date");
        case 2:
            return i18n("Author");
        }
    }
    return {};
}

Git::Log *HistoryModel::log(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() >= mLogs.size())
        return nullptr;

    return mLogs.at(index.row());
}

QModelIndex HistoryModel::findIndexByHash(const QString &hash) const
{
    int idx{0};
    for (const auto &log : mLogs)
        if (log->commitHash() == hash)
            return index(idx);
        else
            idx++;
    return {};
}

Git::Log *HistoryModel::findLogByHash(const QString &hash) const
{
    int idx{0};
    for (const auto &log : qAsConst(mLogs))
        if (log->commitHash() == hash)
            return log;
        else
            idx++;
    return nullptr;
}
