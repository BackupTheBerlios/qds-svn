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
#include <qurl.h>

// QDBus includes
#include <dbus/qdbuserror.h>
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusmessage.h>

// QDS includes
#include "qds/launcher.h"

// local includes
#include "launcherservice.h"

static const QString interface = "de.berlios.qds.Launcher";

///////////////////////////////////////////////////////////////////////////////

LauncherService::LauncherService(const QString& path, QDS::Launcher* launcher)
    : m_path(path),
      m_registered(false),
      m_launcher(launcher)
{
}

///////////////////////////////////////////////////////////////////////////////

LauncherService::~LauncherService()
{
}

///////////////////////////////////////////////////////////////////////////////

void LauncherService::setConnection(const QDBusConnection& connection)
{
    if (m_registered) m_connection.unregisterObject(m_path);

    m_connection = connection;

    m_registered = m_connection.registerObject(m_path, this);
    if (m_registered)
    {
        qDebug("Interface '%s' available at path '%s' on DBUS name '%s'",
               interface.local8Bit().data(), m_path.local8Bit().data(),
               m_connection.uniqueName().local8Bit().data());
    }
}

///////////////////////////////////////////////////////////////////////////////

bool LauncherService::handleMethodCall(const QDBusMessage& message)
{
    if (message.interface() != interface) return false;

    bool ret;
    QDBusError error;

    if (message.member() == "LaunchFile")
    {
        switch (message.count())
        {
            case 1:
                if (message[0].type() == QVariant::String)
                    ret = launch(message[0].toString(), error);
                else
                    return false;
            break;

            case 2:
                if (message[0].type() == QVariant::String &&
                    message[1].type() == QVariant::String)
                {
                    ret = launch(message[0].toString(), message[1].toString(), error);
                }
                else
                    return false;
            break;

            default:
                return false;
        }
    }
    else if (message.member() == "LaunchURL")
    {
        switch (message.count())
        {
            case 1:
                if (message[0].type() == QVariant::String)
                    ret = launch(QUrl(message[0].toString()), error);
                else
                    return false;
            break;

            case 2:
                if (message[0].type() == QVariant::String &&
                    message[1].type() == QVariant::String)
                {
                    ret = launch(QUrl(message[0].toString()), message[1].toString(), error);
                }
                else
                    return false;
            break;

            default:
                return false;
        }
    }
    else
        return false;

    sendReply(message, ret, error);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool LauncherService::launch(const QString& filename, QDBusError& error)
{
    Q_UNUSED(error);

    return m_launcher->launch(filename);
}

///////////////////////////////////////////////////////////////////////////////

bool LauncherService::launch(const QString& filename, const QString& mimeType, QDBusError& error)
{
    Q_UNUSED(error);

    return m_launcher->launch(filename, mimeType);
}

///////////////////////////////////////////////////////////////////////////////

bool LauncherService::launch(const QUrl& url, QDBusError& error)
{
    Q_UNUSED(error);
    return m_launcher->launch(url);
}

///////////////////////////////////////////////////////////////////////////////

bool LauncherService::launch(const QUrl& url, const QString& mimeType, QDBusError& error)
{
    Q_UNUSED(error);

    return m_launcher->launch(url, mimeType);
}

///////////////////////////////////////////////////////////////////////////////

void LauncherService::sendReply(const QDBusMessage& message, bool ret, const QDBusError& error)
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
