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
#include <qfileinfo.h>
#include <qurl.h>

// QDBus includes
#include <dbus/qdbuserror.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>

// local includes
#include "dbuslauncherimpl.h"

using namespace QDS;

///////////////////////////////////////////////////////////////////////////////

DBusLauncherImpl::DBusLauncherImpl(QDBusProxy* proxy) : m_proxy(proxy)
{
}

///////////////////////////////////////////////////////////////////////////////

DBusLauncherImpl::~DBusLauncherImpl()
{
}

///////////////////////////////////////////////////////////////////////////////

bool DBusLauncherImpl::launch(const QString& fileName, QWidget* window)
{
    Q_UNUSED(window);

    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists() || !fileInfo.isReadable()) return false;

    QDBusError error;

    QValueList<QVariant> params;
    params.append(QVariant(fileInfo.absFilePath()));

    QDBusMessage reply = m_proxy->sendWithReply("LaunchFile", params, &error);

    if (error.isValid())
    {
        qWarning("Error while launching through D-Bus:\nerror '%s'\nmessage '%s'",
                    error.name().local8Bit().data(), error.message().local8Bit().data());
        return false;
    }

    if (reply.count() != 1 || reply[0].type() != QVariant::Bool)
    {
        qWarning("Unexpected launcher reply");
        return false;
    }

    return reply[0].toBool();
}

///////////////////////////////////////////////////////////////////////////////

bool DBusLauncherImpl::launch(const QString& fileName, const QString& mimeType,
                                 QWidget* window)
{
    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists() || !fileInfo.isReadable()) return false;

    if (mimeType.isEmpty()) return launch(fileName, window);

    QDBusError error;
    QValueList<QVariant> params;
    params.append(fileInfo.absFilePath());
    params.append(mimeType);

    QDBusMessage reply = m_proxy->sendWithReply("LaunchFile", params, &error);

    if (error.isValid())
    {
        qWarning("Error while launching through D-Bus:\nerror '%s'\nmessage '%s'",
                    error.name().local8Bit().data(), error.message().local8Bit().data());
        return false;
    }

    if (reply.count() != 1 || reply[0].type() != QVariant::Bool)
    {
        qWarning("Unexpected launcher reply");
        return false;
    }

    return reply[0].toBool();
}

///////////////////////////////////////////////////////////////////////////////

bool DBusLauncherImpl::launch(const QUrl& url, QWidget* window)
{
    if (!url.isValid()) return false;

    if (url.isLocalFile()) return launch(url.path(), window); 

    QDBusError error;
    QValueList<QVariant> params;
    params.append(url.toString());

    qDebug("LaunchURL: %s", url.toString().latin1());
    QDBusMessage reply = m_proxy->sendWithReply("LaunchURL", params, &error);

    if (error.isValid())
    {
        qWarning("Error while launching through D-Bus:\nerror '%s'\nmessage '%s'",
                    error.name().local8Bit().data(), error.message().local8Bit().data());
        return false;
    }

    if (reply.count() != 1 || reply[0].type() != QVariant::Bool)
    {
        qWarning("Unexpected launcher reply");
        return false;
    }

    return reply[0].toBool();
}

///////////////////////////////////////////////////////////////////////////////

bool DBusLauncherImpl::launch(const QUrl& url, const QString& mimeType, QWidget* window)
{
    if (!url.isValid()) return false;

    if (url.isLocalFile()) return launch(url.path(), mimeType, window); 

    if (mimeType.isEmpty()) return launch(url, window);

    QDBusError error;
    QValueList<QVariant> params;
    params.append(url.toString());
    params.append(mimeType);

    QDBusMessage reply = m_proxy->sendWithReply("LaunchURL", params, &error);

    if (error.isValid())
    {
        qWarning("Error while launching through D-Bus:\nerror '%s'\nmessage '%s'",
                    error.name().local8Bit().data(), error.message().local8Bit().data());
        return false;
    }

    if (reply.count() != 1 || reply[0].type() != QVariant::Bool)
    {
        qWarning("Unexpected launcher reply");
        return false;
    }

    return reply[0].toBool();
}

// End of File
