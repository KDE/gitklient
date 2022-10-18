/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef COMMANDREBASE_H
#define COMMANDREBASE_H

#include "abstractcommand.h"

namespace Git
{

class LIBGITKLIENT_EXPORT CommandRebase : public AbstractCommand
{
    Q_OBJECT

public:
    explicit CommandRebase(QObject *parent = nullptr);

    QStringList generateArgs() const override;

    const QString &fromBranch() const;
    void setFromBranch(const QString &newFromBranch);

    const QString &toBranch() const;
    void setToBranch(const QString &newToBranch);

private:
    QString mFromBranch;
    QString mToBranch;
};

}

#endif // COMMANDREBASE_H
