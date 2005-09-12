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
#include <qregexp.h>
#include <qtextstream.h>

// local includes
#include "mimetypes.h"

using namespace QDS;

///////////////////////////////////////////////////////////////////////////////
    
void MIMETypes::addTypesFromFile(const QString& fileName)
{
    if (fileName.isEmpty()) return;
    
    static QRegExp regex("\\s");
    
    QFile file(fileName);
    if (!file.exists() || !file.open(IO_ReadOnly)) return;
    
    QTextStream stream(&file);
    QString line;
    for (line = stream.readLine().stripWhiteSpace(); !line.isNull();
         line  = stream.readLine().stripWhiteSpace())
    {
        if (line.isEmpty() || line.startsWith("#")) continue;
        
        QStringList parts = QStringList::split(regex, line);
        if (parts.count() < 2) continue;
        
        QStringList::const_iterator it    = parts.begin();
        QStringList::const_iterator endIt = parts.end();
        
        QString mimeType = *it;
        for (++it; it != endIt; ++it)
        {
            if ((*it).isEmpty()) continue;
            m_mimeTypes[*it].append(mimeType);
        }
    }
}
    
///////////////////////////////////////////////////////////////////////////////
    
QStringList MIMETypes::mimeForExtension(const QString& extension) const
{
    if (!extension.isEmpty()) return m_mimeTypes[extension];
    
    return QStringList();
}
    
// End of File
