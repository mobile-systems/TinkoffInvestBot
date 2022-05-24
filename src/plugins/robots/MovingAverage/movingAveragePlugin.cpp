#include <QDebug>
#include <QCoreApplication>
#include <QMetaMethod>
#include "hevaa_consts.h"
#include "hevaa_transport.h"
#include "hevaa_tree.h"
#include "movingAveragePlugin.h"
#include "customservice.h"
#include "sandboxservice.h"
#include "commontypes.h"

using namespace hevaa;

MovingAverage::MovingAverage(AppSettins &plugin_settings): CustomComponent(plugin_settings)
{
    setObjectName("MovingAverage");
    m_client = QSharedPointer<InvestApiClient>::create(m_plugin_settings["TINKHOST"].toStdString(), m_plugin_settings["TINKTOKEN"].toStdString());
}

MovingAverage::~MovingAverage()
{

}

const hevaa::transport::Node MovingAverage::ComponentInfo()
{
    hevaa::transport::Row rootdata = {objectName()};
    auto root = hevaa::transport::Node::create(rootdata);
    return root;
}

void MovingAverage::handleData(const hevaa::transport::message &msg)
{
    CustomComponent::handleData(msg);
    try {
        if (msg.type() == hevaa::transport::Command)
        {

        }
    } catch (std::exception& e) {
         qCritical() << QString("error: %1").arg(e.what());
    }
}

MovingAverageManager::~MovingAverageManager()
{

}

QString MovingAverageManager::robotName() const
{
    return "MovingAverageModule";
}

QSharedPointer<CustomComponent> MovingAverageManager::getComponent() const
{
    return m_component;
}

void MovingAverageManager::init(AppSettins &plugin_settings)
{
    m_component = QSharedPointer<MovingAverage>::create(plugin_settings);
}

void MovingAverageManager::start()
{
    m_thread.setObjectName(robotName());
    m_component->moveToThread(&m_thread);

    qInfo() << "Starting" << m_thread.objectName() << "...";
    m_thread.start();
}

void MovingAverageManager::stop()
{
    m_thread.requestInterruption();
    m_thread.quit();
    m_thread.wait();
}

