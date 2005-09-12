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

// plugin includes
#include "qdsplugin.h"

// local includes
#include "gnomevfslauncherimpl.h"

///////////////////////////////////////////////////////////////////////////////

LauncherImpl* initLauncher()
{
    return new GNOMEVFSLauncherImpl();
}

// end of file
