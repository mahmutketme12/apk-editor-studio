#include "base/actionprovider.h"
#include "base/application.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include "windows/keymanager.h"
#include "windows/optionsdialog.h"
#include "tools/adb.h"
#include <QMenu>
#include <QDesktopServices>
#include <QDateTime>

bool ActionProvider::openApk(QWidget *parent)
{
    return openApk(QString(), parent);
}

bool ActionProvider::openApk(const QString &defaultPath, QWidget *parent)
{
    const QStringList paths = Dialogs::getOpenApkFilenames(defaultPath, parent);
    if (paths.isEmpty()) {
        return false;
    }
    for (const QString &path : paths) {
        app->openApk(path);
    }
    return true;
}

bool ActionProvider::closeApk(Project *project)
{
    return app->projects.close(project);
}

void ActionProvider::visitWebPage()
{
    QDesktopServices::openUrl(app->getWebPage());
}

void ActionProvider::visitSourcePage()
{
    QDesktopServices::openUrl(app->getSourcePage());
}

void ActionProvider::visitDonatePage()
{
    QDesktopServices::openUrl(app->getDonatePage());
}

void ActionProvider::visitUpdatePage()
{
    QDesktopServices::openUrl(app->getUpdatePage());
}

void ActionProvider::visitBlogPage(const QString &post)
{
    const QString url = app->getWebPage() + "blog/" + post + "/";
    QDesktopServices::openUrl(url);
}

void ActionProvider::exit()
{
    app->window->close();
}

void ActionProvider::checkUpdates(QWidget *parent)
{
    Updater::check(true, parent);
}

bool ActionProvider::resetSettings(QWidget *parent)
{
    const QString question(tr("Are you sure you want to reset settings?"));
    const int answer = QMessageBox::question(parent, {}, question);
    if (answer != QMessageBox::Yes) {
        return false;
    }
    app->settings->reset();
    return true;
}

bool ActionProvider::installExternalApk(QWidget *parent)
{
    const QStringList paths = Dialogs::getOpenApkFilenames(parent);
    if (paths.isEmpty()) {
        return false;
    }
    const auto device = Dialogs::getInstallTargetDevice(parent);
    if (!device) {
        return false;
    }
    for (const QString &path : paths) {
        Project *project = app->openApk(path, false);
        project->install(device->getSerial());
    }
    return true;
}

QAction *ActionProvider::getOpenApk(QWidget *parent)
{
    auto action = new QAction(app->icons.get("open.png"), {}, parent);
    action->setShortcut(QKeySequence::Open);

    auto translate = [=]() { action->setText(tr("&Open APK...")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        openApk(parent);
    });
    return action;
}

