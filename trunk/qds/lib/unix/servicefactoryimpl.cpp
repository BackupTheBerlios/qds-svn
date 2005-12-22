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

// QDS includes
#include "qds/launcher.h"

// local includes
#include "../servicefactoryimpl.h"
#include "dbusplugin.h"
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

    if (m_private->loadPlugin())
    {
        DBusPlugin* (*createPlugin)(int, char**);

        *(void**) (&createPlugin) = m_private->pluginLib->resolve("createPlugin");

        if (createPlugin != 0)
        {
            m_private->plugin = (*createPlugin)(argc, argv);
            return m_private->plugin != 0;
        }
    }

    qWarning("QDS: failed to load D-BUS plugin. Continuing with built-in services");

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactoryImpl::initNetwork()
{
    if (m_private->plugin == 0) return false;

    bool (*initNetwork)(DBusPlugin*);

    *(void**) (&initNetwork) = m_private->pluginLib->resolve("init");

    if (initNetwork == 0) return false;

    return (*initNetwork)(m_private->plugin);
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

    LauncherImpl* (*initLauncher)(DBusPlugin*);
    *(void**) (&initLauncher) = m_private->pluginLib->resolve("initLauncher");

    if (initLauncher == 0) return false;

    LauncherImpl* launcherImpl = (*initLauncher)(m_private->plugin);
    if (launcherImpl == 0)
    {
        qWarning("QDS: D-BUS plugin failed to create a launcher");

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

ServiceFactoryImplPrivate::ServiceFactoryImplPrivate()
    : pluginLib(0),
      plugin(0),
      launcher(0)
{
}

///////////////////////////////////////////////////////////////////////////////

ServiceFactoryImplPrivate::~ServiceFactoryImplPrivate()
{
}

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactoryImplPrivate::loadPlugin()
{
    QString libName = "qds-dbus";

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
