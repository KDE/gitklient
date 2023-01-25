/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QWidget>

#include <QSettings>

class QSplitter;
class QTreeView;
class AppWindow;

namespace Git
{
class Manager;
}
class WidgetBase : public QWidget
{
    Q_OBJECT

protected:
    Git::Manager *mGit{nullptr};
    AppWindow *mParent{nullptr};
    QString stateName(QWidget *w) const;

public:
    explicit WidgetBase(QWidget *parent = nullptr);
    explicit WidgetBase(Git::Manager *git, AppWindow *parent = nullptr);
    Git::Manager *git() const;
    void setGit(Git::Manager *newGit);

    virtual void reload();

    int exec(QWidget *parent = nullptr);

    virtual void saveState(QSettings &settings) const;
    virtual void restoreState(QSettings &settings);

    void save(QSettings &settings, QSplitter *splitter) const;
    void restore(const QSettings &settings, QSplitter *splitter);

    void save(QSettings &settings, QTreeView *treeView) const;
    void restore(const QSettings &settings, QTreeView *treeView);

    virtual void settingsUpdated();
private Q_SLOTS:
    void git_pathChanged();
};
