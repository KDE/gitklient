/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandargsparser.h"

#include "appwindow.h"
#include "commands/commandclean.h"
#include "commands/commandmerge.h"
#include "commands/commandswitchbranch.h"
#include "dialogs/changedfilesdialog.h"
#include "dialogs/cleanupdialog.h"
#include "dialogs/clonedialog.h"
#include "dialogs/commitpushdialog.h"
#include "dialogs/fileblamedialog.h"
#include "dialogs/filehistorydialog.h"
#include "dialogs/ignorefiledialog.h"
#include "dialogs/initdialog.h"
#include "dialogs/mergedialog.h"
#include "dialogs/mergerebasewizard.h"
#include "dialogs/pulldialog.h"
#include "dialogs/runnerdialog.h"
#include "dialogs/switchbranchdialog.h"
#include "dialogs/taginfodialog.h"
#include "diffwindow.h"
#include "gitfile.h"
#include "gitmanager.h"
#include "mergewindow.h"

#include "gitklient_appdebug.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMetaMethod>

#include <KMessageBox>
#include <klocalizedstring.h>
#include <kwidgetsaddons_version.h>

namespace Errors
{
constexpr int InvalidPath = 1;
};

#define checkGitPath(path)                                                                                                                                     \
    do {                                                                                                                                                       \
        QFileInfo fi(path);                                                                                                                                    \
        if (fi.isFile())                                                                                                                                       \
            git->setPath(fi.absolutePath());                                                                                                                   \
        else                                                                                                                                                   \
            git->setPath(path);                                                                                                                                \
        if (!git->isValid()) {                                                                                                                                 \
            KMessageBox::error(nullptr, i18n("The path is not git repo: %1", path));                                                                           \
            return 1;                                                                                                                                          \
        }                                                                                                                                                      \
    } while (false)

CommandArgsParser::CommandArgsParser()
    : QObject()
{
    git = Git::Manager::instance();
}

void CommandArgsParser::add(const QString &name, const CommandList &list)
{
    mCommands.insert(name, list);
}

void CommandArgsParser::add(const QString &name, const QString &list)
{
    CommandList cmdList;
    const auto parts = list.split(QLatin1Char(' '));
    for (const auto &pp : parts) {
        auto p = pp;
        bool isOptional{false};
        if (p.startsWith(QLatin1String("[")) && p.endsWith(QLatin1String("]"))) {
            isOptional = true;
            p = p.mid(1, p.length() - 2);
        }

        if (p.startsWith(QLatin1Char('<')) && p.endsWith(QLatin1Char('>')))
            cmdList.append({Command::Named, p.mid(1, p.length() - 2), isOptional});
        else
            cmdList.append({Command::Fixed, p, isOptional});
    }
    add(name, cmdList);
}

bool CommandArgsParser::check(const CommandList &commands)
{
    mParams.clear();
    if (qApp->arguments().size() != commands.size() + 1)
        return false;
    const auto appArgs = qApp->arguments();

    int idx{1};
    for (const auto &cmd : commands) {
        switch (cmd.type) {
        case Command::Fixed:
            if (appArgs[idx] != cmd.s)
                return false;
            break;
        case Command::Named:
            mParams.insert(cmd.s, appArgs[idx]);
            break;
        }
        idx++;
    }
    return true;
}

QString CommandArgsParser::checkAll()
{
    for (auto i = mCommands.begin(); i != mCommands.end(); ++i)
        if (check(i.value()))
            return i.key();
    return {};
}

QString CommandArgsParser::param(const QString &name) const
{
    return mParams.value(name);
}

ArgParserReturn CommandArgsParser::run(const QStringList &args)
{
#define GET_OP(x) params.size() > x ? Q_ARG(QString, params.at(x)) : QGenericArgument()
    if (args.size() == 1)
        return main();
    const auto name = QString(args.at(1)).replace(QLatin1String("-"), QLatin1String("_")).toLocal8Bit();
    const auto c = metaObject()->methodCount();
    qCDebug(GITKLIENT_LOG) << "Running" << args;
    for (int i = 0; i < c; i++) {
        const auto method = metaObject()->method(i);

        if (method.name().compare(name, Qt::CaseInsensitive) == 0) {
            if (method.parameterCount() != args.size() - 1) {
                auto params = args.mid(2);
                ArgParserReturn r;
                qCDebug(GITKLIENT_LOG) << "Running:" << method.name();
                auto b = metaObject()->invokeMethod(this,
                                                    method.name().constData(),
                                                    Q_RETURN_ARG(ArgParserReturn, r),
                                                    GET_OP(0),
                                                    GET_OP(1),
                                                    GET_OP(2),
                                                    GET_OP(3),
                                                    GET_OP(4),
                                                    GET_OP(5),
                                                    GET_OP(6),
                                                    GET_OP(7),
                                                    GET_OP(8),
                                                    GET_OP(9));

                if (!b) {
                    qCDebug(GITKLIENT_LOG) << args.size() << method.parameterCount();
                }

                return r;
            }
        }
    }
#undef GET_OP
    qWarning().noquote() << "Method not found" << args.at(1);

    if (args.size() == 2)
        return main(args.at(1));
    return main();
}

