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
#include <qlibrary.h>
#include <qsettings.h>

// QDS includes
#include "qds/launcher.h"

// QDS internal includes
#include "servicefactoryimpl.h"
#include "unix/mailcaplauncherimpl.h"

///////////////////////////////////////////////////////////////////////////////

namespace QDS
{

class ServiceFactoryImplPrivate
{
public:
    ServiceFactoryImplPrivate() : pluginLib(0), launcher(0)
    {
    }

    bool loadPlugin(const QString& name);

public:
    QLibrary* pluginLib;
    Launcher* launcher;
};

};

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
    QApplication* app = 0;

    QString pluginName;
    for (int i = 0; i < argc; ++i)
    {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg.startsWith("--plugin="))
        {
            pluginName = arg.mid(9);
            break;
        }
    }

    // no commandline override, check config
    if (pluginName.isEmpty())
    {
        qDebug("No QDS plugin specified with --plugin=name commandline option. Checking config");

        QSettings config;
        config.setPath("qdesktopservices", "QDesktopServices");

        config.beginGroup("qds"); // this results in a Qt config file called qdsrc

        pluginName = config.readEntry("/General/Plugin");
    }

    // no plugin in config either, try detection
    if (pluginName.isEmpty())
    {
        qDebug("No QDS plugin specified in config file .qt/qdsrc. Trying detection");

        // use KDE plugin if KDE_FULL_SESSION is true
        // use GNOME plugin if GNOME_DESKTOP_SESSION_ID is set

        char* value = std::getenv("KDE_FULL_SESSION");
        if (value != 0 && QString::fromLocal8Bit(value) == "true")
        {
                pluginName = "kde";
        }
        else
        {
            value = std::getenv("GNOME_DESKTOP_SESSION_ID");
            if (value != 0 && !QString::fromLocal8Bit(value).isEmpty())
            {
                pluginName = "gnome";
            }
        }
    }

    if (pluginName.isEmpty())
    {
        qWarning("QDS-Daemon: cannot determine which plugin to use.\n"
                 "Continuing with standard Qt feature set");
        return app;
    }

    qDebug("Using QDS plugin '%s'", pluginName.local8Bit().data());

    if (m_private->loadPlugin(pluginName))
    {
        QApplication* (*createApp)(int, char**, bool);

        *(void**) (&createApp) = m_private->pluginLib->resolve("createApplication");

        if (createApp != 0)
        {
            app = (*createApp)(argc, argv, false);
        }
    }
    else
        qDebug("Failed to load plugin '%s'. Continuing with standard Qt feature set",
               pluginName.local8Bit().data());

    return app;
}

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactoryImpl::initNetwork()
{
    if (m_private->pluginLib == 0) return false;

    bool (*initNetwork)();
    *(void**) (&initNetwork) = m_private->pluginLib->resolve("initNetwork");

    if (initNetwork == 0) return false;

    return (*initNetwork)();
}

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactoryImpl::initLauncher()
{
    if (m_private->pluginLib == 0)
    {
        m_private->launcher = new Launcher();
        m_private->launcher->m_impl = new MailcapLauncherImpl();
        return true;
    }

    LauncherImpl* (*initLauncher)();
    *(void**) (&initLauncher) = m_private->pluginLib->resolve("initLauncher");

    if (initLauncher == 0) return false;

    LauncherImpl* launcherImpl = (*initLauncher)();
    if (launcherImpl == 0)
    {
        qWarning("QDS-Daemon: plugin failed to create a launcher");

        launcherImpl = new MailcapLauncherImpl();
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

bool ServiceFactoryImplPrivate::loadPlugin(const QString& name)
{
    QString libName = "qds_" + name;

    QLibrary* lib = new QLibrary(libName);

    if (!lib->load())
    {
        // Some Qt version do not create the full name correctly, try again
        // using full name
        libName = "lib" + libName + ".so";

        delete lib;
        lib = new QLibrary(libName);

        if (!lib->load())
        {
            delete lib;
            return false;
        }
    }

    pluginLib = lib;

    return true;
}

// End of File
