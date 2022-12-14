/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submoduleinfodialog.h"

#include "commands/addsubmodulecommand.h"
#include "gitmanager.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QFileDialog>

SubmoduleInfoDialog::SubmoduleInfoDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);
}

bool SubmoduleInfoDialog::force() const
{
    return checkBoxForce->isChecked();
}

void SubmoduleInfoDialog::setForce(bool newForce)
{
    checkBoxForce->setChecked(newForce);
}

QString SubmoduleInfoDialog::url() const
{
    return lineEditUrl->text();
}

void SubmoduleInfoDialog::setUrl(const QString &newUrl)
{
    lineEditUrl->setText(newUrl);
}

QString SubmoduleInfoDialog::path() const
{
    return lineEditPath->text();
}

void SubmoduleInfoDialog::setPath(const QString &newPath)
{
    lineEditPath->setText(newPath);
}

QString SubmoduleInfoDialog::branch() const
{
    if (checkBoxBranch->isChecked())
        return lineEditBranch->text();
    return {};
}

void SubmoduleInfoDialog::setBranch(const QString &newBranch)
{
    if (newBranch.isEmpty()) {
        checkBoxBranch->setChecked(true);
        lineEditBranch->setText(newBranch);
    } else {
        checkBoxBranch->setChecked(false);
        lineEditBranch->clear();
    }
}

Git::AddSubmoduleCommand *SubmoduleInfoDialog::command() const
{
    auto cmd = new Git::AddSubmoduleCommand(mGit);
    cmd->setForce(checkBoxForce->isChecked());

    if (checkBoxBranch->isChecked())
        cmd->setbranch(lineEditBranch->text());

    cmd->setUrl(lineEditUrl->text());
    cmd->setLocalPath(lineEditPath->text());

    return cmd;
}
