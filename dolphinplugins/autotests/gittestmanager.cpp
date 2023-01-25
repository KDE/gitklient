/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gittestmanager.h"

#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QUuid>

GitTestManager::GitTestManager()
    : mPath{QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QLatin1Char('/') + QUuid::createUuid().toString(QUuid::Id128) + "/"}
{
    QDir d;
    d.mkpath(mPath);
}

bool GitTestManager::touch(const QString &fileName)
{
    QFile f(mPath + QLatin1Char('/') + fileName);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    f.write(QUuid::createUuid().toString(QUuid::Id128).toLatin1());
    f.close();
    return true;
}

bool GitTestManager::run(const QString &cmd)
{
    auto parts = cmd.split(QLatin1Char(' '));
    Q_UNUSED(parts)
    return true;
}

void GitTestManager::init()
{
    runGit({QStringLiteral("init")});
}

void GitTestManager::add(const QString &file)
{
    runGit({QStringLiteral("add"), file});
}

void GitTestManager::remove(const QString &file, bool cached)
{
    QStringList args;
    args << QStringLiteral("rm");
    if (cached)
        args << QStringLiteral("--cached");
    args << file;
    runGit(args);
}

void GitTestManager::commit(const QString &message)
{
    runGit({QStringLiteral("commit"), QStringLiteral("-m"), message});
}

void GitTestManager::addToIgnore(const QString &pattern)
{
    QFile f(mPath + "/.gitignore");
    if (!f.open(QIODevice::Text | QIODevice::Append))
        return;

    f.write(pattern.toUtf8() + "\n");
    f.close();
}

QString GitTestManager::absoluteFilePath(const QString &file)
{
    return mPath + file;
}

QString GitTestManager::runGit(const QStringList &args)
{
    QProcess p;
    p.setProgram(QStringLiteral("git"));
    p.setArguments(args);
    p.setWorkingDirectory(mPath);
    p.start();
    p.waitForFinished();
    auto out = p.readAllStandardOutput();
    // auto err = p.readAllStandardError();
    return out; // + err;
}

namespace GitKlientTest
{
bool touch(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    f.write(QUuid::createUuid().toString(QUuid::Id128).toLatin1());
    f.close();
    return true;
}

QString getTempPath()
{
    auto path = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QLatin1Char('/') + QUuid::createUuid().toString(QUuid::Id128);
    QDir d;
    d.mkpath(path);
    return path;
}
} // namespace GitKlientTest
