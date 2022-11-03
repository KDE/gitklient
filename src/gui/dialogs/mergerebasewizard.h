/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef MERGEREBASEWIZARD_H
#define MERGEREBASEWIZARD_H

#include "graphview/itemlist.h"
#include "ui_mergerebasewizard.h"

namespace GraphView
{
class Node;
class Scene;
} // namespace GraphView
namespace Git
{
class Manager;
class AbstractCommand;
}
class MergeRebaseWizard : public QWizard, private Ui::MergeRebaseWizard
{
    Q_OBJECT

public:
    explicit MergeRebaseWizard(Git::Manager *git, QWidget *parent = nullptr);
    bool validateCurrentPage() override;

protected:
    void initializePage(int id) override;

private Q_SLOTS:
    void node_clicked();
    void showHideItems();

public Q_SLOTS:
    void done(int code) override;

private:
    Git::Manager *mGit;
    GraphView::Scene *scene;

    GraphView::ItemList mMergeNoSquashItems;
    GraphView::ItemList mMergeSquashItems;
    GraphView::ItemList mRebaseItems;
    GraphView::ItemList mCurrentStateItems;

    void generateItems();
    GraphView::Node *addNodeAtCell(int row, int col, const QString &title);
    GraphView::Node *addNodeForLog(int pos, int index, const QString &commitHash);

    GraphView::Node *selectedNode{nullptr};

    Git::AbstractCommand *command() const;
    static constexpr int PosCurrent{0};
    static constexpr int PosBase{1};
    static constexpr int PosOther{2};
};

#endif // MERGEREBASEWIZARD_H