ArgParserReturn CommandArgsParser::help()
{
    const auto c = metaObject()->methodCount();

    for (auto i = metaObject()->classInfoOffset(); i < metaObject()->classInfoCount(); i++) {
        auto name = QString(metaObject()->classInfo(i).name());
        const auto value = QString(metaObject()->classInfo(i).value());

        if (!name.startsWith(QStringLiteral("help.")))
            continue;
        name = name.mid(5);

        mHelpTexts.insert(name, value);
    }
    qCDebug(GITKLIENT_LOG) << "Git Klient command line interface help:";
    for (int i = metaObject()->methodOffset(); i < c; i++) {
        auto method = metaObject()->method(i);
        qCDebug(GITKLIENT_LOG).noquote() << "    " << method.name() << method.parameterNames().join(" ");
        qCDebug(GITKLIENT_LOG).noquote() << mHelpTexts.value(method.name());
    }
    return 0;
}

ArgParserReturn CommandArgsParser::clone(const QString &path)
{
    CloneDialog d;
    d.setLocalPath(path);

    if (d.exec() == QDialog::Accepted) {
        RunnerDialog r;

        auto cmd = d.command();
        r.run(cmd);
        r.exec();
        cmd->deleteLater();
    }
    return 0;
}

ArgParserReturn CommandArgsParser::init(const QString &path)
{
    InitDialog d(git);
    d.setPath(path);

    if (d.exec() == QDialog::Accepted) {
        QDir dir;
        if (!dir.mkpath(d.path())) {
            KMessageBox::error(nullptr, i18n("Unable to create path: %1", d.path()), i18n("Init repo"));
            return 1;
        }

        git->init(d.path());
        KMessageBox::information(nullptr, i18n("The repo inited successfully"));
    }
    return 0;
}

ArgParserReturn CommandArgsParser::pull(const QString &path)
{
    git->setPath(path);
    PullDialog d;
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog r;
        const auto branch = git->currentBranch();
        r.run({QStringLiteral("pull"), QStringLiteral("origin"), branch});
        r.exec();
    }
    return 0;
}

ArgParserReturn CommandArgsParser::push(const QString &path)
{
    QFileInfo fi(path);

    if (!fi.exists()) {
        return 0;
    }
    if (fi.isFile())
        git->setPath(fi.absolutePath());
    else
        git->setPath(fi.absoluteFilePath());
    CommitPushDialog d(git);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::merge(const QString &path)
{
    git->setPath(path);

    if (!git->isValid())
        return Errors::InvalidPath;

    MergeDialog d(git);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog r;

        auto cmd = d.command();
        r.run(cmd);
        r.exec();
        cmd->deleteLater();
    }
    return 0;
}

