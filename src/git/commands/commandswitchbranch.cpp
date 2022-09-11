/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandswitchbranch.h"

#include <QDebug>

#include <klocalizedstring.h>

namespace Git {

CommandSwitchBranch::CommandSwitchBranch(Manager *git) : AbstractCommand(git)
{

}

void CommandSwitchBranch::parseOutput(const QByteArray &output, const QByteArray &errorOutput)
{
    if (errorOutput.contains("error: Your local changes to the following files would be overwritten by checkout")) {
        setStatus(Error);
        setErrorMessage(
            i18n("Your local changes to the following files would be overwritten by checkout. "
                 "Please commit your changes or stash them before you switch branches."));
    }
    qDebug() << "Error" << errorOutput;
    qDebug() << "out" << output;
}

const QString &CommandSwitchBranch::target() const
{
    return _target;
}

void CommandSwitchBranch::setTarget(const QString &newTarget)
{
    _target = newTarget;
}

CommandSwitchBranch::Mode CommandSwitchBranch::mode() const
{
    return _mode;
}

void CommandSwitchBranch::setMode(Mode newMode)
{
    _mode = newMode;
}

QStringList CommandSwitchBranch::generateArgs() const
{
    QStringList cmd;

    if (_mode == NewBranch)
        cmd << QStringLiteral("checkout") << QStringLiteral("-b") << _target;
    else
        cmd << QStringLiteral("switch") << _target;

    if (_force)
        cmd.append(QStringLiteral("--force"));
    return cmd;
}

bool CommandSwitchBranch::force() const
{
    return _force;
}

void CommandSwitchBranch::setForce(bool newForce)
{
    _force = newForce;
}

} // namespace Git
