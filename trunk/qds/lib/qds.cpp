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

// QDS includes
#include "qds/launcher.h"
#include "qds/qds.h"
#include "qds/servicefactory.h"

namespace QDS {

///////////////////////////////////////////////////////////////////////////////

QApplication* createApplication(int argc, char** argv, bool useGUI)
{
    ServiceFactory* factory = ServiceFactory::instance();

    QApplication* app = factory->createApplication(argc, argv, useGUI);

    factory->initService(Network);
    factory->initService(Launching);
    
    return app;
}

///////////////////////////////////////////////////////////////////////////////

bool launch(const QString& fileName)
{
    ServiceFactory* factory = ServiceFactory::instance();

    Launcher* launcher = factory->launcher();
    if (launcher == 0) return false;

    return launcher->launch(fileName);
}

///////////////////////////////////////////////////////////////////////////////

bool launch(const QUrl& url)
{
    ServiceFactory* factory = ServiceFactory::instance();

    Launcher* launcher = factory->launcher();
    if (launcher == 0) return false;

    return launcher->launch(url);
}

} // namespace QDS

// End of File
