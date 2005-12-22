/******************************************************************************
    Copyright 2005, Kevin Krammer <kevin.krammer@gmx.at>
    All Rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// QDBus includes
#include <dbus/qdbusconnection.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>

// QDS includes
#include "plugins/launcherimpl.h"

// local includes
#include "dbuslauncherimpl.h"
#include "dbusplugin.h"
#include "dbusplugin_p.h"

///////////////////////////////////////////////////////////////////////////////

DBusPlugin::DBusPlugin() : QObject(0, "QDS-DBusPlugin"), daemon(0)
{
    daemon = new QDBusProxy(this, "QDS-Daemon-Proxy");

    daemon->setService("de.berlios.QDS");
    daemon->setPath("/QDS");
    daemon->setInterface("de.berlios.qds.Factory");
}

///////////////////////////////////////////////////////////////////////////////

DBusPlugin::~DBusPlugin()
{
}

///////////////////////////////////////////////////////////////////////////////

bool DBusPlugin::init(int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QDBusConnection connection =
        QDBusConnection::addConnection(QDBusConnection::SessionBus);
    if (!connection.isConnected())
    {
        qWarning("QDS: no connection to D-BUS session bus");
    }

    daemon->setConnection(connection);

    return connection.isConnected();
}

///////////////////////////////////////////////////////////////////////////////

bool DBusPlugin::initNetwork()
{
    if (!daemon->canSend()) return false;

    return false; // TODO call daemon
}

///////////////////////////////////////////////////////////////////////////////

LauncherImpl* DBusPlugin::initLauncher()
{
    if (!daemon->canSend()) return 0;

    LauncherImpl* launcherImpl = 0;

    QDBusError error;
    QDBusMessage reply =
        daemon->sendWithReply("InitLauncher", QValueList<QVariant>(), &error);
    if (error.isValid() || reply.count() < 1 || !reply[0].toBool())
    {
        if (error.isValid())
        {
            qWarning("Error while requesting launcher on D-Bus:\nerror '%s'\nmessage '%s'",
                     error.name().local8Bit().data(), error.message().local8Bit().data());
        }
    }
    else
    {
        QDBusProxy* proxy = new QDBusProxy(daemon, "QDS-Launcher-Proxy");
        proxy->setService("de.berlios.QDS");
        proxy->setPath("/QDS/Launcher");
        proxy->setInterface("de.berlios.qds.Launcher");
        proxy->setConnection(daemon->connection());

        launcherImpl = new DBusLauncherImpl(proxy);
    }

    return launcherImpl;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DBusPlugin* createPlugin(int argc, char** argv)
{
    DBusPlugin* plugin = new DBusPlugin();
    if (plugin->init(argc, argv)) return plugin;

    delete plugin;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

bool initNetwork(DBusPlugin* plugin)
{
    if (plugin == 0) return false;

    return plugin->initNetwork();
}

///////////////////////////////////////////////////////////////////////////////

LauncherImpl* initLauncher(DBusPlugin * plugin)
{
    if (plugin == 0) return 0;

    return plugin->initLauncher();
}

// End of File
