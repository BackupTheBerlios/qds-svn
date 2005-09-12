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

#ifndef KNETWORKPROTOCOL_H
#define KNETWORKPROTOCOL_H

// Qt includes
#include <qnetworkprotocol.h>
#include <qptrdict.h>

// KIO includes
#include <kio/global.h>

// forward declarations
namespace KIO {
    class Job;
};

class KURL;
class QUrl;
class QUrlInfo;
class TransferInfo;

class KNetworkProtocol : public QNetworkProtocol
{
    Q_OBJECT
public:
    KNetworkProtocol(const QString& protocol);
    virtual ~KNetworkProtocol();

    int supportedOperations() const;

protected:
    void operationListChildren(QNetworkOperation* op);
    void operationMkDir(QNetworkOperation* op);
    void operationRemove(QNetworkOperation* op);
    void operationRename(QNetworkOperation* op);
    void operationGet(QNetworkOperation* op);
    void operationPut(QNetworkOperation* op);
    bool checkConnection(QNetworkOperation* op);

private:
    void connectJob(KIO::Job* job);
    void disconnectJob(KIO::Job* job);

    KURL convertURL(QUrl* url);
    QUrlInfo convertUDSEntry(const KIO::UDSEntry& entry);

private slots:
    void slotResult(KIO::Job* job);
    void slotConnected(KIO::Job* job);
    void slotTotalSize(KIO::Job*, KIO::filesize_t);
    void slotProcessedSize(KIO::Job*, KIO::filesize_t);
    void slotEntries(KIO::Job* job, const KIO::UDSEntryList& entries);
    void slotData(KIO::Job* job, const QByteArray& buffer);
    void slotRequestData(KIO::Job* job, QByteArray& data);

    void slotInfoMessage(KIO::Job* job, const QString& message);

private:
    bool m_isLocalFS;
    int m_supportedOperations;

    QPtrDict<QNetworkOperation> m_operations;
    QPtrDict<TransferInfo> m_transfers;
};

///////////////////////////////////////////////////////////////////////////////

class KNetworkProtocolFactory : public QNetworkProtocolFactoryBase
{
public:
    KNetworkProtocolFactory(const QString& protocol) : m_protocol(protocol) {}
    virtual ~KNetworkProtocolFactory() {}

    QNetworkProtocol* createObject() {
        return new KNetworkProtocol(m_protocol);
    }

private:
    QString m_protocol;
};

#endif

// end of file
