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

// QDS includes
#include "qds/launcher.h"

// QDS internal includes
#include "servicefactoryimpl.h"

// local includes
#include "factoryservice.h"
#include "launcherservice.h"

static const QString interface = "de.berlios.qds.Factory";
static const QString serviceUnavailable = "de.berlios.qds.Error.ServiceNotAvailable";

///////////////////////////////////////////////////////////////////////////////

FactoryService::FactoryService(const QString& path, QDS::ServiceFactoryImpl* factory)
    : m_path(path),
      m_registered(false),
      m_factory(factory),
      m_launcher(0)
{
}

///////////////////////////////////////////////////////////////////////////////

FactoryService::~FactoryService()
{
    if (m_registered) m_connection.unregisterObject(m_path);
}

///////////////////////////////////////////////////////////////////////////////

void FactoryService::setConnection(const QDBusConnection& connection)
{
    if (m_registered) m_connection.unregisterObject(m_path);

    m_connection = connection;

    m_registered = m_connection.registerObject(m_path, this);
    if (m_registered)
    {
        qDebug("Interface '%s' available at path '%s' on DBUS name '%s'",
               interface.local8Bit().data(), m_path.local8Bit().data(),
               m_connection.baseService().local8Bit().data());
    }

    // pass it on to sub objects
    if (m_launcher != 0) m_launcher->setConnection(connection);
}

///////////////////////////////////////////////////////////////////////////////

bool FactoryService::handleMethodCall(const QDBusMessage& message)
{
    if (message.interface() != interface) return false;

    QDBusError error;
    bool ret = false;

    if (message.name() == "InitLauncher")
    {
        ret = initLauncher(error);
    }
    else if (message.name() == "InitNetwork")
    {
        ret = initNetwork(error);
    }
    else
        return false;

    sendReply(message, ret, error);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool FactoryService::initLauncher(QDBusError& error)
{
    if (m_launcher != 0) return true;

    if (!m_factory->initLauncher())
    {
        // TODO translate message
        error = QDBusError(serviceUnavailable, "Unable to initialize Launcher Service");
        return false;
    }

    QDS::Launcher* launcher = m_factory->launcher();
    if (launcher == 0)
    {
        // TODO translate message
        error = QDBusError(serviceUnavailable, "Unable to initialize Launcher Service");
        return false;
    }

    m_launcher = new LauncherService(m_path + "/Launcher", launcher);
    m_launcher->setConnection(m_connection);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool FactoryService::initNetwork(QDBusError& error)
{
    // TODO translate message
    error = QDBusError(serviceUnavailable, "Unable to initialize Network Service");
    return false;
}

///////////////////////////////////////////////////////////////////////////////

void FactoryService::sendReply(const QDBusMessage& message, bool ret, const QDBusError& error)
{
    QDBusMessage reply;

    if (error.isValid())
    {
        reply = QDBusMessage::methodError(message, error);
    }
    else
    {
        reply = QDBusMessage::methodReply(message);
        reply.append(QVariant(ret, ret));
    }

    m_connection.send(reply);
}

// End of File
