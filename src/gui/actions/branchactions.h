/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"

class BranchActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit BranchActions(Git::Manager *git, QWidget *parent = nullptr);

    Q_REQUIRED_RESULT const QString &branchName() const;
    void setBranchName(const QString &newBranchName);

    Q_REQUIRED_RESULT const QString &otherBranch() const;
    void setOtherBranch(const QString &newOtherBranch);

private:
    void fetch();
    void create();
    void browse();
    void checkout();
    void diff();
    void remove();
    void merge();
    void note();
    QString mBranchName;
    QString mOtherBranch;

    DEFINE_ACTION(actionFetch)
    DEFINE_ACTION(actionBrowse)
    DEFINE_ACTION(actionCheckout)
    DEFINE_ACTION(actionDiff)
    DEFINE_ACTION(actionMerge)
    DEFINE_ACTION(actionCreate)
    DEFINE_ACTION(actionRemove)
    DEFINE_ACTION(actionNote)
};
