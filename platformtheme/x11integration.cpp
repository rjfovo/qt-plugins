#include "x11integration.h"

#include <QCoreApplication>
#include <QPlatformSurfaceEvent>
#include <QGuiApplication>
#include <QWindow>
#include <QWidget>
#include <QVariant>
#include <QRegion>
#include <QDebug>

#include <NETWM>
#include <KWindowEffects>

#include <xcb/xcb.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <qpa/qplatformnativeinterface.h>
#else
#include <QX11Info>
#endif

static const char s_schemePropertyName[] = "KDE_COLOR_SCHEME_PATH";
static const QByteArray s_blurBehindPropertyName = QByteArrayLiteral("ENABLE_BLUR_BEHIND_HINT");
static const QByteArray s_blurRegionPropertyName = QByteArrayLiteral("BLUR_REGION");

X11Integration::X11Integration()
    : QObject()
{
}

X11Integration::~X11Integration() = default;

void X11Integration::init()
{
    QCoreApplication::instance()->installEventFilter(this);
}

bool X11Integration::eventFilter(QObject *watched, QEvent *event)
{
    //the drag and drop window should NOT be a tooltip
    //https://bugreports.qt.io/browse/QTBUG-52560
    if (event->type() == QEvent::Show && watched->inherits("QShapedPixmapWindow")) {
        //static cast should be safe there
        QWindow *w = static_cast<QWindow *>(watched);
        
        // 修复：使用 QPlatformNativeInterface 来获取 X11 连接和根窗口
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto native = qApp->platformNativeInterface();
        auto *connection = static_cast<xcb_connection_t*>(native->nativeResourceForIntegration("connection"));
        auto rootWindow = static_cast<xcb_window_t>(reinterpret_cast<quintptr>(native->nativeResourceForScreen("rootwindow", qApp->primaryScreen())));
        NETWinInfo info(connection, w->winId(), rootWindow, NET::WMWindowType, NET::Properties2());
#else
        NETWinInfo info(QX11Info::connection(), w->winId(), QX11Info::appRootWindow(), NET::WMWindowType, NET::Properties2());
#endif
        info.setWindowType(NET::DNDIcon);
        // TODO: does this flash the xcb connection?
    }

    // if (event->type() == QEvent::PlatformSurface) {
    //     if (QWindow *w = qobject_cast<QWindow*>(watched)) {
    //         QPlatformSurfaceEvent *pe = static_cast<QPlatformSurfaceEvent*>(event);
    //         if (!w->flags().testFlag(Qt::ForeignWindow)) {
    //             if (pe->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
    //                 const auto blurBehindProperty = w->property(s_blurBehindPropertyName.constData());
    //                 if (blurBehindProperty.isValid()) {
    //                     KWindowEffects::enableBlurBehind(w->winId(), blurBehindProperty.toBool());
    //                 }
    //                 installDesktopFileName(w);
    //             }
    //         }
    //     }
    // }

    // if (event->type() == QEvent::ApplicationPaletteChange) {
    //     const auto topLevelWindows = QGuiApplication::topLevelWindows();
    //     for (QWindow *w : topLevelWindows) {
    //         installColorScheme(w);
    //     }
    // }

    return false;
}

void X11Integration::installDesktopFileName(QWindow *w)
{
    if (!w->isTopLevel()) {
        return;
    }

    QString desktopFileName = QGuiApplication::desktopFileName();
    if (desktopFileName.isEmpty()) {
        return;
    }
    // handle apps which set the desktopFileName property with filename suffix,
    // due to unclear API dox (https://bugreports.qt.io/browse/QTBUG-75521)
    if (desktopFileName.endsWith(QLatin1String(".desktop"))) {
        desktopFileName.chop(8);
    }
    
    // 修复：使用 QPlatformNativeInterface 来获取 X11 连接和根窗口
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto native = qApp->platformNativeInterface();
    auto *connection = static_cast<xcb_connection_t*>(native->nativeResourceForIntegration("connection"));
    auto rootWindow = static_cast<xcb_window_t>(reinterpret_cast<quintptr>(native->nativeResourceForScreen("rootwindow", qApp->primaryScreen())));
    NETWinInfo info(connection, w->winId(), rootWindow, NET::Properties(), NET::Properties2());
#else
    NETWinInfo info(QX11Info::connection(), w->winId(), QX11Info::appRootWindow(), NET::Properties(), NET::Properties2());
#endif
    info.setDesktopFileName(desktopFileName.toUtf8().constData());
}

void X11Integration::setWindowProperty(QWindow *window, const QByteArray &name, const QByteArray &value)
{
    // 修复：使用 QPlatformNativeInterface 来获取 X11 连接
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto native = qApp->platformNativeInterface();
    if (!native) {
        qWarning() << "X11Integration: QPlatformNativeInterface is null";
        return;
    }
    auto *c = static_cast<xcb_connection_t*>(native->nativeResourceForIntegration("connection"));
#else
    auto *c = QX11Info::connection();
#endif

    if (!c) {
        qWarning() << "X11Integration: XCB connection is null";
        return;
    }

    if (!window || !window->handle()) {
        qWarning() << "X11Integration: Window or window handle is null";
        return;
    }

    xcb_atom_t atom;
    auto it = m_atoms.find(name);
    if (it == m_atoms.end()) {
        const xcb_intern_atom_cookie_t cookie = xcb_intern_atom(c, false, name.length(), name.constData());
        QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> reply(xcb_intern_atom_reply(c, cookie, nullptr));
        if (!reply.isNull()) {
            atom = reply->atom;
            m_atoms[name] = atom;
        } else {
            qWarning() << "X11Integration: Failed to intern atom for" << name;
            return;
        }
    } else {
        atom = *it;
    }

    if (atom == XCB_ATOM_NONE) {
        qWarning() << "X11Integration: Invalid atom for" << name;
        return;
    }

    if (value.isEmpty()) {
        xcb_delete_property(c, window->winId(), atom);
    } else {
        xcb_change_property(c, XCB_PROP_MODE_REPLACE, window->winId(), atom, XCB_ATOM_STRING,
                            8, value.length(), value.constData());
    }
}
