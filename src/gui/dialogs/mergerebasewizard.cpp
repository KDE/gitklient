/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mergerebasewizard.h"

#include "commands/commandmerge.h"
#include "commands/commandrebase.h"
#include "gitlog.h"
#include "gitmanager.h"
#include "models/branchesmodel.h"
#include "models/logsmodel.h"

#include "graphview/line.h"
#include "graphview/node.h"
#include "graphview/scene.h"

#include <QDebug>

using namespace GraphView;

MergeRebaseWizard::MergeRebaseWizard(Git::Manager *git, QWidget *parent)
    : QWizard(parent)
    , mGit(git)
{
    setupUi(this);

    connect(comboBoxFrom, &QComboBox::currentIndexChanged, this, &QWizard::validateCurrentPage);
    comboBoxFrom->setModel(mGit->branchesModel());

    scene = new GraphView::Scene(this);
    graphicsView->setScene(scene);

    labelCurrentBranch->setText(mGit->currentBranch());

    logDetails->setEnableFilesLinks(false);
    //    graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

}

bool MergeRebaseWizard::validateCurrentPage()
{
    if (currentId() == 0) {
        if (labelCurrentBranch->text() == comboBoxFrom->currentText()){
            button(NextButton)->setEnabled(false);
            return false;
        }
    }
    button(NextButton)->setEnabled(true);
    return QWizard::validateCurrentPage();
}

void MergeRebaseWizard::initializePage(int id)
{
    switch (id) {
    case 1:
        generateItems();
        showHideItems();
        break;
    default:
        break;
    }
}

void MergeRebaseWizard::node_clicked()
{
    if (selectedNode)
        selectedNode->setIsSelected(false);
    auto node = qobject_cast<Node *>(sender());
    auto log = static_cast<Git::Log *>(node->data());
    logDetails->setLog(log);

    node->setIsSelected(true);
    selectedNode = node;
}

void MergeRebaseWizard::showHideItems()
{
    checkBoxSquash->setEnabled(radioButtonMerge->isChecked());

    mRebaseItems.setVisible(radioButtonRebase->isChecked());

    mRebaseItems.setVisible(radioButtonRebase->isChecked());
    mMergeNoSquashItems.setVisible(!checkBoxSquash->isChecked() && radioButtonMerge->isChecked());

    mMergeSquashItems.setVisible(checkBoxSquash->isChecked() && radioButtonMerge->isChecked());

    mCurrentStateItems.setVisible(!radioButtonMerge->isChecked() && !radioButtonRebase->isChecked());
}

Node *MergeRebaseWizard::addNodeAtCell(int row, int col, const QString &title)
{
    auto node = scene->addNode(row, col, "");
    node->setDescript(title);
    return node;
}

Node *MergeRebaseWizard::addNodeForLog(int pos, int index, const QString &commitHash)
{
    auto n = addNodeAtCell(pos, index, "");
    auto log = mGit->logsModel()->findLogByHash(commitHash);
    QString descript;
    if (log)
        descript = log->subject();
    else
        descript = mGit->commitMessage(commitHash);
    n->setDescript(descript);
    n->setData(log);
    connect(n, &Node::clicked, this, &MergeRebaseWizard::node_clicked);

    return n;
}

Git::AbstractCommand *MergeRebaseWizard::command() const
{
    if (radioButtonMerge->isChecked()) {
        auto cmd = new Git::CommandMerge(mGit);
        cmd->setFromBranch(comboBoxFrom->currentText());
        return cmd;
    }
    if (radioButtonRebase->isChecked()) {
        auto cmd = new Git::CommandRebase(mGit);
        cmd->setFromBranch(comboBoxFrom->currentText());
        return cmd;
    }
    return nullptr;
}

