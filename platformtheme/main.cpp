#include <qpa/qplatformthemeplugin.h>
#include "platformtheme.h"

QT_BEGIN_NAMESPACE

class PlatformThemePlugin : public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformThemeFactoryInterface.5.1" FILE "cutefish-platformtheme.json")

public:
    PlatformThemePlugin(QObject *parent = nullptr)
        : QPlatformThemePlugin(parent) {}

    QPlatformTheme *create(const QString &key, const QStringList &paramList) override
    {
        Q_UNUSED(key)
        Q_UNUSED(paramList)
        return new PlatformTheme;
    }
};

QT_END_NAMESPACE

#include "main.moc"
