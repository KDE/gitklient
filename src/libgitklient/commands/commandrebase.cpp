/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandrebase.h"

namespace Git
{

CommandRebase::CommandRebase(QObject *parent)
    : AbstractCommand{parent}
{
}

QStringList CommandRebase::generateArgs() const
{
    QStringList args{QStringLiteral("rebase"), mFromBranch};

    if (!mToBranch.isEmpty())
        args << QStringLiteral("--onto=") << mToBranch;

    return args;
}

const QString &CommandRebase::fromBranch() const
{
    return mFromBranch;
}

void CommandRebase::setFromBranch(const QString &newFromBranch)
{
    mFromBranch = newFromBranch;
}

const QString &CommandRebase::toBranch() const
{
    return mToBranch;
}

void CommandRebase::setToBranch(const QString &newToBranch)
{
    mToBranch = newToBranch;
}

}
