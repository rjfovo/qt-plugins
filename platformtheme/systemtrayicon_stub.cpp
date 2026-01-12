#include "systemtrayicon.h"
#include <QRect>
#include <QAction>
#include <QMenu>

SystemTrayIcon::SystemTrayIcon()
    : QPlatformSystemTrayIcon()
    , mSni(nullptr)
{
}

SystemTrayIcon::~SystemTrayIcon()
{
}

void SystemTrayIcon::init()
{
}

void SystemTrayIcon::cleanup()
{
}

void SystemTrayIcon::updateIcon(const QIcon &icon)
{
    Q_UNUSED(icon);
}

void SystemTrayIcon::updateToolTip(const QString &tooltip)
{
    Q_UNUSED(tooltip);
}

void SystemTrayIcon::updateMenu(QPlatformMenu *menu)
{
    Q_UNUSED(menu);
}

QRect SystemTrayIcon::geometry() const
{
    return QRect();
}

void SystemTrayIcon::showMessage(const QString &title, const QString &msg,
                                 const QIcon &icon, MessageIcon iconType, int secs)
{
    Q_UNUSED(title);
    Q_UNUSED(msg);
    Q_UNUSED(icon);
    Q_UNUSED(iconType);
    Q_UNUSED(secs);
}

bool SystemTrayIcon::isSystemTrayAvailable() const
{
    return false;
}

bool SystemTrayIcon::supportsMessages() const
{
    return false;
}

QPlatformMenu *SystemTrayIcon::createMenu() const
{
    return nullptr;
}

// Minimal stub implementations for SystemTrayMenu and SystemTrayMenuItem
SystemTrayMenu::SystemTrayMenu()
    : m_tag(0)
    , m_menu(nullptr)
{
}

SystemTrayMenu::~SystemTrayMenu()
{
}

void SystemTrayMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    Q_UNUSED(menuItem);
    Q_UNUSED(before);
}

QPlatformMenuItem *SystemTrayMenu::menuItemAt(int position) const
{
    Q_UNUSED(position);
    return nullptr;
}

QPlatformMenuItem *SystemTrayMenu::menuItemForTag(quintptr tag) const
{
    Q_UNUSED(tag);
    return nullptr;
}

void SystemTrayMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    Q_UNUSED(menuItem);
}

void SystemTrayMenu::setEnabled(bool enabled)
{
    Q_UNUSED(enabled);
}

void SystemTrayMenu::setIcon(const QIcon &icon)
{
    Q_UNUSED(icon);
}

void SystemTrayMenu::setTag(quintptr tag)
{
    m_tag = tag;
}

void SystemTrayMenu::setText(const QString &text)
{
    Q_UNUSED(text);
}

void SystemTrayMenu::setVisible(bool visible)
{
    Q_UNUSED(visible);
}

void SystemTrayMenu::syncMenuItem(QPlatformMenuItem *menuItem)
{
    Q_UNUSED(menuItem);
}

void SystemTrayMenu::syncSeparatorsCollapsible(bool enable)
{
    Q_UNUSED(enable);
}

quintptr SystemTrayMenu::tag() const
{
    return m_tag;
}

QPlatformMenuItem *SystemTrayMenu::createMenuItem() const
{
    return nullptr;
}

QMenu *SystemTrayMenu::menu() const
{
    return m_menu;
}

SystemTrayMenuItem::SystemTrayMenuItem()
    : m_tag(0)
    , m_action(nullptr)
{
}

SystemTrayMenuItem::~SystemTrayMenuItem()
{
}

void SystemTrayMenuItem::setCheckable(bool checkable)
{
    Q_UNUSED(checkable);
}

void SystemTrayMenuItem::setChecked(bool isChecked)
{
    Q_UNUSED(isChecked);
}

void SystemTrayMenuItem::setEnabled(bool enabled)
{
    Q_UNUSED(enabled);
}

void SystemTrayMenuItem::setFont(const QFont &font)
{
    Q_UNUSED(font);
}

void SystemTrayMenuItem::setIcon(const QIcon &icon)
{
    Q_UNUSED(icon);
}

void SystemTrayMenuItem::setIsSeparator(bool isSeparator)
{
    Q_UNUSED(isSeparator);
}

void SystemTrayMenuItem::setMenu(QPlatformMenu *menu)
{
    Q_UNUSED(menu);
}

void SystemTrayMenuItem::setRole(MenuRole role)
{
    Q_UNUSED(role);
}

void SystemTrayMenuItem::setShortcut(const QKeySequence &shortcut)
{
    Q_UNUSED(shortcut);
}

void SystemTrayMenuItem::setTag(quintptr tag)
{
    m_tag = tag;
}

void SystemTrayMenuItem::setText(const QString &text)
{
    Q_UNUSED(text);
}

void SystemTrayMenuItem::setVisible(bool isVisible)
{
    Q_UNUSED(isVisible);
}

quintptr SystemTrayMenuItem::tag() const
{
    return m_tag;
}

void SystemTrayMenuItem::setIconSize(int size)
{
    Q_UNUSED(size);
}

QAction *SystemTrayMenuItem::action() const
{
    return m_action;
}
