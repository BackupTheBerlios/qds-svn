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
#include <windows.h>
#include <shellapi.h>

// Qt includes
#include <qurl.h>
#include <qwidget.h>

// QDS includes
#include "qds/launcher.h"

using namespace QDS;

///////////////////////////////////////////////////////////////////////////////

bool Launcher::launch(const QString& fileName, QWidget* window)
{
    int result = (int)(ShellExecute((window == 0 ? 0 : window->winId()),
                                    L"open", (wchar_t*)fileName.ucs2(),
                                    NULL, NULL, 0));
    return result > 32;
}

///////////////////////////////////////////////////////////////////////////////

bool Launcher::launch(const QString& fileName, const QString& mimeType, QWidget* window)
{
    Q_UNUSED(mimeType)
    return launch(fileName, window);
}

///////////////////////////////////////////////////////////////////////////////

bool Launcher::launch(const QUrl& url, QWidget* window)
{
    return launch(url.toString(), window);
}

///////////////////////////////////////////////////////////////////////////////

bool Launcher::launch(const QUrl& url, const QString& mimeType, QWidget* window)
{
    return launch(url.toString(), mimeType, window);
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
