/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "settingsmanager.h"

#include "GitKlientSettings.h"
#include "appwindow.h"
#include "git/gitmanager.h"
#include "gitklient_appdebug.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#include <QCalendar>
#endif

#include <KConfigDialog>

#include <QWidget>

SettingsManager::SettingsManager() = default;

SettingsManager *SettingsManager::instance()
{
    static auto *instance = new SettingsManager;
    return instance;
}

void SettingsManager::settingsChanged()
{
    qCDebug(GITKLIENT_LOG) << GitKlientSettings::calendarType() << pageBase.kcfg_calendarType->currentText();
    GitKlientSettings::setCalendarType(pageBase.kcfg_calendarType->currentText());

    auto git = Git::Manager::instance();

    if (GitKlientSettings::registerDiffTool()) {
        git->setConfig(QStringLiteral("difftool.gitklientdiff.cmd"), QStringLiteral("gitklientdiff \"$LOCAL\" \"$REMOTE\""), Git::Manager::ConfigGlobal);
    } else {
        git->unsetConfig(QStringLiteral("difftool.gitklientdiff.cmd"), Git::Manager::ConfigGlobal);
    }
    if (GitKlientSettings::registerMergeTool()) {
        git->setConfig(QStringLiteral("mergetool.gitklientmerge.cmd"),
                       QStringLiteral("gitklientmerge \"$BASE\" \"$LOCAL\" \"$REMOTE\" \"$MERGED\""),
                       Git::Manager::ConfigGlobal);
        git->setConfig(QStringLiteral("mergetool.gitklientmerge.trustExitCode"), QStringLiteral("true"), Git::Manager::ConfigGlobal);
    } else {
        git->unsetConfig(QStringLiteral("mergetool.gitklientmerge.cmd"), Git::Manager::ConfigGlobal);
        git->unsetConfig(QStringLiteral("mergetool.gitklientmerge.trustExitCode"), Git::Manager::ConfigGlobal);
    }
}

void SettingsManager::show()
{
    exec(AppWindow::instance());
}

QWidget *SettingsManager::createBasePage()
{
    auto w = new QWidget;
    pageBase.setupUi(w);

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    pageBase.kcfg_calendarType->addItems(QCalendar::availableCalendars());
    pageBase.kcfg_calendarType->setCurrentText(GitKlientSettings::calendarType());
#else
    pageBase.labelDefaultCalendar->hide();
    pageBase.kcfg_calendarType->hide();
#endif
    return w;
}
QWidget *SettingsManager::createDiffPage()
{
    auto w = new QWidget;
    pageDiff.setupUi(w);
    return w;
}
QWidget *SettingsManager::createGitPage()
{
    auto w = new QWidget;
    pageGit.setupUi(w);
    return w;
}

void SettingsManager::exec(QWidget *parentWidget)
{
    auto name = QStringLiteral("settings");
    auto dialog = KConfigDialog::exists(name);

    if (dialog) {
        dialog->setParent(parentWidget);
        dialog->exec();
        return;
    }
    //    if (KConfigDialog::showDialog(QStringLiteral("settings"))) {
    //        return;
    //    }

    dialog = new KConfigDialog(parentWidget, name, GitKlientSettings::self());

    dialog->addPage(createBasePage(), i18n("General"), QStringLiteral("package_setting"));
    dialog->addPage(createDiffPage(), i18n("Diff"), QStringLiteral("package_setting"));
    dialog->addPage(createGitPage(), i18n("Git"), QStringLiteral("package_setting"));

    connect(dialog, &KConfigDialog::settingsChanged, this, &SettingsManager::settingsChanged);
    dialog->show();
}
