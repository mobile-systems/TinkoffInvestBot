#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QPluginLoader>
#include <QSharedPointer>
#include <QSettings>
#include "hevaa_consts.h"
#include "customcomponent.h"
#include "imoduleplugin.h"
#include "pluginsloader.h"

using namespace  hevaa;

PluginsLoader::PluginsLoader(QObject *parent) : QObject(parent)
{

}

PluginsLoader::PluginsLoader(QString password, QObject *parent) :
    QObject(parent),
    m_password(password)
{
    if (loadSettings()) {
        loadModules();
//        connectModules(hevaa::MODULE_NAME_TELEGRAM, hevaa::MODULE_NAME_DATABASE);
//        connectModules(hevaa::MODULE_NAME_DATABASE, hevaa::MODULE_NAME_TELEGRAM);
        connectModules(hevaa::MODULE_NAME_TINKOFF, hevaa::MODULE_NAME_TELEGRAM);
        connectModules(hevaa::MODULE_NAME_TELEGRAM, hevaa::MODULE_NAME_TINKOFF);
        startModules();
    }
}

PluginsLoader::~PluginsLoader()
{
    stopModules();
    saveSettings();
}

bool PluginsLoader::isSettingsOk()
{
    return m_isSettingsOk;
}

ModulesList *PluginsLoader::modulesList()
{
    return &m_modules_list;
}

bool PluginsLoader::loadSettings()
{
    QString SettingsFillPath = c_SettingsFile;
    QSettings settings(SettingsFillPath, QSettings::IniFormat);
    m_isSettingsOk = true;
    for(auto hs_key : m_app_settings.keys())
    {
        if (settings.value(hs_key).isNull())
        {
            m_isSettingsOk = false;
            qDebug() << "Не установлена переменная окружения" << hs_key;
        }
    }
    return m_isSettingsOk;
}

void PluginsLoader::connectModules(const QString &sender, const QString &recipient)
{
    hevaa::IModulePlugin *senderModule = qobject_cast<hevaa::IModulePlugin *>(m_modules_list[sender]);
    hevaa::IModulePlugin *recipientModule = qobject_cast<hevaa::IModulePlugin *>(m_modules_list[recipient]);

    if ((senderModule) && (recipientModule)) {
        auto sender = senderModule->getComponent();
        auto reciever = recipientModule->getComponent();

        if ((sender) && (reciever)) {
            qDebug() << senderModule->moduleName() << "connected to" << recipientModule->moduleName();
            connect(sender.data(), &CustomComponent::transmitData, reciever.data(), &CustomComponent::handleData);
        } else {
            if (!reciever) qDebug() << "Recipient" << recipient << "does not exist";
            if (!sender) qDebug() << "Sender" << sender << " does not exist";
        }
    }
}

void PluginsLoader::startModules()
{
    foreach (QObject * value, m_modules_list) {
        auto module = qobject_cast<hevaa::IModulePlugin * >(value);
        if (module) {
            module->startModule();
        }
    }
}

void PluginsLoader::saveSettings()
{
    QString fullFileName = c_SettingsFile;
    if (!QFileInfo(fullFileName).exists())
    {
        qInfo() << "Файл" << fullFileName << "не найден. Создается новый файл настроек.";
        QSettings settings(fullFileName, QSettings::IniFormat);
        uint i = 1;
        settings.setValue("count", i);
        QMapIterator<QString, QString> it {m_app_settings};
        while(it.hasNext())
        {
            it.next();
            settings.setValue(it.key(), "");
        }
    }
}

void PluginsLoader::stopModules()
{
    foreach (QObject * value, m_modules_list) {
        auto module = qobject_cast<hevaa::IModulePlugin * >(value);
        if (module) {
            module->stopModule();
        }
    }
}

void PluginsLoader::loadModules()
{
    QDir pluginsDir = QDir(QCoreApplication::applicationDirPath() + "/plugins");
    const auto entryList = pluginsDir.entryList(QDir::Files);
    for (const QString &fileName : entryList) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        auto plugin = loader.instance();
        auto module = qobject_cast<hevaa::IModulePlugin *>(plugin);
        if (module) {
            qDebug() << "Plugin" << module->moduleName() << "is loaded";
            module->initModule(m_app_settings);
            m_modules_list.insert(module->moduleName(), plugin);
        }
    }
}
