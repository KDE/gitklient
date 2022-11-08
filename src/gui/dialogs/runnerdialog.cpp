/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "runnerdialog.h"

#include "gitmanager.h"

#include <KLocalizedString>
#include <KMessageBox>

#include "gitklient_appdebug.h"

RunnerDialog::RunnerDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(parent)
    , mGitProcess(new QProcess{this})
    , mGit(git)
{
    setupUi(this);

    mGitProcess->setProgram(QStringLiteral("git"));
    mGitProcess->setWorkingDirectory(git->path());

    connect(mGitProcess, &QProcess::readyReadStandardOutput, this, &RunnerDialog::git_readyReadStandardOutput);
    connect(mGitProcess, &QProcess::readyReadStandardError, this, &RunnerDialog::git_readyReadStandardError);

    connect(mGitProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RunnerDialog::git_finished);
}

void RunnerDialog::run(const QStringList &args)
{
    progressBar->hide();
    mMode = RunByArgs;
    lineEditCommand->setText(QStringLiteral("git ") + args.join(QLatin1Char(' ')));
    textBrowser->append(lineEditCommand->text());

    textBrowser->append(QStringLiteral("$ ") + lineEditCommand->text());
    mGitProcess->setArguments(args);
    mGitProcess->start();
}

void RunnerDialog::run(Git::AbstractCommand *command)
{
    mMode = RunByCommand;
    if (command->supportWidget()) {
        auto w = command->createWidget();
        tabWidget->insertTab(0, w, i18n("View"));
        tabWidget->setCurrentIndex(0);
    }

    const auto args = command->generateArgs();
    lineEditCommand->setText(QStringLiteral("git ") + args.join(QLatin1Char(' ')));

    textBrowser->append(lineEditCommand->text());
    if (command->supportProgress()) {
        progressBar->show();
        connect(command, &Git::AbstractCommand::progressChanged, progressBar, &QProgressBar::setValue);
    } else {
        progressBar->hide();
    }
    mGitProcess->setArguments(args);
    mGitProcess->start();
    mCmd = command;

    mTimer.start();
}

void RunnerDialog::git_readyReadStandardOutput()
{
    const auto buffer = mGitProcess->readAllStandardOutput();
    mErrorOutput.append(buffer);
    qCDebug(GITKLIENT_LOG) << "OUT" << buffer;
    //    textBrowser->setTextColor(Qt::black);
    textBrowser->append(buffer);

    if (mCmd)
        mCmd->parseOutput(buffer, QByteArray());
}

void RunnerDialog::git_readyReadStandardError()
{
    const auto buffer = mGitProcess->readAllStandardError();
    mStandardOutput.append(buffer);
    qCDebug(GITKLIENT_LOG) << "ERROR" << buffer;
    //    textBrowser->setTextColor(Qt::red);
    textBrowser->append(buffer);

    if (mCmd)
        mCmd->parseOutput(QByteArray(), buffer);
}

void RunnerDialog::git_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    pushButton->setText(i18n("Close"));

    if (mCmd)
        mCmd->parseOutput(mStandardOutput,mErrorOutput);

    bool isSuccessful = mCmd?mCmd->status()==Git::AbstractCommand::Finished:exitStatus==QProcess::NormalExit;
    QString exitMessage;

    if (isSuccessful) {
        exitMessage=i18n("Process finished");
    } else {
        if (mCmd)
            KMessageBox::error(this, mCmd->errorMessage());
        else
        KMessageBox::error(this, i18n("The git process crashed"));
        exitMessage=i18n("Process finished with error");
    }

    textBrowser->append(
        QStringLiteral("%1: (Elapsed time: %2)").arg(exitMessage, QTime::fromMSecsSinceStartOfDay(mTimer.elapsed()).toString(QStringLiteral("HH:mm:ss"))));

    if (mAutoClose){
        if (isSuccessful)
        accept();
    else
        setResult(QDialog::Rejected);
    }
}

bool RunnerDialog::autoClose() const
{
    return mAutoClose;
}

void RunnerDialog::setAutoClose(bool newAutoClose)
{
    mAutoClose = newAutoClose;
}
