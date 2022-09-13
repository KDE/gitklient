/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "searchdialog.h"
#include "fileviewerdialog.h"

#include <KLocalizedString>
#include <QStandardItemModel>
#include <QtConcurrent>
#include <git/gitlog.h>
#include <git/gitmanager.h>

SearchDialog::SearchDialog(const QString &path, Git::Manager *git, QWidget *parent)
    : AppDialog(parent)
    , mGit(git)
{
    setupUi(this);
    initModel();
    lineEditPath->setText(path);
}

void SearchDialog::initModel()
{
    if (!mModel) {
        mModel = new QStandardItemModel(this);
        treeView->setModel(mModel);
    }

    mModel->setColumnCount(3);
    mModel->setHeaderData(0, Qt::Horizontal, i18n("File name"));
    mModel->setHeaderData(1, Qt::Horizontal, i18n("Branch"));
    mModel->setHeaderData(2, Qt::Horizontal, i18n("Commit"));
}

SearchDialog::SearchDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(parent)
    , mGit(git)
{
    setupUi(this);
    initModel();
}

void SearchDialog::on_pushButtonSearch_clicked()
{
    mModel->clear();
    initModel();
    startTimer(500);
    pushButtonSearch->setEnabled(false);
    _progress.total = _progress.value = 0;
    auto f = QtConcurrent::run(this, &SearchDialog::beginSearch);
}

void SearchDialog::on_treeView_doubleClicked(QModelIndex index)
{
    if (!index.isValid())
        return;
    auto file = mModel->data(mModel->index(index.row(), 0)).toString();
    auto branch = mModel->data(mModel->index(index.row(), 1)).toString();
    auto commit = mModel->data(mModel->index(index.row(), 2)).toString();

    QString place;
    if (!commit.isEmpty() && !branch.isEmpty())
        place = branch + ":" + commit;
    else
        place = branch.isEmpty() ? commit : branch;

    auto d = new FileViewerDialog(place, file);
    d->setWindowModality(Qt::ApplicationModal);
    d->setAttribute(Qt::WA_DeleteOnClose, true);
    d->show();
}

void SearchDialog::beginSearch()
{
    if (radioButtonSearchBranches->isChecked()) {
        const auto branchesList = mGit->branches();
        _progress.total = branchesList.size();
        for (const auto &branch : branchesList) {
            searchOnPlace(branch, QString());
            _progress.value++;
        }
    } else {
        Git::LogList list;
        list.load();

        _progress.total = list.size();
        for (auto &branch : list) {
            searchOnPlace(QString(), branch->commitHash());
            _progress.value++;
        }
    }

    pushButtonSearch->setEnabled(true);
}

void SearchDialog::searchOnPlace(const QString &branch, const QString &commit)
{
    QString place = branch.isEmpty() ? commit : branch;
    auto files = mGit->ls(place);

    for (auto &file : files) {
        if (!lineEditPath->text().isEmpty() && !file.contains(lineEditPath->text()))
            continue;

        bool ok = mGit->fileContent(place, file).contains(lineEditText->text(), checkBoxCaseSensetive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
        if (ok) {
            mModel->appendRow({new QStandardItem(file), new QStandardItem(branch), new QStandardItem(commit)});
        }
    }
}

void SearchDialog::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    progressBar->setMaximum(_progress.total);
    progressBar->setValue(_progress.value);
}
