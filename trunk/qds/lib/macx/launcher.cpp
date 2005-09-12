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

// system includes
#include <Carbon/Carbon.h>

// Qt includes
#include <qfileinfo.h>
#include <qurl.h>
#include <qwidget.h>

// QDS includes
#include "qds/launcher.h"

using namespace QDS;

///////////////////////////////////////////////////////////////////////////////

bool Launcher::launch(const QString& fileName, QWidget* window)
{
    if (fileName.isEmpty()) return false;
    
    QFileInfo info(fileName);
    if (!info.exists()) return false;

    QUrl url;
    url.setProtocol("file");
    url.setPath(info.absFilePath());
    
    return launch(url, window);
}

///////////////////////////////////////////////////////////////////////////////

bool Launcher::launch(const QString& fileName, const QString& mimeType, QWidget* window)
{
    Q_UNUSED(mimeType);
    Q_UNUSED(window);

    return launch(fileName);
}

///////////////////////////////////////////////////////////////////////////////

bool Launcher::launch(const QUrl& url, QWidget* window)
{
    Q_UNUSED(window);

    if (!url.isValid()) return false;
    
    CFStringRef cfstring =
        CFStringCreateWithCString(NULL, url.toString().utf8(), kCFStringEncodingUTF8);

    CFURLRef cfurl = CFURLCreateWithString(NULL, cfstring, NULL);

    OSStatus status = LSOpenCFURLRef(cfurl, NULL);

    CFRelease(cfurl);
    CFRelease(cfstring);
    
    return ((int)status) == 0;
}

///////////////////////////////////////////////////////////////////////////////

bool Launcher::launch(const QUrl& url, const QString& mimeType, QWidget* window)
{
    Q_UNUSED(mimeType);
    Q_UNUSED(window);
    
    return launch(url);
}

///////////////////////////////////////////////////////////////////////////////

Launcher::Launcher() : m_impl(0)
{
}

///////////////////////////////////////////////////////////////////////////////

Launcher::~Launcher()
{
}

// End of File
