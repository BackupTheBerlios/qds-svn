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

#ifndef MAILCAPLAUNCHERIMPL_H
#define MAILCAPLAUNCHERIMPL_H

// plugin includes
#include "plugins/launcherimpl.h"

// forwar declarations
class QFileInfo;
class QRegExp;

namespace QDS
{

// forward declarations
class Mailcap;
class MIMETypes;

class MailcapLauncherImpl : public LauncherImpl
{
public:
    MailcapLauncherImpl();
    virtual ~MailcapLauncherImpl();

    virtual bool launch(const QString& fileName, QWidget* window = 0);

    virtual bool launch(const QString& fileName, const QString& mimeType,
                        QWidget* window = 0);

    virtual bool launch(const QUrl& url, QWidget* window = 0);

    virtual bool launch(const QUrl& url, const QString& mimeType,
                        QWidget* window = 0);

private:
    Mailcap* m_mailCaps;
    MIMETypes* m_mimeTypes;

    QRegExp* m_targetPattern;    
    QRegExp* m_mimeTypePattern;
    
private:
    QString mimeTypeByExtension(const QFileInfo& fileInfo);
    QString bestCommandForMIME(const QString& mimeType);
    
    bool launchWebbrowser(const QUrl& url);
    bool launchCommand(const QString& command, const QString& data);

    void replace(QString& text, QRegExp* regex, const QString& replacement);
};

};

#endif

// End of File
