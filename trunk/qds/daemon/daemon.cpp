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
#include <qtimer.h>

// QDBus includes
#include <dbus/qdbusconnection.h>

// QDS includes
#include "qds/launcher.h"

// QDS internal includes
#include "servicefactoryimpl.h"

// local includes
#include "daemon.h"
#include "factoryservice.h"

using namespace QDS;

///////////////////////////////////////////////////////////////////////////////

Daemon::Daemon() : QObject(0, "QDS-Daemon"), m_factory(0), m_service(0)
{
    m_factory = new ServiceFactoryImpl();

    QTimer::singleShot(0, this, SLOT(slotRegisterService()));
}

///////////////////////////////////////////////////////////////////////////////

Daemon::~Daemon()
{
    delete m_service;
    delete m_factory;
}

///////////////////////////////////////////////////////////////////////////////

QApplication* Daemon::createApplication(int argc, char** argv)
{
    QApplication* app = 0;
    if (!m_factory->init(argc, argv))
    {
        app = new QApplication(argc, argv, false);
    }
    else
        app = qApp;

    return app;
}

///////////////////////////////////////////////////////////////////////////////

void Daemon::slotRegisterService()
{
    QDBusConnection connection = QDBusConnection::addConnection(QDBusConnection::SessionBus);
    if (!connection.isConnected())
    {
        qFatal("Cannot connect to D-BUS session bus");
    }

    connection.requestName("de.berlios.QDS", QDBusConnection::NoReplace);

    m_service = new FactoryService("/QDS", m_factory);
    m_service->setConnection(connection);
}

// End of File
