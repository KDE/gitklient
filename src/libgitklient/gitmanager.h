// Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "blamedata.h"
#include "commands/abstractcommand.h"
#include "filestatus.h"
#include "gitfile.h"
#include "gitloglist.h"
#include "gitremote.h"
#include "gitsubmodule.h"
#include "libgitklient_export.h"
#include "stash.h"

#include <QObject>
#include <QString>

namespace Git
{

class RemotesModel;
class SubmodulesModel;
class BranchesModel;
class LogsModel;
class StashesModel;
class TagsModel;

enum LoadFlag {
    LoadNone = 0,
    LoadStashes = 1,
    LoadRemotes = 2,
    LoadSubmodules = 4,
    LoadBranches = 8,
    LoadLogs = 16,
    LoadTags = 32,
    LoadAll = LoadStashes | LoadRemotes | LoadSubmodules | LoadBranches | LoadLogs | LoadTags
};
Q_DECLARE_FLAGS(LoadFlags, LoadFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(LoadFlags)

class LIBGITKLIENT_EXPORT Manager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(bool isMerging READ isMerging WRITE setIsMerging NOTIFY isMergingChanged)

public:
    enum ChangeStatus {
        Unknown,
        Unmodified,
        Modified,
        Added,
        Removed,
        Renamed,
        Copied,
        UpdatedButInmerged,
        Ignored,
        Untracked,
    };

    struct Log {
        QString hash;
        QString author;
        QString date;
        QString message;
    };

    Manager();
    explicit Manager(const QString &path);
    static Manager *instance();

    Q_REQUIRED_RESULT QString currentBranch() const;

    QString run(const AbstractCommand &cmd) const;

    void init(const QString &path);

    bool isGitDir() const;
    QByteArray runGit(const QStringList &args) const;
    Q_REQUIRED_RESULT QStringList ls(const QString &place) const;
    QString fileContent(const QString &place, const QString &fileName) const;
    void saveFile(const QString &place, const QString &fileName, const QString &localFile) const;
    Q_REQUIRED_RESULT QStringList branches() const;
    Q_REQUIRED_RESULT QStringList remoteBranches() const;
    Q_REQUIRED_RESULT QStringList remotes() const;

    Q_REQUIRED_RESULT QStringList tags() const;
    void createTag(const QString &name, const QString &message) const;

    QList<Stash> stashes();
    void createStash(const QString &name = QString()) const;
    bool removeStash(const QString &name) const;
    bool applyStash(const QString &name) const;

    Remote remoteDetails(const QString &remoteName);
    bool removeBranch(const QString &branchName) const;

    BlameData blame(const File &file);

    QList<Git::Submodule> submodules() const;
    bool addSubmodule(const Git::Submodule &module);
    void revertFile(const QString &filePath) const;

    QMap<QString, Manager::ChangeStatus> changedFiles() const;
    void commit(const QString &message) const;
    void push() const;
    void addFile(const QString &file) const;
    void removeFile(const QString &file, bool cached) const;

    const QString &path() const;
    void setPath(const QString &newPath);

    QMap<QString, ChangeStatus> changedFiles(const QString &hash) const;

    QStringList ignoredFiles() const;
    QList<FileStatus> repoFilesStatus() const;

    QList<Log *> log(const QString &branch) const;
    bool isValid() const;

    bool addRemote(const QString &name, const QString &url) const;
    bool removeRemote(const QString &name) const;
    bool renameRemote(const QString &name, const QString &newName) const;

    bool isIgnored(const QString &path);

    QPair<int, int> uniqueCommiteOnBranches(const QString &branch1, const QString &branch2) const;

    QStringList fileLog(const QString &fileName) const;

    QString diff(const QString &from, const QString &to) const;
    QList<FileStatus> diffBranch(const QString &from) const;
    QList<FileStatus> diffBranches(const QString &from, const QString &to) const;

    enum ConfigType { ConfigGlobal, ConfigLocal };

    QString config(const QString &name, ConfigType type = ConfigLocal) const;
    bool configBool(const QString &name, ConfigType type = ConfigLocal) const;
    void setConfig(const QString &name, const QString &value, ConfigType type = ConfigLocal) const;
    void unsetConfig(const QString &name, ConfigType type = ConfigLocal) const;

    RemotesModel *remotesModel() const;
    SubmodulesModel *submodulesModel() const;
    BranchesModel *branchesModel() const;
    LogsModel *logsModel() const;
    StashesModel *stashesModel() const;
    TagsModel *tagsModel() const;

    bool isMerging() const;
    void setIsMerging(bool newIsMerging);

    const LoadFlags &loadFlags() const;
    void setLoadFlags(Git::LoadFlags newLoadFlags);

    QString readNote(const QString &branchName) const;
    void saveNote(const QString &branchName, const QString &note) const;

    QString mergeBase(const QString &branch1, const QString &branch2) const;
    QStringList showCommitsAfter(const QString &branch, const QString &commitHash) const;
    QString commitMessage(const QString &commitHash) const;

Q_SIGNALS:
    void pathChanged();

    void isMergingChanged();

private:
    QString mPath;
    bool _isValid{false};
    QMap<QString, Remote> _remotes;
    LoadFlags _loadFlags{LoadAll};
    bool m_isMerging{false};

    QStringList readAllNonEmptyOutput(const QStringList &cmd) const;
    QString escapeFileName(const QString &filePath) const;
    void loadAsync();

    RemotesModel *const _remotesModel;
    SubmodulesModel *const _submodulesModel;
    BranchesModel *const _branchesModel;
    LogsModel *const _logsCache;
    StashesModel *const _stashesCache;
    TagsModel *const _tagsModel;

    friend class Stash;
    friend class RemotesModel;
    friend class SubmodulesModel;
    friend class BranchesModel;
    friend class LogsModel;
    friend class StashesModel;
    friend class TagsModel;
};

} // namespace Git
