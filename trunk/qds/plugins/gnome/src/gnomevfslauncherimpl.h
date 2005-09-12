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

#ifndef GNOMEVFSLAUNCHERIMPL_H
#define GNOMEVFSLAUNCHERIMPL_H

// Plugin includes
#include "launcherimpl.h"

// forward declarations
class QString;
class QUrl;
class QWidget;

class GNOMEVFSLauncherImpl : public LauncherImpl
{
public:
    GNOMEVFSLauncherImpl();
    virtual ~GNOMEVFSLauncherImpl();

    virtual bool launch(const QString& fileName, QWidget* window = 0);

    virtual bool launch(const QString& fileName, const QString& mimeType,
                        QWidget* window = 0);

    virtual bool launch(const QUrl& url, QWidget* window = 0);

    virtual bool launch(const QUrl& url, const QString& mimeType,
                        QWidget* window = 0);

private:
    bool m_initOK;

private:
    QString mimeTypeForURL(const QUrl& url);
};

#endif

// End of File
