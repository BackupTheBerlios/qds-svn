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

// Qt includes
#include <qfileinfo.h>
#include <qurl.h>

// KDE includes
#include <krun.h>
#include <kurl.h>

// local includes
#include "klauncherimpl.h"

///////////////////////////////////////////////////////////////////////////////

class KLauncherPrivate
{
public:
    KLauncherPrivate() {}
    ~KLauncherPrivate() {}

    KURL convertURL(const QUrl& url);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

KLauncherImpl::KLauncherImpl() : m_private(0)
{
    m_private = new KLauncherPrivate();
}

///////////////////////////////////////////////////////////////////////////////

KLauncherImpl::~KLauncherImpl()
{
    delete m_private;
}

///////////////////////////////////////////////////////////////////////////////

bool KLauncherImpl::launch(const QString& fileName, QWidget* window)
{
    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists()) return false;

    KURL url = KURL::fromPathOrURL(fileInfo.absFilePath());

    KRun* run = new KRun(url, window, 0, true);

    return !run->hasError();
}

///////////////////////////////////////////////////////////////////////////////

bool KLauncherImpl::launch(const QString& fileName, const QString& mimeType,
                           QWidget* window)
{
    if (mimeType.isEmpty())
        return launch(fileName, window);

    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists()) return false;

    KURL url = KURL::fromPathOrURL(fileInfo.absFilePath());

    return KRun::runURL(url, mimeType, false) != 0;
}

///////////////////////////////////////////////////////////////////////////////

bool KLauncherImpl::launch(const QUrl& url, QWidget* window)
{
    KRun* run = new KRun(m_private->convertURL(url), window);

    return !run->hasError();
}

///////////////////////////////////////////////////////////////////////////////

bool KLauncherImpl::launch(const QUrl& url, const QString& mimeType,
                           QWidget* window)
{
    Q_UNUSED(window)

    if (mimeType.isEmpty())
        return launch(url, window);

    return KRun::runURL(m_private->convertURL(url), mimeType, false) != 0;
}

///////////////////////////////////////////////////////////////////////////////

KURL KLauncherPrivate::convertURL(const QUrl& url)
{
    KURL kurl(url);
    if (!url.hasPort()) kurl.setPort(0);

    return kurl;
}

// end of file
