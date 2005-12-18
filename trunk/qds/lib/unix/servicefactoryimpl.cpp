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

// Qt includes
#include <qapplication.h>

// QDBus includes
#include <dbus/qdbusconnection.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>

// QDS includes
#include "qds/launcher.h"

// local includes
#include "../servicefactoryimpl.h"
#include "dbuslauncherimpl.h"
#include "mailcaplauncherimpl.h"
#include "servicefactoryimpl_p.h"

using namespace QDS;

///////////////////////////////////////////////////////////////////////////////

ServiceFactoryImpl::ServiceFactoryImpl() : m_private(0)
{
    m_private = new ServiceFactoryImplPrivate();
}

///////////////////////////////////////////////////////////////////////////////

ServiceFactoryImpl::~ServiceFactoryImpl()
{
    delete m_private;
}

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactoryImpl::init(int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QDBusConnection connection =
        QDBusConnection::addConnection(QDBusConnection::SessionBus);
    if (!connection.isConnected())
    {
        qWarning("QDS: no connection to D-BUS session bus");
    }

    m_private->daemon->setConnection(connection);

    return connection.isConnected();
}

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactoryImpl::initNetwork()
{
    if (!m_private->daemon->canSend()) return false;

    return false; // TODO call daemon
}

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactoryImpl::initLauncher()
{
    if (!m_private->daemon->canSend())
    {
        m_private->launcher = new Launcher();
        m_private->launcher->m_impl = new MailcapLauncherImpl();
        return true;
    }

    LauncherImpl* launcherImpl;

    QDBusError error;
    QDBusMessage reply =
        m_private->daemon->sendWithReply("InitLauncher", QValueList<QVariant>(), &error);
    if (error.isValid() || reply.count() < 1 || !reply[0].toBool())
    {
        if (error.isValid())
        {
            qWarning("Error while requesting launcher on D-Bus:\nerror '%s'\nmessage '%s'",
                     error.name().local8Bit().data(), error.message().local8Bit().data());
        }

        launcherImpl = new MailcapLauncherImpl();
    }
    else
    {
        QDBusProxy* proxy = new QDBusProxy(m_private->daemon, "QDS-Launcher-Proxy");
        proxy->setService("de.berlios.QDS");
        proxy->setPath("/QDS/Launcher");
        proxy->setInterface("de.berlios.qds.Launcher");
        proxy->setConnection(m_private->daemon->connection());

        launcherImpl = new DBusLauncherImpl(proxy);
    }

    m_private->launcher = new Launcher();
    m_private->launcher->m_impl = launcherImpl;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

Launcher* ServiceFactoryImpl::launcher()
{
    return m_private->launcher;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ServiceFactoryImplPrivate::ServiceFactoryImplPrivate()
    : QObject(0),
      daemon(0),
      launcher(0)
{
    daemon = new QDBusProxy(this, "QDS-Daemon-Proxy");

    daemon->setService("de.berlios.QDS");
    daemon->setPath("/QDS");
    daemon->setInterface("de.berlios.qds.Factory");
}

///////////////////////////////////////////////////////////////////////////////

ServiceFactoryImplPrivate::~ServiceFactoryImplPrivate()
{
}

// End of File
