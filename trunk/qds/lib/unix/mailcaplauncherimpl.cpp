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

// Qt includes
#include <qdir.h>
#include <qfileinfo.h>
#include <qprocess.h>
#include <qregexp.h>
#include <qurl.h>

// local includes
#include "mailcap.h"
#include "mailcaplauncherimpl.h"
#include "mimetypes.h"

using namespace QDS;

///////////////////////////////////////////////////////////////////////////////

MailcapLauncherImpl::MailcapLauncherImpl() : m_mailCaps(0), m_mimeTypes(0)
{
    m_mailCaps = new Mailcap();    
    m_mailCaps->addCapabilitiesFromFile(QDir::homeDirPath() + "/.mailcap");
    m_mailCaps->addCapabilitiesFromFile("/etc/mailcap");
    
    m_mimeTypes = new MIMETypes();
    m_mimeTypes->addTypesFromFile(QDir::homeDirPath() + "/.mime.types");
    m_mimeTypes->addTypesFromFile("/etc/mime.types");

    m_targetPattern   = new QRegExp("(%s)|('%s')");
    m_mimeTypePattern = new QRegExp("(%t)|('%t')");
}

///////////////////////////////////////////////////////////////////////////////

MailcapLauncherImpl::~MailcapLauncherImpl()
{
    delete m_mimeTypePattern;
    delete m_targetPattern;
    delete m_mimeTypes;
    delete m_mailCaps;
}

///////////////////////////////////////////////////////////////////////////////

bool MailcapLauncherImpl::launch(const QString& fileName, QWidget* window)
{
    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists() || !fileInfo.isReadable()) return false;

    QString mimeType = mimeTypeByExtension(fileInfo);
    if (mimeType.isEmpty()) return false;
    
    return launch(fileName, mimeType, window);
}

///////////////////////////////////////////////////////////////////////////////

bool MailcapLauncherImpl::launch(const QString& fileName, const QString& mimeType,
                                 QWidget* window)
{
    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists() || !fileInfo.isReadable()) return false;

    if (mimeType.isEmpty()) return launch(fileName, window);

    QString command = bestCommandForMIME(mimeType);
    
    return launchCommand(command, fileInfo.absFilePath());
}

///////////////////////////////////////////////////////////////////////////////

bool MailcapLauncherImpl::launch(const QUrl& url, QWidget* window)
{
    if (!url.isValid()) return false;

    if (url.isLocalFile()) return launch(url.path(), window); 

    if (url.protocol() == "http")
    {
        if (launchWebbrowser(url)) return true;
        
        return launchCommand(bestCommandForMIME("text/html"), url.toString());
    }

    if (url.protocol() == "mailto")
    {
        if (launchWebbrowser(url)) return true;
        
        return launchCommand(bestCommandForMIME("text/html"), url.toString());
    }

    QFileInfo fileInfo(url.path());

    QString mimeType = mimeTypeByExtension(fileInfo);
    if (mimeType.isEmpty()) return false;
    
    return launch(url, mimeType, window);
}

///////////////////////////////////////////////////////////////////////////////

bool MailcapLauncherImpl::launch(const QUrl& url, const QString& mimeType, QWidget* window)
{
    if (!url.isValid()) return false;

    if (url.isLocalFile()) return launch(url.path(), mimeType, window); 

    if (url.protocol() == "http")
    {
        if (launchWebbrowser(url)) return true;

        if (mimeType.isEmpty()) return launch(url, window);
        
        return launchCommand(bestCommandForMIME(mimeType), url.toString());
    }

    if (url.protocol() == "mailto")
    {
        if (launchWebbrowser(url)) return true;
        
        return launchCommand(bestCommandForMIME("text/html"), url.toString());
    }
    
    if (mimeType.isEmpty()) return launch(url, window);
    
    return launchCommand(bestCommandForMIME(mimeType), url.toString());
}

///////////////////////////////////////////////////////////////////////////////