QAction *ActionProvider::getVisitWebPage(QObject *parent)
{
    auto action = new QAction(app->icons.get("website.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("Visit &Website")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    connect(action, &QAction::triggered, this, &ActionProvider::visitWebPage);
    return action;
}

QAction *ActionProvider::getVisitSourcePage(QObject *parent)
{
    auto action = new QAction(app->icons.get("github.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Source Code")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    connect(action, &QAction::triggered, this, &ActionProvider::visitSourcePage);
    return action;
}

QAction *ActionProvider::getVisitDonatePage(QObject *parent)
{
    auto action = new QAction(app->icons.get("donate.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("Make a &Donation")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    connect(action, &QAction::triggered, this, &ActionProvider::visitDonatePage);
    return action;
}

QAction *ActionProvider::getExit(QObject *parent)
{
    auto action = new QAction(app->icons.get("close.png"), {}, parent);
    action->setShortcut(QKeySequence::Quit);
    action->setMenuRole(QAction::QuitRole);

    auto translate = [=]() { action->setText(tr("E&xit")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    connect(action, &QAction::triggered, this, &ActionProvider::exit);
    return action;
}

QAction *ActionProvider::getCheckUpdates(QWidget *parent)
{
    auto action = new QAction(app->icons.get("update.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("Check for &Updates")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        checkUpdates(parent);
    });

    return action;
}

QAction *ActionProvider::getResetSettings(QWidget *parent)
{
    auto action = new QAction(app->icons.get("close.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Reset Settings...")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        resetSettings(parent);
    });

    return action;
}

QAction *ActionProvider::getInstallExternalApk(QWidget *parent)
{
    auto action = new QAction(app->icons.get("install.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("Install &External APK...")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    action->setShortcut(QKeySequence("Ctrl+Shift+I"));
    connect(action, &QAction::triggered, [=]() {
        installExternalApk(parent);
    });

    return action;
}

QAction *ActionProvider::getOpenOptions(QWidget *parent)
{
    auto action = new QAction(app->icons.get("settings.png"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+P"));
    action->setMenuRole(QAction::PreferencesRole);

    auto translate = [=]() { action->setText(tr("&Options...")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        OptionsDialog settings(parent);
        settings.exec();
    });

    return action;
}

QAction *ActionProvider::getOpenDeviceManager(QWidget *parent)
{
    auto action = new QAction(app->icons.get("devices.png"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+D"));

    //: This string refers to multiple devices (as in "Manager of devices").
    auto translate = [=]() { action->setText(tr("&Device Manager...")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        DeviceManager deviceManager(parent);
        deviceManager.exec();
    });

    return action;
}

QAction *ActionProvider::getOpenKeyManager(QWidget *parent)
{
    auto action = new QAction(app->icons.get("key.png"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+K"));

    //: This string refers to multiple keys (as in "Manager of keys").
    auto translate = [=]() { action->setText(tr("&Key Manager...")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        KeyManager keyManager(parent);
        keyManager.exec();
    });

    return action;
}

QMenu *ActionProvider::getLanguages(QWidget *parent)
{
    auto menu = new QMenu(parent);
    auto actions = new QActionGroup(parent);
    actions->setExclusive(true);

    auto translate = [=]() {
        // Set title
        menu->setTitle(tr("&Language"));
        // Set icon
        const QString currentLocale = app->settings->getLanguage();
        QIcon flag(app->getLocaleFlag(currentLocale));
        menu->setIcon(flag);
        // Set check mark
        for (QAction *action : actions->actions()) {
            if (action->property("locale") == currentLocale) {
                action->setChecked(true);
                break;
            }
        }
    };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    QList<Language> languages = app->getLanguages();
    for (const Language &language : languages) {
        const QString localeTitle = language.getTitle();
        const QString localeCode = language.getCode();
        const QPixmap localeFlag = language.getFlag();
        QAction *action = actions->addAction(localeFlag, localeTitle);
        action->setCheckable(true);
        action->setProperty("locale", localeCode);
        connect(action, &QAction::triggered, [=]() {
            app->setLanguage(localeCode);
        });
    }
    menu->addActions(actions->actions());

    return menu;
}

QMenu *ActionProvider::getRecent(QWidget *parent)
{
    auto menuRecent = new QMenu(parent);
    menuRecent->setIcon(app->icons.get("recent.png"));

    auto translate = [=]() { menuRecent->setTitle(tr("Open &Recent")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    auto initialize = [=]() {
        menuRecent->clear();
        auto recentList = app->recent->all();
        for (const RecentFile &recentEntry : recentList) {
            QAction *action = new QAction(recentEntry.thumbnail(), recentEntry.filename(), parent);
            menuRecent->addAction(action);
            connect(action, &QAction::triggered, [=]() {
                app->openApk(recentEntry.filename());
            });
        }
        menuRecent->addSeparator();
        menuRecent->addAction(recentList.isEmpty() ? getNoRecent(parent) : getClearRecent(parent));
    };
    connect(app->recent, &Recent::changed, initialize);
    initialize();

    return menuRecent;
}

QAction *ActionProvider::getClearRecent(QObject *parent)
{
    auto action = new QAction(app->icons.get("close.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Clear List")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    connect(action, &QAction::triggered, app->recent, &Recent::clear);
    return action;
}

QAction *ActionProvider::getNoRecent(QObject *parent)
{
    auto action = new QAction(parent);
    action->setEnabled(false);

    auto translate = [=]() { action->setText(tr("No Recent Files")); };
    connect(this, &ActionProvider::languageChanged, translate);
    translate();

    return action;
}

bool ActionProvider::event(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        emit languageChanged();
        return true;
    }
    return QObject::event(event);
}
