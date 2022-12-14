/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "reposettingsdialog.h"

#include "gitmanager.h"

RepoSettingsDialog::RepoSettingsDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);

    lineEditUserName->setText(git->config(QStringLiteral("user.name")));
    lineEditUserEmail->setText(git->config(QStringLiteral("user.email")));
    checkBoxAutoCrlf->setChecked(git->configBool(QStringLiteral("core.autocrlf")));
    checkBoxAutoFileMode->setChecked(git->configBool(QStringLiteral("core.fileMode")));

    connect(buttonBox, &QDialogButtonBox::accepted, this, &RepoSettingsDialog::slotAccepted);
}

void RepoSettingsDialog::slotAccepted()
{
    mGit->setConfig(QStringLiteral("user.name"), lineEditUserName->text());
    mGit->setConfig(QStringLiteral("user.email"), lineEditUserEmail->text());
    mGit->setConfig(QStringLiteral("core.fileMode"), checkBoxAutoFileMode->isChecked() ? QStringLiteral("true") : QStringLiteral("false"));
    mGit->setConfig(QStringLiteral("core.autocrlf"), checkBoxAutoCrlf->isChecked() ? QStringLiteral("true") : QStringLiteral("false"));
    accept();
}
