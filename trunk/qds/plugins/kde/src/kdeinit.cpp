///////////////////////////////////////////////////////////////////////////////
//
// Copyright: Kevin Krammer <kevin.krammer@gmx.at>, 2004-2005
//
///////////////////////////////////////////////////////////////////////////////
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Lesser General Public License as
//   published by the Free Software Foundation; either version 2 of the
//   License, or (at your option) any later version.
//
///////////////////////////////////////////////////////////////////////////////

// KDE includes
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kprotocolinfo.h>

// plugin includes
#include "qdsplugin.h"

// local includes
#include "klauncherimpl.h"
#include "knetworkprotocol.h"

///////////////////////////////////////////////////////////////////////////////

static const KCmdLineOptions options[] =
{
    {"+[args]", 0, 0},
    KCmdLineLastOption
};

QApplication* createApplication(int argc, char** argv, bool useGUI)
{
    KCmdLineArgs::init(argc, argv, argv[0], argv[0], "QDS Plugin for KDE", "1.0");
    KCmdLineArgs::addCmdLineOptions(options);
    
    QApplication* app = new KApplication(useGUI, useGUI);

    // EVIL HACK
    // somehow the argv[1] got lost, restore it
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    if (args->count() > 0)
    {
        qApp->argv()[1] = (char*)args->arg(0);
    }
    
    return app;
}

///////////////////////////////////////////////////////////////////////////////

bool initNetwork()
{
    // check if KDE protocols have at least FILE, HTTP, FTP
    QStringList protocols = KProtocolInfo::protocols();
    QStringList::iterator endIt = protocols.end();
    if (protocols.find("file") == endIt) return false;
    if (protocols.find("http") == endIt) return false;
    if (protocols.find("ftp")  == endIt) return false;

    QStringList::iterator it = protocols.begin();
    for (; it != endIt; ++it)
    {
        QNetworkProtocol::registerNetworkProtocol(*it,
            new KNetworkProtocolFactory(*it));
    }

    kdDebug() << "KIO backend initialized with " << protocols.count() << " protocols" << endl;
    return true;
}

///////////////////////////////////////////////////////////////////////////////

LauncherImpl* initLauncher()
{
    kdDebug() << "Initializing KRun based launcher" << endl;
    return new KLauncherImpl();
}

// end of file
