#include "statusnotifieritem.h"
#include "statusnotifieritemadaptor.h"
#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QDBusConnection>
#include <QMenu>

int StatusNotifierItem::mServiceCounter = 0;

StatusNotifierItem::StatusNotifierItem(QString id, QObject *parent)
    : QObject(parent),
      mAdaptor(new StatusNotifierItemAdaptor(this)),
      mService(QString::fromLatin1("org.freedesktop.StatusNotifierItem-%1-%2")
               .arg(QCoreApplication::applicationPid())
               .arg(++mServiceCounter)),
      mId(id),
      mTitle(QLatin1String("Test")),
      mStatus(QLatin1String("Active")),
      mCategory(QLatin1String("ApplicationStatus")),
      mMenu(nullptr),
      mMenuPath(QLatin1String("/NO_DBUSMENU")),
      mSessionBus(QDBusConnection::connectToBus(QDBusConnection::SessionBus, mService))
{
    // 注册对象到 SessionBus
    mSessionBus.registerObject(QLatin1String("/StatusNotifierItem"), this);

    registerToHost();

    // 监听 StatusNotifierWatcher
    QDBusServiceWatcher *watcher = new QDBusServiceWatcher(
        QLatin1String("org.kde.StatusNotifierWatcher"),
        mSessionBus,
        QDBusServiceWatcher::WatchForOwnerChange,
        this
    );
    connect(watcher, &QDBusServiceWatcher::serviceOwnerChanged,
            this, &StatusNotifierItem::onServiceOwnerChanged);
}

StatusNotifierItem::~StatusNotifierItem()
{
    mSessionBus.unregisterObject(QLatin1String("/StatusNotifierItem"));
    QDBusConnection::disconnectFromBus(mService);
}

void StatusNotifierItem::registerToHost()
{
    QDBusInterface interface(
        QLatin1String("org.kde.StatusNotifierWatcher"),
        QLatin1String("/StatusNotifierWatcher"),
        QLatin1String("org.kde.StatusNotifierWatcher"),
        mSessionBus
    );
    interface.asyncCall(QLatin1String("RegisterStatusNotifierItem"), mSessionBus.baseService());
}

void StatusNotifierItem::onServiceOwnerChanged(const QString& service, const QString&, const QString& newOwner)
{
    Q_UNUSED(service);
    if (!newOwner.isEmpty())
        registerToHost();
}

void StatusNotifierItem::onMenuDestroyed()
{
    mMenu = nullptr;
    setMenuPath(QLatin1String("/NO_DBUSMENU"));
}

void StatusNotifierItem::setTitle(const QString &title)
{
    if (mTitle == title) return;
    mTitle = title;
    Q_EMIT mAdaptor->NewTitle();
}

void StatusNotifierItem::setStatus(const QString &status)
{
    if (mStatus == status) return;
    mStatus = status;
    Q_EMIT mAdaptor->NewStatus(mStatus);
}

void StatusNotifierItem::setCategory(const QString &category)
{
    mCategory = category;
}

void StatusNotifierItem::setMenuPath(const QString& path)
{
    mMenuPath.setPath(path);
}

void StatusNotifierItem::setIconByName(const QString &name)
{
    if (mIconName == name) return;
    mIconName = name;
    Q_EMIT mAdaptor->NewIcon();
}

void StatusNotifierItem::setIconByPixmap(const QIcon &icon)
{
    if (mIconCacheKey == icon.cacheKey()) return;
    mIconCacheKey = icon.cacheKey();
    mIcon = iconToPixmapList(icon);
    mIconName.clear();
    Q_EMIT mAdaptor->NewIcon();
}

void StatusNotifierItem::setContextMenu(QMenu* menu)
{
    if (mMenu == menu) return;

    if (mMenu)
        disconnect(mMenu, &QObject::destroyed, this, &StatusNotifierItem::onMenuDestroyed);

    mMenu = menu;

    if (mMenu)
        setMenuPath(QLatin1String("/MenuBar"));
    else
        setMenuPath(QLatin1String("/NO_DBUSMENU"));

    if (mMenu)
        connect(mMenu, &QObject::destroyed, this, &StatusNotifierItem::onMenuDestroyed);
}

void StatusNotifierItem::Activate(int x, int y)
{
    if (mStatus == QLatin1String("NeedsAttention"))
        mStatus = QLatin1String("Active");
    Q_EMIT activateRequested(QPoint(x, y));
}

void StatusNotifierItem::SecondaryActivate(int x, int y)
{
    if (mStatus == QLatin1String("NeedsAttention"))
        mStatus = QLatin1String("Active");
    Q_EMIT secondaryActivateRequested(QPoint(x, y));
}

void StatusNotifierItem::ContextMenu(int x, int y)
{
    if (mMenu)
        mMenu->popup(QPoint(x, y));
}

void StatusNotifierItem::Scroll(int delta, const QString &orientation)
{
    Qt::Orientation orient = (orientation.toLower() == QLatin1String("horizontal")) ? Qt::Horizontal : Qt::Vertical;
    Q_EMIT scrollRequested(delta, orient);
}

void StatusNotifierItem::showMessage(const QString& title, const QString& msg,
                                     const QString& iconName, int secs)
{
    QDBusInterface iface(
        QLatin1String("org.freedesktop.Notifications"),
        QLatin1String("/org/freedesktop/Notifications"),
        QLatin1String("org.freedesktop.Notifications"),
        mSessionBus
    );
    iface.call(QLatin1String("Notify"), mTitle, (uint)0, iconName, title,
               msg, QStringList(), QVariantMap(), secs);
}

IconPixmapList StatusNotifierItem::iconToPixmapList(const QIcon& icon)
{
    IconPixmapList pixmapList;
    const QList<QSize> sizes = icon.availableSizes();
    for (const QSize &size : sizes)
    {
        QImage image = icon.pixmap(size).toImage();
        IconPixmap pix{size.width(), size.height(), QByteArray((char*)image.bits(), image.sizeInBytes())};

        // 转换为大端字节序
        if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)
        {
            quint32 *buf = (quint32*)pix.bytes.data();
            for (int i = 0; i < pix.bytes.size()/4; ++i)
            {
                buf[i] = qToBigEndian(buf[i]);
            }
        }

        pixmapList.append(pix);
    }
    return pixmapList;
}
