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

// GNOME includes
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-mime.h>
#include <libgnomevfs/gnome-vfs-mime-handlers.h>

// Qt includes
#include <qfileinfo.h>
#include <qurl.h>

// local includes
#include "gnomevfslauncherimpl.h"

///////////////////////////////////////////////////////////////////////////////

GNOMEVFSLauncherImpl::GNOMEVFSLauncherImpl() : m_initOK(false)
{
    m_initOK = gnome_vfs_init() != 0;
    qDebug("Initializing GNOME-VFS based launcher: gnome_vfs_init=%d", m_initOK);
}

///////////////////////////////////////////////////////////////////////////////

GNOMEVFSLauncherImpl::~GNOMEVFSLauncherImpl()
{
    gnome_vfs_shutdown();
}

///////////////////////////////////////////////////////////////////////////////

bool GNOMEVFSLauncherImpl::launch(const QString& fileName, QWidget* window)
{
    if (!m_initOK) return false;
    if (fileName.isEmpty()) return false;

    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists()) return false;

    QUrl url(fileInfo.absFilePath());
    
    return launch(url, mimeTypeForURL(url), window);
}

///////////////////////////////////////////////////////////////////////////////

bool GNOMEVFSLauncherImpl::launch(const QString& fileName, const QString& mimeType,
                                  QWidget* window)
{
    if (!m_initOK) return false;
    if (fileName.isEmpty() || mimeType.isEmpty()) return false;

    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists()) return false;
    
    QUrl url(fileInfo.absFilePath());
    
    return launch(url, mimeType, window);
}

///////////////////////////////////////////////////////////////////////////////

bool GNOMEVFSLauncherImpl::launch(const QUrl& url, QWidget* window)
{
    Q_UNUSED(window);
    if (!m_initOK) return false;
    if (!url.isValid()) return false;

#if 0
    char* uri = strdup(url.toString().utf8());
    GnomeVFSResult result = gnome_vfs_url_show(uri);
    free(uri);
    
    qDebug("result(%d)=%s", result,
           QString::fromUtf8(gnome_vfs_result_to_string(result)).local8Bit().data());
    return result == GNOME_VFS_OK;
#else
    return launch(url, mimeTypeForURL(url), window);
#endif
}

///////////////////////////////////////////////////////////////////////////////

bool GNOMEVFSLauncherImpl::launch(const QUrl& url, const QString& mimeType, QWidget* window)
{
    Q_UNUSED(window);
    
    if (!m_initOK) return false;
    if (!url.isValid() || mimeType.isEmpty()) return false;

    qDebug("GNOMEVFSLauncherImpl: URL(%s) mimeType(%s)",
           url.toString().latin1(), mimeType.latin1());
           
    GnomeVFSMimeApplication* default_app =
//        gnome_vfs_mime_get_default_application(mimeType.utf8());
        gnome_vfs_mime_get_default_application_for_uri(url.toString().utf8(), mimeType.utf8());
        
    qDebug("defaultApplication(%s)\n", (default_app == 0 ? "NULL" : default_app->name));
    if (default_app == 0) return false;

    char* uri = strdup(url.toString().utf8());
    
    GList* uri_list = 0;
    uri_list = g_list_append(uri_list, uri);
    
    GnomeVFSResult result = gnome_vfs_mime_application_launch(default_app, uri_list);

    free(uri);
    g_list_free(uri_list);
    gnome_vfs_mime_application_free(default_app);
    
    return result == GNOME_VFS_OK;
}

///////////////////////////////////////////////////////////////////////////////

QString GNOMEVFSLauncherImpl::mimeTypeForURL(const QUrl& url)
{
    GnomeVFSURI* uri = gnome_vfs_uri_new(url.toString().utf8());
    if (uri == 0) return QString::null;
    
    const char* mime_type = gnome_vfs_get_mime_type_from_uri(uri);
    gnome_vfs_uri_unref(uri);

    return (mime_type == 0 ? QString::null : QString::fromUtf8(mime_type));
}

// End of File
