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
#include <qglobal.h>

// QDS includes
#include "qds/launcher.h"

// local includes
#include "../servicefactoryimpl.h"

///////////////////////////////////////////////////////////////////////////////

namespace QDS
{

class ServiceFactoryImplPrivate
{
public:
    ServiceFactoryImplPrivate() : launcher(0)
    {
    }

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
}

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactoryImpl::init(int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactoryImpl::initNetwork()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool ServiceFactoryImpl::initLauncher()
{
    m_private->launcher = new Launcher();

    return m_private->launcher != 0;
}

///////////////////////////////////////////////////////////////////////////////

Launcher* ServiceFactoryImpl::launcher()
{
    return m_private->launcher;
}

// End of File
