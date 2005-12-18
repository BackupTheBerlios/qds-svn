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

#ifndef LAUNCHERSERVICE_H
#define LAUNCHERSERVICE_H

// QDBus includes
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusobject.h>

// forward declarations
class QDBusError;
namespace QDS
{
    class Launcher;
};
class QString;
class QUrl;

class LauncherService : public QDBusObjectBase
{
public:
    LauncherService(const QString& path, QDS::Launcher* launcher);
    virtual ~LauncherService();

    void setConnection(const QDBusConnection& connection);

protected:
    virtual bool handleMethodCall(const QDBusMessage& message);

private:
    bool launch(const QString& filename, QDBusError& error);
    bool launch(const QString& filename, const QString& mimeType, QDBusError& error);
    bool launch(const QUrl& url, QDBusError& error);
    bool launch(const QUrl& url, const QString& mimeType, QDBusError& error);

    void sendReply(const QDBusMessage& message, bool ret, const QDBusError& error);

private:
    QDBusConnection m_connection;
    QString m_path;
    bool m_registered;

    QDS::Launcher* m_launcher;

private:
    LauncherService(const LauncherService&);
    LauncherService& operator=(const LauncherService&);
};

#endif

// End of File
