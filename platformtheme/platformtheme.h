#ifndef PLATFORMTHEME_H
#define PLATFORMTHEME_H

// Qt6 兼容的头文件包含
#include <QtGui/qpa/qplatformtheme.h>
#include "hintsettings.h"
#include "systemtrayicon.h"

#include <QHash>
#include <QKeySequence>
#include <QObject>
#include <QScopedPointer>

class QIconEngine;
class QWindow;
class X11Integration;

class PlatformTheme : public QObject, public QPlatformTheme
{
    Q_OBJECT

public:
    PlatformTheme();
    ~PlatformTheme() override;

    QVariant themeHint(ThemeHint hint) const override;
    const QFont *font(Font type) const override;

    QPlatformMenuBar *createPlatformMenuBar() const override;

    QPlatformSystemTrayIcon *createPlatformSystemTrayIcon() const override {
        auto trayIcon = new SystemTrayIcon;
        if (trayIcon->isSystemTrayAvailable())
            return trayIcon;
        else {
            delete trayIcon;
            return nullptr;
        }
    }

private:
    void onFontChanged();
    void onIconThemeChanged();

private:
    HintsSettings *m_hints;
    QScopedPointer<X11Integration> m_x11Integration;
};

#endif // PLATFORMTHEME_H
