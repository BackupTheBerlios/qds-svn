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
#include <qfile.h>

// local includes
#include "mailcap.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

namespace QDS
{

class MailcapEntryPrivate
{
public:
    MailcapEntryPrivate() : refCount(1) {}
    
    uint refCount;
    
    QString mimeType;
    QString command;
    QStringList options;
};

};

using namespace QDS;

///////////////////////////////////////////////////////////////////////////////

MailcapEntry::MailcapEntry() : m_private(0)
{
}

///////////////////////////////////////////////////////////////////////////////

MailcapEntry::MailcapEntry(const MailcapEntry& other) : m_private(0)
{
    m_private = other.m_private;
    if (m_private != 0) m_private->refCount++;
}

///////////////////////////////////////////////////////////////////////////////

MailcapEntry::MailcapEntry(const QString& mimeType, const QString& command,
                 const QStringList& options) : m_private(new MailcapEntryPrivate())
{
    m_private->mimeType = mimeType;
    m_private->command  = command;
    m_private->options  = options;
}

///////////////////////////////////////////////////////////////////////////////

MailcapEntry::~MailcapEntry()
{
    if (isNull()) return;
    
    detach();
}

///////////////////////////////////////////////////////////////////////////////

MailcapEntry& MailcapEntry::operator=(const MailcapEntry& other)
{
    if (&other == this) return *this;
    
    detach();
        
    m_private = other.m_private;
    if (!isNull()) m_private->refCount++;
    
    return *this;
}

///////////////////////////////////////////////////////////////////////////////

QString MailcapEntry::mimeType() const
{
    if (isNull()) return QString::null;
    
    return m_private->mimeType;
}

///////////////////////////////////////////////////////////////////////////////

QString MailcapEntry::command() const
{
    if (isNull()) return QString::null;
    
    return m_private->command;
}

///////////////////////////////////////////////////////////////////////////////

QStringList MailcapEntry::options() const
{
    if (isNull()) return QStringList();
    
    return m_private->options;
}

///////////////////////////////////////////////////////////////////////////////

void MailcapEntry::detach()
{
    if (!isNull())
    {
        m_private->refCount--;
        if (m_private->refCount == 0) delete m_private;
    }
    m_private = 0;    
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
    
void Mailcap::addCapabilitiesFromFile(const QString& fileName)
{
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.exists() || !file.open(IO_ReadOnly)) return;
    
    QString record;
    
    QTextStream stream(&file);
    QString line;
    bool multiline = false;
    for (line = stream.readLine().stripWhiteSpace(); !line.isNull();
         line  = stream.readLine().stripWhiteSpace())
    {
        if (line.isEmpty() || line.startsWith("#")) continue;
        
        if (line.endsWith("\\"))
        {
            record.append(line.left(line.length() - 1));
            multiline = true;
        }
        else
        {
            record.append(line);
            
            parseRecord(record);
            
            multiline = false;
        }
        
        if (!multiline) record = QString::null;
    }
    
    if (!record.isEmpty()) parseRecord(record);
}

///////////////////////////////////////////////////////////////////////////////

MailcapEntryList Mailcap::mailcapsForMIME(const QString& mimeType) const
{
    if (!mimeType.isEmpty())
    {
        QMap<QString, MailcapEntryList>::const_iterator it = m_mailcaps.find(mimeType);
        if (it == m_mailcaps.end())
        {
            int slashIndex = mimeType.find('/');
            if (slashIndex > 0) it = m_mailcaps.find(mimeType.left(slashIndex));
            
            if (it == m_mailcaps.end()) it = m_mailcaps.find("unknown");

            if (it != m_mailcaps.end()) return it.data();
        }
        else
            return it.data();
    }
    
    return MailcapEntryList();
}

///////////////////////////////////////////////////////////////////////////////
    
void Mailcap::parseRecord(const QString& record)
{
    QStringList split = QStringList::split(';', record);
    
    QString mimeType;
    QString command;
    QStringList options;
    
    uint count = 0;
    QString part;
    
    QStringList::const_iterator it    = split.begin();
    QStringList::const_iterator endIt = split.end();
    for (; it != endIt; ++it)
    {
        part.append((*it).stripWhiteSpace());
        if (part.endsWith("\\")) // escaped ';'
        {
            part.append(";");
        }
        else
        {
            switch (count)
            {
                case 0:
                    mimeType = part;
                    if (mimeType.find('*') != -1)
                    {
                        int slashIndex = mimeType.find('/');
                        if (slashIndex != -1) mimeType = mimeType.left(slashIndex);

                        if (mimeType.find('*') != -1) mimeType = "unknown";
                    }
                    break;

                case 1:
                    command = part;
                    break;

                default:
                    options.append(part);
                    break;
            }

            part = QString::null;
            ++count;
        }
    }

    if (!part.isEmpty()) options.append(part.left(part.length() - 1));

    if (mimeType.isEmpty() || command.isEmpty()) return;

    m_mailcaps[mimeType].append(MailcapEntry(mimeType, command, options));
}

// End of File
