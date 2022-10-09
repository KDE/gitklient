/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QStringList>

namespace Diff
{

enum class LineEnding { None, Cr, Lf, CrLf };

enum class DiffType { Unchanged, Added, Removed, Modified };

enum MergeType { None, KeepLocal, KeepRemote, KeepLocalThenRemote, KeepRemoteThenLocal, MergeCustom };

enum class SegmentType {
    SameOnBoth,
    OnlyOnLeft,
    OnlyOnRight,
    DifferentOnBoth,
};

enum MergeDiffType { Unchanged, LocalAdd, RemoteAdd, BothChanged };

struct Text {
    QList<QString> lines; // TODO: convert to QList<QstringRef> if it's possible
    LineEnding lineEnding;
};

Text readLines(const QString &text);

struct Segment {
    virtual ~Segment() = default;

    QStringList oldText;
    QStringList newText;
    SegmentType type;

    virtual QStringList get(int index)
    {
        switch (index) {
        case 0:
            return oldText;
        case 1:
            return newText;
        }
        return {};
    }
};

struct DiffSegment : Segment {
    ~DiffSegment() override = default;

    QStringList get(int index) override
    {
        switch (index) {
        case 0:
            return oldText;
        case 1:
            return newText;
        }
        return {};
    }
};

struct MergeSegment : Segment {
    ~MergeSegment() override = default;
    QStringList base;
    QStringList local;
    QStringList remote;
    //    MergeDiffType type;
    MergeType mergeType{None};

    QStringList get(int index) override
    {
        switch (index) {
        case 0:
            return base;
        case 1:
            return local;
        case 2:
            return remote;
        }
        return {};
    }
    MergeSegment();
    MergeSegment(const QStringList &base, const QStringList &local, const QStringList &remote);
};

struct Diff2Result {
    LineEnding oldTextLineEnding;
    LineEnding newTextLineEnding;
    QList<Segment *> segments;
};

struct Diff3Result {
    LineEnding baseTextLineEnding;
    LineEnding localTextLineEnding;
    LineEnding remoteTextLineEnding;
    QList<MergeSegment *> segments;
};

int matchesCount(const QStringList &base, const QStringList &local, const QStringList &remote);
QStringList take(QStringList &list, int count);
int remove(QStringList &list, int count);

QList<Segment *> diff(const QString &oldText, const QString &newText);
QList<Segment *> diff(const QStringList &oldText, const QStringList &newText);

Diff2Result diff2(const QString &oldText, const QString &newText);

QList<MergeSegment *> diff3(const QString &base, const QString &local, const QString &remote);
QList<MergeSegment *> diff3(const QStringList &base, const QStringList &local, const QStringList &remote);

QMap<QString, DiffType> diffDirs(const QString &dir1, const QString &dir2);

} // namespace Diff