void MergeRebaseWizard::generateItems()
{
    scene->clear();
    mMergeNoSquashItems.clear();
    mMergeSquashItems.clear();
    mRebaseItems.clear();
    mCurrentStateItems.clear();

    QList<Node *> currentItems;
    QList<Node *> currentItems2;
    QList<Node *> currentItems3;
    QList<Node *> otherItems;
    QList<Node *> rebaseItems;

    auto mergeCommitHash = mGit->mergeBase(mGit->currentBranch(), comboBoxFrom->currentText());
    QStringList mCurrentCommitLogs = mGit->showCommitsAfter(mGit->currentBranch(), mergeCommitHash);
    QStringList mOtherCommitLogs = mGit->showCommitsAfter(comboBoxFrom->currentText(), mergeCommitHash);

    auto index = mCurrentCommitLogs.indexOf(mergeCommitHash);
    if (index != -1)
        mCurrentCommitLogs.mid(index);
    index = mOtherCommitLogs.indexOf(mergeCommitHash);
    if (index != -1)
        mOtherCommitLogs.mid(index);

    auto baseNode = addNodeForLog(PosBase, 0, mergeCommitHash);

    int indexCurrent{1};
    int indexOther{1};
    int indexRebase{mOtherCommitLogs.size() + 1};

    auto squashMerge = addNodeAtCell(0, qMax(mCurrentCommitLogs.size(), mOtherCommitLogs.size() + 1), "Squash");
    auto mergeItem = addNodeAtCell(0, qMax(mCurrentCommitLogs.size(), mOtherCommitLogs.size() + 1), "Merged");
    for (const auto &c : std::as_const(mCurrentCommitLogs)) {
        currentItems << addNodeForLog(PosCurrent, indexCurrent, c);
        currentItems2 << addNodeForLog(PosCurrent, indexCurrent, c);
        currentItems3 << addNodeForLog(PosCurrent, indexCurrent, c);
        indexCurrent++;
        rebaseItems << addNodeForLog(PosCurrent, indexRebase++, c);
    }

    for (const auto &c : std::as_const(mOtherCommitLogs)) {
        auto node = addNodeForLog(PosOther, indexOther++, c);
        otherItems << node;
        mMergeSquashItems.append(scene->addRelation(node, squashMerge));
    }

    if (!otherItems.isEmpty())
        scene->addArrow(baseNode, otherItems.first());
    scene->addArrows(otherItems);

    if (!currentItems.isEmpty()) {
        mMergeNoSquashItems.append(scene->addArrow(baseNode, currentItems.first()));
        mMergeNoSquashItems.append(scene->addArrow(currentItems.last(), squashMerge));
        mMergeNoSquashItems.append(scene->addArrow(currentItems.last(), squashMerge));
        mMergeNoSquashItems.append(currentItems);
        mMergeNoSquashItems.append(scene->addArrows(currentItems));
    }
    if (!otherItems.isEmpty()) {
        mMergeNoSquashItems.append(scene->addArrow(otherItems.last(), squashMerge));
    }
    mMergeNoSquashItems.append(mergeItem);

    if (!currentItems.isEmpty()) {
        mMergeSquashItems.append(scene->addArrow(baseNode, currentItems.first()));
        mMergeSquashItems.append(scene->addArrow(currentItems.last(), squashMerge));
    }
    mMergeSquashItems.append(squashMerge);
    mMergeSquashItems.append(currentItems2);
    mMergeSquashItems.append(scene->addArrows(currentItems2));

    if (!currentItems3.isEmpty()) {
        mCurrentStateItems.append(scene->addArrow(baseNode, currentItems3.first()));
    }
    mCurrentStateItems.append(currentItems3);
    mCurrentStateItems.append(scene->addArrows(currentItems3));

    if (!otherItems.isEmpty() && !rebaseItems.isEmpty()) {
        mRebaseItems.append(scene->addArrow(otherItems.last(), rebaseItems.first()));
    }
    mRebaseItems.append(rebaseItems);
    mRebaseItems.append(scene->addArrows(rebaseItems));
}
