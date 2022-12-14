/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedfileactions.h"
#include "diffwindow.h"
#include "gitmanager.h"

#include <QAction>

#include <KLocalizedString>

ChangedFileActions::ChangedFileActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionDiff = addAction(i18n("Diff..."), this, &ChangedFileActions::diff, false, true);
    _actionRevert = addAction(i18n("Revert..."), this, &ChangedFileActions::revert, false, true);

    auto f = _actionDiff->font();
    f.setBold(true);
    _actionDiff->setFont(f);
}

const QString &ChangedFileActions::filePath() const
{
    return mFilePath;
}

void ChangedFileActions::setFilePath(const QString &newFilePath)
{
    mFilePath = newFilePath;

    setActionEnabled(_actionDiff, true);
    setActionEnabled(_actionRevert, true);
}

void ChangedFileActions::diff()
{
    const Git::File original{mGit, mGit->currentBranch(), mFilePath};
    const Git::File changed{mGit->path() + QLatin1Char('/') + mFilePath};

    auto diffWin = new DiffWindow(original, changed);
    diffWin->showModal();
}

void ChangedFileActions::revert()
{
    mGit->revertFile(mFilePath);
    Q_EMIT reloadNeeded();
}
