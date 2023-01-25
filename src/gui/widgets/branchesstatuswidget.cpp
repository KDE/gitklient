/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchesstatuswidget.h"

#include "actions/branchactions.h"
#include "core/kmessageboxhelper.h"
#include "gitmanager.h"
#include "models/branchesmodel.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <QDebug>

BranchesStatusWidget::BranchesStatusWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase{git, parent}
    , mActions{new BranchActions{git, this}}

{
    setupUi(this);
    init(git);
}

void BranchesStatusWidget::init(Git::Manager *git)
{
    mModel = git->branchesModel();
    treeView->setModel(mModel);

    comboBoxReferenceBranch->setModel(mModel);

    pushButtonNew->setAction(mActions->actionCreate());
    pushButtonBrowse->setAction(mActions->actionBrowse());
    pushButtonCheckout->setAction(mActions->actionCheckout());
    pushButtonDiff->setAction(mActions->actionDiff());
    pushButtonRemoveSelected->setAction(mActions->actionRemove());

    mActions->setOtherBranch(comboBoxReferenceBranch->currentText());

    connect(comboBoxReferenceBranch, &QComboBox::currentIndexChanged, this, &BranchesStatusWidget::slotComboBoxReferenceBranchCurrentIndexChanged);
    connect(pushButtonRemoveSelected, &QPushButton::clicked, this, &BranchesStatusWidget::slotPushButtonRemoveSelectedClicked);
    connect(treeView, &QTreeView::customContextMenuRequested, this, &BranchesStatusWidget::slotTreeViewCustomContextMenuRequested);
}

void BranchesStatusWidget::saveState(QSettings &settings) const
{
    save(settings, treeView);
}

void BranchesStatusWidget::restoreState(QSettings &settings)
{
    restore(settings, treeView);
}

void BranchesStatusWidget::slotComboBoxReferenceBranchCurrentIndexChanged(int)
{
    auto selectedBranch = comboBoxReferenceBranch->currentText();
    mModel->setReferenceBranch(selectedBranch);
    mActions->setOtherBranch(selectedBranch);
}

void BranchesStatusWidget::slotPushButtonRemoveSelectedClicked()
{
    if (!treeView->currentIndex().isValid())
        return;

    if (KMessageBoxHelper::removeQuestion(this, i18n("Are you sure to remove the selected branch?"))) {
        auto branchData = mGit->branchesModel()->fromIndex(treeView->currentIndex());
        if (branchData) {
            if (!mGit->removeBranch(branchData->name)) {
                KMessageBox::information(this, i18n("Unable to remove the selected branch"));
                return;
            }
            mGit->branchesModel()->load();
        }
    }
}

void BranchesStatusWidget::slotTreeViewCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto b = mModel->fromIndex(treeView->currentIndex());
    if (!b)
        return;
    mActions->setBranchName(b->name);
    mActions->popup();
}
