///////////////////////////////////////////////////////////////////////////////
//
// Copyright: Kevin Krammer <kevin.krammer@gmx.at>, 2004
//
///////////////////////////////////////////////////////////////////////////////
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Lesser General Public License as
//   published by the Free Software Foundation; either version 2 of the
//   License, or (at your option) any later version.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef KLAUNCHERIMPL_H
#define KLAUNCHERIMPL_H

// Qt includes
#include <qobject.h>

// plugin includes
#include "launcherimpl.h"

// forward declarations
class KLauncherPrivate;
class QString;
class QUrl;
class QWidget;

class KLauncherImpl : public LauncherImpl
{
public:
    KLauncherImpl();
    virtual ~KLauncherImpl();

    virtual bool launch(const QString& fileName, QWidget* window = 0);

    virtual bool launch(const QString& fileName, const QString& mimeType,
                        QWidget* window = 0);

    virtual bool launch(const QUrl& url, QWidget* window = 0);

    virtual bool launch(const QUrl& url, const QString& mimeType,
                        QWidget* window = 0);

private:
    KLauncherPrivate* m_private;
};

#endif

// end of file