QString MailcapLauncherImpl::mimeTypeByExtension(const QFileInfo& fileInfo)
{
    QStringList mimeTypes = m_mimeTypes->mimeForExtension(fileInfo.extension());
    if (mimeTypes.isEmpty())
        mimeTypes = m_mimeTypes->mimeForExtension(fileInfo.extension(false));

    if (mimeTypes.isEmpty()) return QString::null;

    return mimeTypes[0];
}

///////////////////////////////////////////////////////////////////////////////

QString MailcapLauncherImpl::bestCommandForMIME(const QString& mimeType)
{
    if (mimeType.isEmpty()) return QString::null;

    MailcapEntryList offers = m_mailCaps->mailcapsForMIME(mimeType);
    if (offers.isEmpty()) return QString::null;

    // take first command that has neither needsterminal nor copiousoutput
    // in its options
    QString command;
    MailcapEntryList::const_iterator offerIt    = offers.begin();
    MailcapEntryList::const_iterator offerEndIt = offers.end();
    for (; offerIt != offerEndIt; ++offerIt)
    {
        MailcapEntry entry = *offerIt;

        // check options
        bool optionsOK = true;
        QStringList options = entry.options();
        QStringList::const_iterator it    = options.begin();
        QStringList::const_iterator endIt = options.end();
        for (; it != endIt; ++it)
        {
            if ((*it).find("needsterminal") != -1 || (*it).find("copiousoutput") != -1)
            {
                optionsOK = false;
                break;
            }
        }

        if (optionsOK)
        {
            command = entry.command();
            break;
        }
    }

    replace(command, m_mimeTypePattern, mimeType);
    
    return command;
}

///////////////////////////////////////////////////////////////////////////////

bool MailcapLauncherImpl::launchWebbrowser(const QUrl& url)
{
    if (!url.isValid()) return false;

    // check environment for $BROWSER
    QString browser = QString::fromLocal8Bit(getenv("BROWSER"));
    if (browser.isEmpty()) return false;

    QStringList browserList = QStringList::split(':', browser);    
    
    QStringList::const_iterator it    = browserList.begin();
    QStringList::const_iterator endIt = browserList.end();
    for (; it != endIt; ++it)
    {
        if (launchCommand(*it, url.toString())) return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool MailcapLauncherImpl::launchCommand(const QString& command, const QString& data)
{
    qDebug("MailcapLauncherImpl::launchCommand(%s, %s)", command.latin1(), data.latin1());
    if (command.isEmpty() || data.isEmpty()) return false;

    // should be better, e.g. care for escaped stuff
    QStringList fullCommand = QStringList::split(' ', command);
    
    if (command.find("%s") == -1)
    {
        fullCommand.append(data);
    }
    else
    {
        QStringList::iterator it    = fullCommand.begin();
        QStringList::iterator endIt = fullCommand.end();
        for (; it != endIt; ++it)
        {
            replace(*it, m_targetPattern, data);
        }
    }

    QStringList::const_iterator it    = fullCommand.begin();
    QStringList::const_iterator endIt = fullCommand.end();
    for (uint i = 0; it != endIt; ++it, ++i)
    {
        qDebug("QProcess arg[%d]: %s", i, (*it).latin1());
    }
    
    QProcess* process = new QProcess(fullCommand);
    QObject::connect(process, SIGNAL(processExited()), process, SLOT(deleteLater()));

    return process->start();
}

///////////////////////////////////////////////////////////////////////////////

void MailcapLauncherImpl::replace(QString& text, QRegExp* regex, const QString& replacement)
{
    if (text.isEmpty() || regex == 0) return;

    QString replaced;
    for (int index = text.find("%%"); index != -1; index = text.find("%%"))
    {
        replaced.append(text.left(index).replace(*regex, replacement));
        replaced.append("%%");
        
        text = text.mid(index + 2);
    }
    
    replaced.append(text.replace(*regex, replacement));
    
    text = replaced;
}

// End of File
