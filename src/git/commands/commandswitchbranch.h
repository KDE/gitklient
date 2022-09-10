/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"

namespace Git {

class CommandSwitchBranch : public AbstractCommand
{
    Q_OBJECT

public:
    explicit CommandSwitchBranch(Manager *git);
    void parseOutput(const QByteArray &output, const QByteArray &errorOutput) override;

    enum Mode { Unknown, NewBranch, ExistingBranch, Tag };

    const QString &target() const;
    void setTarget(const QString &newTarget);

    Mode mode() const;
    void setMode(Mode newMode);

    QStringList generateArgs() const override;

    bool force() const;
    void setForce(bool newForce);

private:
    Mode _mode{Unknown};
    QString _target;
    bool _force{false};

};

} // namespace Git

