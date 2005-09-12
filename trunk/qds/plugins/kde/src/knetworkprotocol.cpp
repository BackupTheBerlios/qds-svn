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

// system includes
#include <sys/stat.h>

// Qt includes
#include <qurloperator.h>

// KDE includes
#include <kdebug.h>
#include <kio/job.h>
#include <kprotocolinfo.h>

// local includes
#include "knetworkprotocol.h"

///////////////////////////////////////////////////////////////////////////////

class TransferInfo
{
public:
    TransferInfo() : totalSize(0), offset(0) {}

    ulong totalSize;
    ulong offset;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

KNetworkProtocol::KNetworkProtocol(const QString& protocol)
    : m_isLocalFS(false),
      m_supportedOperations(0)
{
    if (!protocol.isEmpty())
    {
        m_isLocalFS = (protocol == "file");

        KURL url;

        url.setProtocol(protocol);

        if (KProtocolInfo::supportsListing(url))
            m_supportedOperations |= OpListChildren;
        if (KProtocolInfo::supportsMakeDir(url))
            m_supportedOperations |= OpMkDir;
        if (KProtocolInfo::supportsDeleting(url))
            m_supportedOperations |= OpRemove;
        if (KProtocolInfo::supportsMoving(url))
            m_supportedOperations |= OpRename;
        if (KProtocolInfo::supportsReading(url))
            m_supportedOperations |= OpGet;
        if (KProtocolInfo::supportsWriting(url))
            m_supportedOperations |= OpPut;
    }
    
    m_transfers.setAutoDelete(true);
}

///////////////////////////////////////////////////////////////////////////////

KNetworkProtocol::~KNetworkProtocol()
{
}

///////////////////////////////////////////////////////////////////////////////

int KNetworkProtocol::supportedOperations() const
{
    return m_supportedOperations;
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::operationListChildren(QNetworkOperation* op)
{
    KURL kurl = convertURL(url());

    KIO::Job* job = KIO::listDir(kurl, !m_isLocalFS);
    if (job == 0)
    {
        op->setState(StFailed);
        emit finished(op);
        return;
    }

    m_operations.insert(job, op);
    connectJob(job);

    op->setState(StInProgress);
    emit start(op);
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::operationMkDir(QNetworkOperation* op)
{
    KURL kurl = convertURL(url());
    kurl = KURL(kurl.prettyURL() + "/" + op->arg(0));

    KIO::Job* job = KIO::mkdir(kurl);
    if (job == 0)
    {
        op->setState(StFailed);
        emit finished(op);
        return;
    }

    m_operations.insert(job, op);
    connectJob(job);

    op->setState(StInProgress);
    emit start(op);
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::operationRemove(QNetworkOperation* op)
{
    KURL kurl = convertURL(url());
    kurl = KURL(kurl.prettyURL() + "/" + op->arg(0));

    KIO::Job* job = KIO::del(kurl);
    if (job == 0)
    {
        op->setState(StFailed);
        emit finished(op);
        return;
    }

    m_operations.insert(job, op);
    connectJob(job);

    op->setState(StInProgress);
    emit start(op);
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::operationRename(QNetworkOperation* op)
{
    KURL kurl = convertURL(url());
    KURL fromURL = KURL(kurl.prettyURL() + "/" + op->arg(0));
    KURL toURL   = KURL(kurl.prettyURL() + "/" + op->arg(1));

    KIO::Job* job = KIO::rename(fromURL, toURL, false);
    if (job == 0)
    {
        op->setState(StFailed);
        emit finished(op);
        return;
    }

    m_operations.insert(job, op);
    connectJob(job);

    op->setState(StInProgress);
    emit start(op);
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::operationGet(QNetworkOperation* op)
{
    KURL kurl = KURL(op->arg(0));
    
    KIO::Job* job = KIO::get(kurl);
    if (job == 0)
    {
        op->setState(StFailed);
        emit finished(op);
        return;
    }

    m_operations.insert(job, op);
    m_transfers.insert(op, new TransferInfo());
    connectJob(job);

    op->setState(StInProgress);
    emit start(op);
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::operationPut(QNetworkOperation* op)
{
    KURL kurl = KURL(op->arg(0));

    KIO::Job* job = KIO::put(kurl, -1, false, false);
    if (job == 0)
    {
        op->setState(StFailed);
        emit finished(op);
        return;
    }

    m_operations.insert(job, op);
    TransferInfo* info = new TransferInfo();
    info->totalSize = op->rawArg(1).size();

    m_transfers.insert(op, info);
    connectJob(job);

    op->setState(StInProgress);
    emit start(op);
}

///////////////////////////////////////////////////////////////////////////////

bool KNetworkProtocol::checkConnection(QNetworkOperation* op)
{
    Q_UNUSED(op)

    return true;
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::connectJob(KIO::Job* job)
{
    if (job == 0) return;

    QObject::connect(job, SIGNAL(result(KIO::Job*)),
                     this, SLOT(slotResult(KIO::Job*)));
    QObject::connect(job, SIGNAL(connected(KIO::Job*)),
                     this, SLOT(slotConnected(KIO::Job*)));
    if (job->inherits("KIO::ListJob"))
    {
        QObject::connect(job, SIGNAL(entries(KIO::Job*,
                                     const KIO::UDSEntryList&)),
                         this, SLOT(slotEntries(KIO::Job*,
                                    const KIO::UDSEntryList&)));
    }
    if (job->inherits("KIO::TransferJob"))
    {
        QObject::connect(job,
                         SIGNAL(totalSize(KIO::Job*, KIO::filesize_t)),
                         this,
                         SLOT(slotTotalSize(KIO::Job*, KIO::filesize_t)));
        QObject::connect(job,
                         SIGNAL(processedSize(KIO::Job*, KIO::filesize_t)),
                         this,
                         SLOT(slotProcessedSize(KIO::Job*, KIO::filesize_t)));
        QObject::connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
                         this, SLOT(slotData(KIO::Job*, const QByteArray&)));
        QObject::connect(job, SIGNAL(dataReq(KIO::Job*, QByteArray&)),
                         this,
                         SLOT(slotRequestData(KIO::Job*, QByteArray&)));
    }
    QObject::connect(job, SIGNAL(infoMessage(KIO::Job*, const QString&)),
                     this, SLOT(slotInfoMessage(KIO::Job*, const QString&)));
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::disconnectJob(KIO::Job* job)
{
    if (job == 0) return;

    QObject::disconnect(job, SIGNAL(result(KIO::Job*)),
                        this, SLOT(slotResult(KIO::Job*)));
    QObject::disconnect(job, SIGNAL(connected(KIO::Job*)),
                        this, SLOT(slotConnected(KIO::Job*)));
    if (job->inherits("KIO::ListJob"))
    {
        QObject::disconnect(job, SIGNAL(entries(KIO::Job*,
                                        const KIO::UDSEntryList&)),
                            this, SLOT(slotEntries(KIO::Job*,
                                       const KIO::UDSEntryList&)));
    }
    if (job->inherits("KIO::TransferJob"))
    {
        QObject::disconnect(job,
                         SIGNAL(totalSize(KIO::Job*, KIO::filesize_t)),
                         this,
                         SLOT(slotTotalSize(KIO::Job*, KIO::filesize_t)));
        QObject::disconnect(job,
                         SIGNAL(processedSize(KIO::Job*, KIO::filesize_t)),
                         this,
                         SLOT(slotProcessedSize(KIO::Job*, KIO::filesize_t)));
        QObject::disconnect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
                            this, SLOT(slotData(KIO::Job*, const QByteArray&)));
        QObject::disconnect(job, SIGNAL(dataReq(KIO::Job*, QByteArray&)),
                            this, SLOT(slotRequestData(KIO::Job*,
                                                       QByteArray&)));
    }
    QObject::disconnect(job, SIGNAL(infoMessage(KIO::Job*, const QString&)),
                        this, SLOT(slotInfoMessage(KIO::Job*, const QString&)));
}

///////////////////////////////////////////////////////////////////////////////

KURL KNetworkProtocol::convertURL(QUrl* url)
{
    KURL kurl(*url);
    if (!url->hasPort()) kurl.setPort(0);

    return kurl;
}

///////////////////////////////////////////////////////////////////////////////

QUrlInfo KNetworkProtocol::convertUDSEntry(const KIO::UDSEntry& entry)
{
    QString name;
    QString owner;
    QString group;
    QString link;
    uint size = 0;
    uint permissions = 0;
    uint fileType;

    KIO::UDSEntry::const_iterator it    = entry.begin();
    KIO::UDSEntry::const_iterator endIt = entry.end();
    for(; it != endIt; ++it)
    {
        KIO::UDSAtom atom = *it;
        switch (atom.m_uds)
        {
            case KIO::UDS_NAME:
                name = atom.m_str;
                break;
            case KIO::UDS_USER:
                owner = atom.m_str;
                break;
            case KIO::UDS_GROUP:
                group = atom.m_str;
                break;
            case KIO::UDS_SIZE:
                size = static_cast<uint>(atom.m_long);
                break;
            case KIO::UDS_ACCESS:
                permissions = static_cast<uint>(atom.m_long);
                break;
            case KIO::UDS_FILE_TYPE:
                fileType = static_cast<uint>(atom.m_long);
                break;
            case KIO::UDS_LINK_DEST:
                link = atom.m_str;
                break;

            default:
                break;
        }
    }

    QUrlInfo info;
    info.setName(name);
    info.setOwner(owner);
    info.setGroup(group);
    info.setSize(size);
    info.setPermissions(permissions);
    if (!link.isEmpty())
        info.setSymLink(true);
    if (S_ISDIR(fileType))
        info.setDir(true);
    info.setFile(!info.isDir());

    return info;
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::slotResult(KIO::Job* job)
{
    //kdDebug() << "KNetworkProtocol::slotResult" << endl;
    if (job == 0) return;

    QNetworkOperation* op = m_operations.take(job);
    disconnectJob(job);

    TransferInfo* info = m_transfers.take(op);
    delete info;

    if (job->error() == 0)
    {
        op->setState(StDone);
        switch (op->operation())
        {
            case OpMkDir:
                {
                    QUrlInfo dir(op->arg(0), 0, "", "", 0, QDateTime(),
                                 QDateTime(), true, false, false, true, true,
                                 true);

                    emit newChild(dir, op);
                    emit createdDirectory(dir, op);
                }
                break;
            case OpRemove:
                emit removed(op);
                break;
            case OpRename:
                emit itemChanged(op);
                break;

            default:
                break;
        }
    }
    else
    {
        op->setState(StFailed);
        op->setProtocolDetail(job->errorString());
    }

    emit finished(op);
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::slotConnected(KIO::Job* job)
{
    Q_UNUSED(job)

    //kdDebug() << "KNetworkProtocol::slotConnected" << endl;
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::slotTotalSize(KIO::Job* job, KIO::filesize_t size)
{
    //kdDebug() << "KNetworkProtocol::slotTotalSize: size="
    //          << static_cast<int>(size) << endl;
    
    QNetworkOperation* op = m_operations.find(job);
    if (op == 0) return;

    TransferInfo* info = m_transfers.find(op);
    if (info == 0) return;

    info->totalSize = static_cast<long>(size);
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::slotProcessedSize(KIO::Job* job, KIO::filesize_t size)
{
    //kdDebug() << "KNetworkProtocol::slotProcessedSize: size="
    //          << static_cast<int>(size) << endl;
    
    QNetworkOperation* op = m_operations.find(job);
    if (op == 0) return;

    TransferInfo* info = m_transfers.find(op);
    if (info == 0) return;

    info->offset = static_cast<long>(size);

    emit dataTransferProgress(info->offset, info->totalSize, op);
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::slotEntries(KIO::Job* job, const KIO::UDSEntryList& entries)
{
    QNetworkOperation* op = m_operations.find(job);
    if (op == 0) return;

    QValueList<QUrlInfo> result;

    KIO::UDSEntryList::const_iterator it    = entries.begin();
    KIO::UDSEntryList::const_iterator endIt = entries.end();
    for(; it != endIt; ++it)
    {
        result.append(convertUDSEntry(*it));
    }

    emit newChildren(result, op);
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::slotData(KIO::Job* job, const QByteArray& buffer)
{
    QNetworkOperation* op = m_operations.find(job);
    if (op == 0) return;

    emit data(buffer, op);
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::slotRequestData(KIO::Job* job, QByteArray& data)
{
    QNetworkOperation* op = m_operations.find(job);
    if (op == 0) return;

    TransferInfo* info = m_transfers.find(op);
    if (info == 0) return;

    uint blockEnd = info->offset + 5120; // 5KB blocks
    if (blockEnd > info->totalSize) blockEnd = info->totalSize;

    data.resize(blockEnd - info->offset);
    QByteArray putData = op->rawArg(1);

    for (uint i = 0; info->offset < blockEnd; ++info->offset, ++i)
    {
        data[i] = putData[info->offset];
    }
}

///////////////////////////////////////////////////////////////////////////////

void KNetworkProtocol::slotInfoMessage(KIO::Job* job, const QString& message)
{
    Q_UNUSED(job)

    //kdDebug() << "KNetworkProtocol::slotInfoMessage: " << message << endl;
}

// end of file
