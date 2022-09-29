/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "appmainwindow.h"
#include <QDebug>

#include <QCloseEvent>
#include <QEventLoop>

AppMainWindow::AppMainWindow(QWidget *parent, Qt::WindowFlags f)
    : KXmlGuiWindow(parent, f)
{
}

int AppMainWindow::exec()
{
    QEventLoop eventLoop;
    mLoop = &eventLoop;
    showModal();
    (void)eventLoop.exec(QEventLoop::DialogExec);
    mLoop = nullptr;
    qDebug() << "returnCode=" << mReturnCode;
    return mReturnCode;
}

void AppMainWindow::showModal()
{
    mIsModal = true;
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose, true);
    show();
}

void AppMainWindow::accept()
{
    mReturnCode = Accepted;
}

void AppMainWindow::reject()
{
    mReturnCode = Rejected;
}

void AppMainWindow::setVisible(bool visible)
{
    if (!visible && mLoop)
        mLoop->exit();
    KXmlGuiWindow::setVisible(visible);
}

// void AppMainWindow::closeDialog(int resultCode)
//{
//     _returnCode = resultCode;
//     if (_loop && _loop->isRunning())
//         _loop->quit();
// }

// void AppMainWindow::closeEvent(QCloseEvent *event)
//{
//     qDebug() << Q_FUNC_INFO;
//     Q_UNUSED(event)
//     if (_loop && _loop->isRunning())
//         _loop->quit();
//     KXmlGuiWindow::closeEvent(event);
// }

void AppMainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && mIsModal)
        close();

    KXmlGuiWindow::keyPressEvent(event);
}
