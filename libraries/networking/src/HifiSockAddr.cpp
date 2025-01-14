//
//  HifiSockAddr.cpp
//  libraries/networking/src
//
//  Created by Stephen Birarda on 11/26/2013.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <qdatastream.h>
#include <qhostinfo.h>
#include <qnetworkinterface.h>

#include "HifiSockAddr.h"
#include "NetworkLogging.h"

static int hifiSockAddrMetaTypeId = qRegisterMetaType<HifiSockAddr>();

HifiSockAddr::HifiSockAddr() :
    _address(),
    _port(0)
{

}

HifiSockAddr::HifiSockAddr(const QHostAddress& address, quint16 port) :
    _address(address),
    _port(port)
{

}

HifiSockAddr::HifiSockAddr(const HifiSockAddr& otherSockAddr) :
    QObject(),
    _address(otherSockAddr._address),
    _port(otherSockAddr._port)
{
    setObjectName(otherSockAddr.objectName());
}

HifiSockAddr& HifiSockAddr::operator=(const HifiSockAddr& rhsSockAddr) {
    setObjectName(rhsSockAddr.objectName());
    _address = rhsSockAddr._address;
    _port = rhsSockAddr._port;
    return *this;
}

HifiSockAddr::HifiSockAddr(const QString& hostname, quint16 hostOrderPort, bool shouldBlockForLookup) :
    _address(hostname),
    _port(hostOrderPort)
{
    // if we parsed an IPv4 address out of the hostname, don't look it up
    if (_address.protocol() != QAbstractSocket::IPv4Protocol) {
        // lookup the IP by the hostname
        if (shouldBlockForLookup) {
            qCDebug(networking) << "Synchronously looking up IP address for hostname" << hostname;
            QHostInfo result = QHostInfo::fromName(hostname);
            handleLookupResult(result);
        } else {
            int lookupID = QHostInfo::lookupHost(hostname, this, SLOT(handleLookupResult(QHostInfo)));
            qCDebug(networking) << "Asynchronously looking up IP address for hostname" << hostname << "- lookup ID is" << lookupID;
        }
    }
}

HifiSockAddr::HifiSockAddr(const sockaddr* sockaddr) {
    _address = QHostAddress(sockaddr);

    if (sockaddr->sa_family == AF_INET) {
        _port = ntohs(reinterpret_cast<const sockaddr_in*>(sockaddr)->sin_port);
    } else {
        _port = ntohs(reinterpret_cast<const sockaddr_in6*>(sockaddr)->sin6_port);
    }
}

void HifiSockAddr::swap(HifiSockAddr& otherSockAddr) {
    using std::swap;
    
    swap(_address, otherSockAddr._address);
    swap(_port, otherSockAddr._port);
    
    // Swap objects name
    auto temp = otherSockAddr.objectName();
    otherSockAddr.setObjectName(objectName());
    setObjectName(temp);
}

bool HifiSockAddr::operator==(const HifiSockAddr& rhsSockAddr) const {
    return _address == rhsSockAddr._address && _port == rhsSockAddr._port;
}

void HifiSockAddr::handleLookupResult(const QHostInfo& hostInfo) {
    if (hostInfo.error() != QHostInfo::NoError) {
        qCDebug(networking) << "Lookup failed for" << hostInfo.lookupId() << ":" << hostInfo.errorString();
        emit lookupFailed();
    } else {
        foreach(const QHostAddress& address, hostInfo.addresses()) {
            // just take the first IPv4 address
            if (address.protocol() == QAbstractSocket::IPv4Protocol) {
                _address = address;
                qCDebug(networking) << "QHostInfo lookup result for"
                    << hostInfo.hostName() << "with lookup ID" << hostInfo.lookupId() << "is" << address.toString();
                emit lookupCompleted();
                break;
            }
        }
    }
}

QDebug operator<<(QDebug debug, const HifiSockAddr& sockAddr) {
    debug.nospace() << sockAddr._address.toString().toLocal8Bit().constData() << ":" << sockAddr._port;
    return debug.space();
}

QDataStream& operator<<(QDataStream& dataStream, const HifiSockAddr& sockAddr) {
    dataStream << sockAddr._address << sockAddr._port;
    return dataStream;
}

QDataStream& operator>>(QDataStream& dataStream, HifiSockAddr& sockAddr) {
    dataStream >> sockAddr._address >> sockAddr._port;
    return dataStream;
}

QHostAddress getLocalAddress() {

    QHostAddress localAddress;

    foreach(const QNetworkInterface &networkInterface, QNetworkInterface::allInterfaces()) {
        if (networkInterface.flags() & QNetworkInterface::IsUp
            && networkInterface.flags() & QNetworkInterface::IsRunning
            && networkInterface.flags() & ~QNetworkInterface::IsLoopBack) {
            // we've decided that this is the active NIC
            // enumerate it's addresses to grab the IPv4 address
            foreach(const QNetworkAddressEntry &entry, networkInterface.addressEntries()) {
                // make sure it's an IPv4 address that isn't the loopback
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol && !entry.ip().isLoopback()) {

                    // set our localAddress and break out
                    localAddress = entry.ip();
                    break;
                }
            }
        }

        if (!localAddress.isNull()) {
            break;
        }
    }

    // return the looked up local address
    return localAddress;
}

uint qHash(const HifiSockAddr& key, uint seed) {
    // use the existing QHostAddress and quint16 hash functions to get our hash
    return qHash(key.getAddress(), seed) ^ qHash(key.getPort(), seed);
}
