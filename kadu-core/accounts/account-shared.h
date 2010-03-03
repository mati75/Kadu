/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ACCOUNT_SHARED_H
#define ACCOUNT_SHARED_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QUuid>
#include <QtNetwork/QHostAddress>

#include "contacts/contact.h"
#include "identities/identity.h"
#include "protocols/protocols-aware-object.h"
#include "status/base-status-container.h"
#include "storage/details-holder.h"
#include "storage/shared.h"

class AccountDetails;
class AccountManager;
class Buddy;
class Protocol;
class ProtocolFactory;
class StatusType;

class KADUAPI AccountShared : public BaseStatusContainer, public Shared, public DetailsHolder<AccountShared, AccountDetails, AccountManager>, ProtocolsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(AccountShared)

private:
	QString ProtocolName;
	Protocol *ProtocolHandler;

	Identity AccountIdentity;
	Contact AccountContact;

	QString Id;

	bool RememberPassword;
	bool HasPassword;
	QString Password;

	bool ConnectAtStart;

	bool UseProxy;
	QHostAddress ProxyHost;
	short int ProxyPort;
	bool ProxyRequiresAuthentication;
	QString ProxyUser;
	QString ProxyPassword;

	short int MaximumImageSize;

	void useProtocolFactory(ProtocolFactory *factory);

protected:
	virtual void load();
	void emitUpdated();

	virtual void detailsAdded();
	virtual void detailsAboutToBeRemoved();
	virtual void protocolUnregistered(ProtocolFactory *protocolHandler);

	virtual void doSetStatus(Status newStatus);

public: //TODO 0.6.6: it is needed in Buddy::dummy()
	virtual void protocolRegistered(ProtocolFactory *protocolHandler);

	static AccountShared * loadFromStorage(StoragePoint *storagePoint);

	explicit AccountShared(QUuid uuid = QUuid());
	virtual ~AccountShared();
	
	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void store();
	virtual void aboutToBeRemoved();

	void setProtocolName(QString protocolName);
	void setId(const QString &id);

	Contact accountContact();

	KaduShared_Property(Identity, accountIdentity, AccountIdentity)
	KaduShared_PropertyRead(QString, protocolName, ProtocolName)
	KaduShared_Property(Protocol *, protocolHandler, ProtocolHandler)
	KaduShared_PropertyRead(QString, id, Id)
	KaduShared_Property(bool, rememberPassword, RememberPassword)
	KaduShared_Property(bool, hasPassword, HasPassword)
	KaduShared_Property(QString, password, Password)
	KaduShared_Property(bool, connectAtStart, ConnectAtStart)
	KaduShared_Property(bool, useProxy, UseProxy)
	KaduShared_Property(QHostAddress, proxyHost, ProxyHost)
	KaduShared_Property(short int, proxyPort, ProxyPort)
	KaduShared_Property(bool, proxyRequiresAuthentication, ProxyRequiresAuthentication)
	KaduShared_Property(QString, proxyUser, ProxyUser)
	KaduShared_Property(QString, proxyPassword, ProxyPassword)

	// StatusContainer implementation

	virtual QString statusContainerName();

	virtual Status status();
	virtual int maxDescriptionLength();

	virtual QString statusName();
	virtual QPixmap statusPixmap();
	virtual QPixmap statusPixmap(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	QPixmap statusPixmap(Status status);

	virtual void setPrivateStatus(bool isPrivate);

signals:
	void buddyStatusChanged(Contact contact, Status oldStatus);
	void protocolLoaded();
	void protocolUnloaded();

	void connected();
	void disconnected();

	void updated();

};

#include "buddies/buddy.h" // for MOC

#endif // ACCOUNT_SHARED_H
