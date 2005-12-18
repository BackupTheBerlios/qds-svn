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
#include <qnetwork.h>

// QDS includes
#include "qds/servicefactory.h"

// local includes
#include "servicefactoryimpl.h"

///////////////////////////////////////////////////////////////////////////////

using namespace QDS;

ServiceFactory* ServiceFactory::m_instance = 0;

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactory::init(int argc, char** argv)
{
    return m_impl->init(argc, argv);
}

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactory::initService(Service service)
{
    bool result = false;

    switch (service)
    {
        case Network:
            if (!m_impl->initNetwork())
                qInitNetworkProtocols();

            result = true;
            break;

        case Launching:
            result = m_impl->initLauncher();
            break;

        default:
            break;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////

Launcher* ServiceFactory::launcher()
{
    return m_impl->launcher();
}

///////////////////////////////////////////////////////////////////////////////

ServiceFactory::ServiceFactory() : m_impl(0)
{
    m_impl = new ServiceFactoryImpl();
}

///////////////////////////////////////////////////////////////////////////////

ServiceFactory::~ServiceFactory()
{
    delete m_impl;
}

// End of File