ArgParserReturn CommandArgsParser::changes()
{
    QDir dir;
    git->setPath(dir.currentPath());
    ChangedFilesDialog d(git);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::changes(const QString &path)
{
    QFileInfo fi(path);

    git->setPath(fi.isFile() ? fi.absoluteFilePath() : fi.absolutePath());
    ChangedFilesDialog d(git);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::create_tag(const QString &path)
{
    checkGitPath(path);

    TagInfoDialog d(nullptr);

    if (d.exec() == QDialog::Accepted) {
        git->createTag(d.tagName(), d.message());
    }
    return 0;
}

ArgParserReturn CommandArgsParser::diff()
{
    auto d = new DiffWindow();
    d->exec();
    return ExecApp;
}

ArgParserReturn CommandArgsParser::diff(const QString &file)
{
    QFileInfo fi(file);

    if (fi.isFile()) {
        git->setPath(fi.absolutePath());
        const QDir dir(git->path());
        const Git::File headFile(file);
        const Git::File changedFile(git->currentBranch(), dir.relativeFilePath(file), git);
        auto d = new DiffWindow(headFile, changedFile);
        d->exec();
        return ExecApp;
    } else if (fi.isDir()) {
        git->setPath(fi.absoluteFilePath());
        auto d = new DiffWindow(git);
        d->exec();
        return ExecApp;
    }
    return 0;
}

ArgParserReturn CommandArgsParser::diff(const QString &file1, const QString &file2)
{
    qCDebug(GITKLIENT_LOG) << file1 << file2;
    QFileInfo fi1(file1);
    QFileInfo fi2(file2);

    if (fi1.isFile() && fi2.isFile()) {
        qCDebug(GITKLIENT_LOG) << fi1.absoluteFilePath() << fi2.absoluteFilePath();
        const Git::File fileLeft(fi1.absoluteFilePath());
        const Git::File fileRight(fi2.absoluteFilePath());
        auto d = new DiffWindow(fileLeft, fileRight);
        d->exec();
        return ExecApp;
    }
    if (fi1.isDir() && fi2.isDir()) {
        auto d = new DiffWindow(fi1.absoluteFilePath(), fi2.absoluteFilePath());
        d->exec();
        return ExecApp;
    }

    return 0;
}

ArgParserReturn CommandArgsParser::diff(const QString &path, const QString &file1, const QString &file2)
{
    if (file1.count(QLatin1Char(':')) != 1 || file2.count(QLatin1Char(':')) != 1)
        return 1;
    git->setPath(path);
    if (!git->isValid())
        return 1;
    const auto parts1 = file1.split(QLatin1Char(':'));
    const auto parts2 = file2.split(QLatin1Char(':'));
    const Git::File fileLeft(parts1.first(), parts1.at(1));
    const Git::File fileRight(parts2.first(), parts2.at(1));
    auto d = new DiffWindow(fileLeft, fileRight);
    d->exec();
    return ExecApp;
}

ArgParserReturn CommandArgsParser::blame(const QString &file)
{
    QFileInfo fi{file};

    if (!fi.exists() || !fi.isFile()) {
        return 0;
    }

    git->setLoadFlags(Git::LoadLogs);
    git->setPath(fi.absolutePath());
    //    git->logsModel()->load();

    const Git::File f(git->currentBranch(), file, git);
    FileBlameDialog d(f);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::history(const QString &file)
{
    git->setPath(file.mid(0, file.lastIndexOf(QLatin1Char('/'))));
    auto fileCopy = file;
    fileCopy = file.mid(git->path().size() + 1);
    FileHistoryDialog d(git, fileCopy);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::merge()
{
    auto d = new MergeWindow;
    d->exec();
    return ExecApp;
}

ArgParserReturn CommandArgsParser::merge(const QString &base, const QString &local, const QString &remote, const QString &result)
{
    auto d = new MergeWindow;
    d->setFilePathLocal(local);
    d->setFilePathBase(base);
    d->setFilePathRemote(remote);
    d->setFilePathResult(result);
    d->load();
    int n = d->exec();

    if (n == QDialog::Accepted)
        return 0;
    else
        return 1;
}

ArgParserReturn CommandArgsParser::ignore(const QString &path)
{
    QFileInfo fi(path);
    if (!fi.exists())
        return 1;

    if (fi.isDir())
        git->setPath(path);
    else
        git->setPath(fi.absolutePath());

    if (!git->isValid())
        return 1;

    IgnoreFileDialog d(git, path);
    d.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::cleanup(const QString &path)
{
    checkGitPath(path);

    CleanupDialog d;
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog runner;
        runner.run(d.command());
        runner.exec();
    }
    return 0;
}

ArgParserReturn CommandArgsParser::switch_checkout(const QString &path)
{
    checkGitPath(path);

    if (git->isMerging()) {
        KMessageBox::error(nullptr, i18n("Cannot switch branch while merging"), i18n("Switch branch"));
        return 1;
    }
    SwitchBranchDialog d(git);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog runner;
        runner.run(d.command());
        runner.exec();
    }
    return 0;
}

ArgParserReturn CommandArgsParser::merge_rebase(const QString &path)
{
    checkGitPath(path);

    MergeRebaseWizard w(git);
    w.exec();
    return 0;
}

ArgParserReturn CommandArgsParser::add(const QString &path)
{
    checkGitPath(path);

    git->addFile(path);
    KMessageBox::information(nullptr, i18n("File(s) added to git successfully"));
    return 0;
}

ArgParserReturn CommandArgsParser::remove(const QString &path)
{
    checkGitPath(path);

#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    auto r =
        KMessageBox::questionTwoActions(nullptr, i18n("Would you like to leave file(s) on disk?"), {}, KGuiItem(i18n("Leave")), KStandardGuiItem::cancel());
    bool cached = r == KMessageBox::PrimaryAction;
#else
    auto r = KMessageBox::questionYesNo(nullptr, i18n("Would you like to leave file(s) on disk?"));
    bool cached = r == KMessageBox::Yes;
#endif

    git->removeFile(path, cached);
    KMessageBox::information(nullptr, i18n("File(s) removed from git successfully"));
    return 0;
}

ArgParserReturn CommandArgsParser::main()
{
    auto window = AppWindow::instance();
    window->show();
    return ExecApp;
}

ArgParserReturn CommandArgsParser::main(const QString &path)
{
    auto window = new AppWindow(path);
    window->show();
    return ExecApp;
}
