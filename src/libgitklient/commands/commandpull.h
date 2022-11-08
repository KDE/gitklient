/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractcommand.h"
#include "libgitklient_export.h"

#ifdef GIT_GUI
namespace Ui
{
class CommandPullWidget;
}
#endif

namespace Git
{

class LIBGITKLIENT_EXPORT CommandPull : public AbstractCommand
{
    Q_OBJECT

public:
    enum Rebase { None, False, True, Preserve, Merge };
    Q_ENUM(Rebase)

    enum FastForward { Unset, Yes, No, OnlyFastForward };
    Q_ENUM(FastForward)

    CommandPull();
    ~CommandPull() override;
    QStringList generateArgs() const override;

    bool squash() const;
    void setSquash(bool newSquash);

    bool noCommit() const;
    void setNoCommit(bool newNoCommit);

    bool prune() const;
    void setPrune(bool newPrune);

    bool tags() const;
    void setTags(bool newTags);

    void parseOutput(const QByteArray &output, const QByteArray &errorOutput) override;
    bool supportWidget() const override;
    QWidget *createWidget() override;

    const QString &remote() const;
    void setRemote(const QString &newRemote);

    const QString &branch() const;
    void setBranch(const QString &newBranch);

    Rebase rebase() const;
    void setRebase(Rebase newRebase);

    FastForward fastForward() const;
    void setFastForward(FastForward newFastForward);

private:
    bool mSquash{false};
    bool mNoCommit{false};
    bool mPrune{false};
    bool mTags{false};
    QString mRemote;
    QString mBranch;
    Rebase mRebase{None};
    FastForward mFastForward{Unset};


#ifdef GIT_GUI
    QWidget *mWidget;
    Ui::CommandPullWidget *mUi;
#endif
};

} // namespace Git