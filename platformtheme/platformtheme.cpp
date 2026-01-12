#include "platformtheme.h"
#include "x11integration.h"

#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QLibrary>
#include <QStyleFactory>
#include <QIcon>
#include <QWindow>

// Qt DBus (optional)
#ifdef QT_DBUS_LIB
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#endif

#ifdef HAVE_KWINDOWSYSTEM
#include <KWindowSystem>
#endif

static const QByteArray s_x11AppMenuServiceNamePropertyName = QByteArrayLiteral("_KDE_NET_WM_APPMENU_SERVICE_NAME");
static const QByteArray s_x11AppMenuObjectPathPropertyName = QByteArrayLiteral("_KDE_NET_WM_APPMENU_OBJECT_PATH");


void onDarkModeChanged()
{
    if (qApp->applicationName() == "systemsettings"
                || qApp->applicationName().contains("plasma")
                || qApp->applicationName().contains("QtCreator")) {
        return;
    }

    QStyle *style = QStyleFactory::create("cutefish");
    if (style) {
        qApp->setStyle(style);
    }
}

PlatformTheme::PlatformTheme()
    : m_hints(new HintsSettings)
{
    // qApp->setProperty("_hints_settings_object", (quintptr)m_hints);

#ifdef HAVE_KWINDOWSYSTEM
    if (KWindowSystem::isPlatformX11()) {
        m_x11Integration.reset(new X11Integration());
        m_x11Integration->init();
    }
#else
    Q_UNUSED(m_x11Integration);
#endif

    connect(m_hints, &HintsSettings::systemFontChanged, this, &PlatformTheme::onFontChanged);
    connect(m_hints, &HintsSettings::systemFontPointSizeChanged, this, &PlatformTheme::onFontChanged);
    connect(m_hints, &HintsSettings::iconThemeChanged, this, &PlatformTheme::onIconThemeChanged);
    connect(m_hints, &HintsSettings::darkModeChanged, &onDarkModeChanged);

    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, false);
    // setQtQuickControlsTheme(); // Platform theme should not set Qt Quick Controls style
}

PlatformTheme::~PlatformTheme()
{
}

QVariant PlatformTheme::themeHint(QPlatformTheme::ThemeHint hintType) const
{
    QVariant hint = m_hints->hint(hintType);
    if (hint.isValid()) {
        return hint;
    } else {
        return QPlatformTheme::themeHint(hintType);
    }
}

const QFont* PlatformTheme::font(Font type) const
{
    switch (type) {
        case SystemFont:
        case MessageBoxFont:
        case LabelFont:
        case TipLabelFont:
        case StatusBarFont:
        case PushButtonFont:
        case ItemViewFont:
        case ListViewFont:
        case HeaderViewFont:
        case ListBoxFont:
        case ComboMenuItemFont:
        case ComboLineEditFont: {
            const QString &fontName = m_hints->systemFont();
            qreal fontSize = m_hints->systemFontPointSize();
            static QFont font = QFont(QString());
            font.setFamily(fontName);
            font.setPointSizeF(fontSize);
            return &font;
        }
        case FixedFont: {
            const QString &fontName = m_hints->systemFixedFont();
            qreal fontSize = m_hints->systemFontPointSize();
            static QFont font = QFont(QString());
            font.setFamily(fontName);
            font.setPointSizeF(fontSize);
            return &font;
        }
        default: {
            const QString &fontName = m_hints->systemFont();
            qreal fontSize = m_hints->systemFontPointSize();
            static QFont font = QFont(QString());
            font.setFamily(fontName);
            font.setPointSizeF(fontSize);
            return &font;
        }
    }

    return QPlatformTheme::font(type);
}

QPlatformMenuBar *PlatformTheme::createPlatformMenuBar() const
{
    // Qt6 不再提供 DBusMenu/全局菜单功能
    return nullptr;
}

void PlatformTheme::onFontChanged()
{
    QFont font;
    font.setFamily(m_hints->systemFont());
    font.setPointSizeF(m_hints->systemFontPointSize());

    // Change font
    if (qobject_cast<QApplication *>(QCoreApplication::instance()))
        QApplication::setFont(font);
    else if (qobject_cast<QGuiApplication *>(QCoreApplication::instance()))
        QGuiApplication::setFont(font);
}

void PlatformTheme::onIconThemeChanged()
{
    // 获取当前图标主题
    QVariant iconThemeVariant = m_hints->hint(QPlatformTheme::SystemIconThemeName);
    QString iconTheme = iconThemeVariant.isValid() ? iconThemeVariant.toString() : "Crule";
    
    // 在Qt6中，需要显式设置图标主题
    QIcon::setThemeName(iconTheme);
    
    // 同时设置后备主题
    QIcon::setFallbackThemeName("hicolor");
    
    // 触发更新事件
    QEvent update(QEvent::UpdateRequest);
    for (QWindow *window : qGuiApp->allWindows()) {
        if (window->type() == Qt::Desktop)
            continue;

        qApp->sendEvent(window, &update);
    }
}

// void PlatformTheme::setQtQuickControlsTheme()
// {
//     //if the user has explicitly set something else, don't meddle
//     if (!QQuickStyle::name().isEmpty()) {
//         return;
//     }
//
//     if (qApp->applicationName() == "systemsettings"
//             || qApp->applicationName().contains("plasma")) {
//         QQuickStyle::setStyle("Plasma");
//         QStyle *style = QStyleFactory::create("Breeze");
//         qApp->setStyle(style);
//         return;
//     }
//
//     QQuickStyle::setStyle(QLatin1String("fish-style"));
// }
