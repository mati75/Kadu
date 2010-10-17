/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QTimer>

#ifdef Q_OS_WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "chat/chat.h"
#include "chat/message/formatted-message.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/password-window.h"
#include "protocols/protocols-manager.h"
#include "qt/long-validator.h"
#include "status/status.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"
#include "url-handlers/url-handler-manager.h"

#include "debug.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "dcc/dcc-manager.h"
#include "helpers/gadu-formatter.h"
#include "server/gadu-contact-list-handler.h"
#include "server/gadu-servers-manager.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "socket-notifiers/gadu-pubdir-socket-notifiers.h"

#include "helpers/gadu-importer.h"
#include "gadu-account-details.h"
#include "gadu-contact-details.h"
#include "gadu-protocol-factory.h"
#ifndef Q_OS_WIN
#include "gadu-resolver.h"
#endif
#include "gadu-url-handler.h"

#include "gadu-protocol.h"

#define GG8_DESCRIPTION_MASK 0x00ff

extern "C" KADU_EXPORT int gadu_protocol_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	if (ProtocolsManager::instance()->hasProtocolFactory("gadu"))
		return 0;

	gg_debug_level = debug_mask | ~255;

	gg_proxy_host = 0;
	gg_proxy_username = 0;
	gg_proxy_password = 0;

#ifndef DEBUG_ENABLED
	gg_debug_level = 1;
#endif
#ifndef Q_OS_WIN
	gg_global_set_custom_resolver(gadu_resolver_start, gadu_resolver_cleanup);
#endif
	ProtocolsManager::instance()->registerProtocolFactory(GaduProtocolFactory::instance());
	UrlHandlerManager::instance()->registerUrlHandler("Gadu", new GaduUrlHandler());

	if (AccountManager::instance()->allItems().isEmpty())
		GaduImporter::instance()->importAccounts();

	GaduImporter::instance()->importContacts();

	return 0;
}

extern "C" KADU_EXPORT void gadu_protocol_close()
{
	UrlHandlerManager::instance()->unregisterUrlHandler("Gadu");
	ProtocolsManager::instance()->unregisterProtocolFactory(GaduProtocolFactory::instance());
}

#define GG_STATUS_INVISIBLE2 0x0009
QString GaduProtocol::statusTypeFromGaduStatus(unsigned int index)
{
	switch (index & GG8_DESCRIPTION_MASK)
	{
		case GG_STATUS_FFC_DESCR:
		case GG_STATUS_FFC:
			return "FreeForChat";

		case GG_STATUS_AVAIL_DESCR:
		case GG_STATUS_AVAIL:
			return "Online";

		case GG_STATUS_BUSY_DESCR:
		case GG_STATUS_BUSY:
			return "Away";

		case GG_STATUS_DND_DESCR:
		case GG_STATUS_DND:
			return "DoNotDisturb";

		case GG_STATUS_INVISIBLE_DESCR:
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE2:
			return "Invisible";

		case GG_STATUS_BLOCKED:
		case GG_STATUS_NOT_AVAIL_DESCR:
		case GG_STATUS_NOT_AVAIL:

		default:
			return "Offline";
	}
}

bool GaduProtocol::isBlockingStatus(unsigned int index)
{
	return GG_STATUS_BLOCKED == index;
}

unsigned int GaduProtocol::gaduStatusFromStatus(const Status &status)
{
	bool hasDescription = !status.description().isEmpty();
	const QString &type = status.type();

	if ("FreeForChat" == type)
		return hasDescription ? GG_STATUS_FFC_DESCR : GG_STATUS_FFC;

	if ("Online" == type)
		return hasDescription ? GG_STATUS_AVAIL_DESCR : GG_STATUS_AVAIL;

	if ("Away" == type || "NotAvailable" == type)
		return hasDescription ? GG_STATUS_BUSY_DESCR : GG_STATUS_BUSY;

	if ("DoNotDisturb" == type)
		return hasDescription ? GG_STATUS_DND_DESCR : GG_STATUS_DND;

	if ("Invisible" == type)
		return hasDescription ? GG_STATUS_INVISIBLE_DESCR : GG_STATUS_INVISIBLE;

	return hasDescription ? GG_STATUS_NOT_AVAIL_DESCR : GG_STATUS_NOT_AVAIL;
}

Buddy GaduProtocol::searchResultToBuddy(gg_pubdir50_t res, int number)
{
	Buddy result = Buddy::create();

	Contact contact = Contact::create();
	contact.setContactAccount(account());
	contact.setOwnerBuddy(result);
	contact.setId(gg_pubdir50_get(res, number, GG_PUBDIR50_UIN));
	contact.setDetails(new GaduContactDetails(contact));

	const char *pubdirStatus = gg_pubdir50_get(res, number, GG_PUBDIR50_STATUS);
	if (pubdirStatus)
	{	Status status;
		status.setType(statusTypeFromGaduStatus(atoi(pubdirStatus) & 127));
		contact.setCurrentStatus(status);
	}

	result.setFirstName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_FIRSTNAME)));
	result.setLastName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_LASTNAME)));
	result.setNickName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_NICKNAME)));
	result.setBirthYear(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_BIRTHYEAR)).toUShort());
	result.setCity(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_CITY)));
	result.setFamilyName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_FAMILYNAME)));
	result.setFamilyCity(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_FAMILYCITY)));
	result.setGender((BuddyGender)QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_GENDER)).toUShort());

	return result;
}

GaduProtocol::GaduProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory), Dcc(0),
		ActiveServer(), GaduLoginParams(), GaduSession(0), PingTimer(0)
{
	kdebugf();

	SocketNotifiers = new GaduProtocolSocketNotifiers(account, this);

	CurrentAvatarService = new GaduAvatarService(account, this);
	connect(this, SIGNAL(connected(Account)),
			this, SLOT(fetchAvatars(Account)));
	CurrentChatImageService = new GaduChatImageService(this);
	CurrentChatService = new GaduChatService(this);
	CurrentContactListService = new GaduContactListService(this);
	CurrentContactPersonalInfoService = new GaduContactPersonalInfoService(this);
	CurrentFileTransferService = new GaduFileTransferService(this);
	CurrentPersonalInfoService = new GaduPersonalInfoService(this);
	CurrentSearchService = new GaduSearchService(this);

	ContactListHandler = 0;

	connect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(buddyUpdated(Buddy &)));
	connect(ContactManager::instance(), SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactReattached(Contact)),
			this, SLOT(contactAttached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactAboutToBeDetached(Contact)),
			this, SLOT(contactAboutToBeDetached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));

	kdebugf2();
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();

	disconnect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(buddyUpdated(Buddy &)));
	disconnect(ContactManager::instance(), SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactReattached(Contact)),
			this, SLOT(contactAttached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeDetached(Contact)),
			this, SLOT(contactAboutToBeDetached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));

	networkDisconnected(false);

	kdebugf2();
}

void GaduProtocol::fetchAvatars(Account account)
{
	foreach (const Contact &contact, ContactManager::instance()->contacts(account))
		CurrentAvatarService->fetchAvatar(contact);
}

bool GaduProtocol::validateUserID(const QString &uid)
{
	LongValidator v(1, 3999999999U, this);
	int pos = 0;

	QString id = uid; // need non-const copy
	if (v.validate(id, pos) == QValidator::Acceptable)
		return true;

	return false;
}

int GaduProtocol::maxDescriptionLength()
{
	return GG_STATUS_DESCR_MAXSIZE;
}

void GaduProtocol::changeStatus()
{
	Status newStatus = nextStatus();
	if (newStatus == status())
		return; // dont reset password

	if (newStatus.isDisconnected() && status().isDisconnected())
	{
		if (NetworkConnecting == state())
			networkDisconnected(false);
		return;
	}

	if (NetworkConnecting == state())
		return;

	if (status().isDisconnected())
	{
		login();
		return;
	}

// TODO 0.6.6: workaround. Find general solution
	if (newStatus.type() == "NotAvailable" && status().type() == "Away")
		return;

// TODO: 0.6.6
	int friends = 0;// GG_STATUS_FRIENDS_MASK; // (!newStatus.isDisconnected() && privateMode() ? GG_STATUS_FRIENDS_MASK : 0);
	int type = gaduStatusFromStatus(newStatus);
	bool hasDescription = !newStatus.description().isEmpty();

	if (hasDescription)
		gg_change_status_descr(GaduSession, type | friends, newStatus.description().toUtf8());
	else
		gg_change_status(GaduSession, type | friends);

	if (newStatus.isDisconnected())
		networkDisconnected(false);

	statusChanged(newStatus);
}

void GaduProtocol::changePrivateMode()
{
	changeStatus();
}

void GaduProtocol::connectionTimeoutTimerSlot()
{
	kdebugf();

	kdebugm(KDEBUG_INFO, "Timeout, breaking connection\n");
	socketConnFailed(ConnectionTimeout);

	kdebugf2();
}

void GaduProtocol::everyMinuteActions()
{
	kdebugf();

	gg_ping(GaduSession);
	CurrentChatImageService->resetSendImageRequests();
}

void GaduProtocol::login(const QString &password, bool permanent)
{
	account().setPassword(password);
	account().setRememberPassword(permanent);
	account().setHasPassword(!password.isEmpty());

	login();
}

void GaduProtocol::login()
{
	kdebugf();

	if (GaduSession)
		return;

	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());

	if (!gaduAccountDetails)
		return;

	if (0 == gaduAccountDetails->uin())
	{
		MessageDialog::msg(tr("UIN not set!"), false, "dialog-warning");
		setStatus(Status());
		kdebugmf(KDEBUG_FUNCTION_END, "end: gadu UIN not set\n");
		return;
	}

	if (!account().hasPassword())
	{
		QString message = tr("Please provide password for %1 (%2) account")
				.arg(account().accountIdentity().name())
				.arg(account().id());
		PasswordWindow::getPassword(message, this, SLOT(login(const QString &, bool)));
		return;
	}

	networkStateChanged(NetworkConnecting);

	setupProxy();
	setupDcc();
	setupLoginParams();

	GaduSession = gg_login(&GaduLoginParams);
	ContactListHandler = new GaduContactListHandler(this);

	cleanUpLoginParams();

	if (GaduSession)
		SocketNotifiers->watchFor(GaduSession);
	else
		networkDisconnected(false);

	kdebugf2();
}

void GaduProtocol::cleanUpProxySettings()
{
	if (gg_proxy_host)
	{
		free(gg_proxy_host);
		gg_proxy_host = 0;
	}

	if (gg_proxy_username)
	{
		free(gg_proxy_username);
		free(gg_proxy_password);
		gg_proxy_username = gg_proxy_password = 0;
	}
}

void GaduProtocol::setupProxy()
{
	kdebugf();

	cleanUpProxySettings();

	AccountProxySettings proxySettings = account().proxySettings();
	gg_proxy_enabled = proxySettings.enabled();
	if (!gg_proxy_enabled)
		return;

	gg_proxy_host = strdup((char *)unicode2latin(proxySettings.address()).data());
	gg_proxy_port = proxySettings.port();

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_host = %s\n", gg_proxy_host);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_port = %d\n", gg_proxy_port);

	if (proxySettings.requiresAuthentication() && !proxySettings.user().isEmpty())
	{
		gg_proxy_username = strdup((char *)unicode2latin(proxySettings.user()).data());
		gg_proxy_password = strdup((char *)unicode2latin(proxySettings.password()).data());
	}
}

void GaduProtocol::setupDcc()
{
	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!gaduAccountDetails)
		return;

	if (gaduAccountDetails->allowDcc())
	{
		if (!Dcc)
			Dcc = new DccManager(this);
	}
	else
	{
		if (Dcc)
		{
			delete Dcc;
			Dcc = 0;
		}
	}
}

void GaduProtocol::setupLoginParams()
{
	memset(&GaduLoginParams, 0, sizeof(GaduLoginParams));

	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!gaduAccountDetails)
		return;

	GaduLoginParams.uin = account().id().toULong();
	GaduLoginParams.password = strdup(account().password().toAscii().data());

	GaduLoginParams.async = 1;
	GaduLoginParams.status = gaduStatusFromStatus(nextStatus()) | GG_STATUS_FRIENDS_MASK; // TODO: 0.6.6 support is friend only
	if (!nextStatus().description().isEmpty())
		GaduLoginParams.status_descr = strdup(nextStatus().description().toUtf8());

	ActiveServer = GaduServersManager::instance()->getServer();
	bool haveServer = !ActiveServer.first.isNull();
	GaduLoginParams.server_addr = haveServer ? htonl(ActiveServer.first.toIPv4Address()) : 0;
	GaduLoginParams.server_port = haveServer ? ActiveServer.second : 0;

	GaduLoginParams.protocol_version = GG_DEFAULT_PROTOCOL_VERSION;
	GaduLoginParams.client_version = (char *)GG_DEFAULT_CLIENT_VERSION;
	GaduLoginParams.protocol_features = GG_FEATURE_DND_FFC; // enable new statuses
	GaduLoginParams.encoding = GG_ENCODING_UTF8;

	GaduLoginParams.has_audio = false; // gaduAccountDetails->allowDcc();
	GaduLoginParams.last_sysmsg = config_file.readNumEntry("General", "SystemMsgIndex", 1389);

	if (Dcc)
		Dcc->setUpExternalAddress(GaduLoginParams);

	GaduLoginParams.tls = 0;
	GaduLoginParams.image_size = gaduAccountDetails->maximumImageSize();
}

void GaduProtocol::cleanUpLoginParams()
{
	memset(GaduLoginParams.password, 0, strlen(GaduLoginParams.password));
	free(GaduLoginParams.password);
	GaduLoginParams.password = 0;

	if (GaduLoginParams.status_descr)
	{
		free(GaduLoginParams.status_descr);
		GaduLoginParams.status_descr = 0;
	}
}

void GaduProtocol::networkConnected()
{
	networkStateChanged(NetworkConnected);
}

void GaduProtocol::networkDisconnected(bool tryAgain)
{
	if (!tryAgain)
		networkStateChanged(NetworkDisconnected);

	if (Dcc)
	{
		delete Dcc;
		Dcc = 0;
	}

	if (PingTimer)
	{
		PingTimer->stop();
		delete PingTimer;
		PingTimer = 0;
	}

	SocketNotifiers->watchFor(0); // stop watching

	if (GaduSession)
	{
		gg_free_session(GaduSession);
		GaduSession = 0;

		delete ContactListHandler;
		ContactListHandler = 0;
	}

	setAllOffline();

	if (tryAgain && !nextStatus().isDisconnected()) // user still wants to login
		QTimer::singleShot(1000, this, SLOT(login())); // try again after one second
	else if (!nextStatus().isDisconnected())
		setStatus(Status());
}

void GaduProtocol::sendUserList()
{
	ContactListHandler->setUpContactList(ContactManager::instance()->contacts(account()));
}

void GaduProtocol::socketContactStatusChanged(unsigned int uin, unsigned int status, const QString &description,
		const QHostAddress &ip, unsigned short port, unsigned int maxImageSize, unsigned int version)
{
	Contact contact = ContactManager::instance()->byId(account(), QString::number(uin));
	Buddy buddy = contact.ownerBuddy();

	if (buddy.isAnonymous())
	{
		kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n", uin);
		emit userStatusChangeIgnored(buddy);
		ContactListHandler->removeContactEntry(uin);
		return;
	}

	contact.setAddress(ip);
	contact.setMaximumImageSize(maxImageSize);
	contact.setPort(port);
	contact.setProtocolVersion(QString::number(version));

	GaduContactDetails *details = gaduContactDetails(contact);
	if (details)
		details->setGaduProtocolVersion(version);

	Status oldStatus = contact.currentStatus();
	Status newStatus;
	newStatus.setType(statusTypeFromGaduStatus(status));
	newStatus.setDescription(description);
	contact.setCurrentStatus(newStatus);
	contact.setBlocking(isBlockingStatus(status));

	emit contactStatusChanged(contact, oldStatus);
}

void GaduProtocol::socketConnFailed(GaduError error)
{
	kdebugf();
	QString msg = QString::null;

	bool tryAgain = true;
	switch (error)
	{
		case ConnectionServerNotFound:
			msg = tr("Unable to connect, server has not been found");
			break;

		case ConnectionCannotConnect:
			msg = tr("Unable to connect");
			break;

		case ConnectionNeedEmail:
			msg = tr("Please change your email in \"Change password / email\" window. "
				"Leave new password field blank.");
			tryAgain = false;
			MessageDialog::msg(msg, false, "dialog-warning");
			break;

		case ConnectionInvalidData:
			msg = tr("Unable to connect, server has returned unknown data");
			break;

		case ConnectionCannotRead:
			msg = tr("Unable to connect, connection break during reading");
			break;

		case ConnectionCannotWrite:
			msg = tr("Unable to connect, connection break during writing");
			break;

		case ConnectionIncorrectPassword:
			msg = tr("Unable to connect, incorrect password");
			tryAgain = false;
			MessageDialog::msg(tr("Connection will be stopped\nYour password is incorrect!"), false, "dialog-error");
			break;

		case ConnectionTlsError:
			msg = tr("Unable to connect, error of negotiation TLS");
			break;

		case ConnectionIntruderError:
			msg = tr("Too many connection attempts with bad password!");
			tryAgain = false;
			MessageDialog::msg(tr("Connection will be stopped\nToo many attempts with bad password"), false, "dialog-error");
			break;

		case ConnectionUnavailableError:
			msg = tr("Unable to connect, servers are down");
			break;

		case ConnectionUnknow:
			msg = tr("Connection broken");
			kdebugm(KDEBUG_INFO, "Connection broken unexpectedly!\nUnscheduled connection termination\n");
			break;

		case ConnectionTimeout:
			msg = tr("Connection timeout!");
			break;

		case Disconnected:
			msg = tr("Disconnection has occured");
			break;

		default:
			kdebugm(KDEBUG_ERROR, "Unhandled error? (%d)\n", int(error));
			msg = tr("Connection broken");
			break;
	}

	if (!msg.isEmpty())
	{
		QHostAddress server = ActiveServer.first;
		QString host;
		if (!server.isNull())
			host = QString("%1:%2").arg(server.toString()).arg(ActiveServer.second);
		else
			host = "HUB";
		kdebugm(KDEBUG_INFO, "%s %s\n", qPrintable(host), qPrintable(msg));
		emit connectionError(account(), host, msg);
	}

	if (tryAgain)
		GaduServersManager::instance()->markServerAsBad(ActiveServer);
	networkDisconnected(tryAgain);

	kdebugf2();
}

void GaduProtocol::socketConnSuccess()
{
	kdebugf();

	sendUserList();

	GaduServersManager::instance()->markServerAsGood(ActiveServer);

	PingTimer = new QTimer(0);
	connect(PingTimer, SIGNAL(timeout()), this, SLOT(everyMinuteActions()));
	PingTimer->start(60000);

	statusChanged(nextStatus());
	networkConnected();

	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(account().details());

	if (details && CurrentContactListService && details->initialRosterImport())
	{
		details->setState(StorableObject::StateNew);
		details->setInitialRosterImport(false);

		CurrentContactListService->importContactList();
	}

	// workaround about servers errors
	if ("Invisible" == status().type())
		setStatus(status());

	kdebugf2();
}

void GaduProtocol::socketDisconnected()
{
	kdebugf();

	networkDisconnected(false);

	kdebugf2();
}

unsigned int GaduProtocol::uin(Contact contact) const
{
	GaduContactDetails *data = gaduContactDetails(contact);
	return data
			? data->uin()
			: 0;
}

GaduContactDetails * GaduProtocol::gaduContactDetails(Contact contact) const
{
	if (contact.isNull())
		return 0;
	return dynamic_cast<GaduContactDetails *>(contact.details());
}

QString GaduProtocol::statusPixmapPath()
{
	return QLatin1String("gadu-gadu");
}

void GaduProtocol::buddyUpdated(Buddy &buddy)
{
	// update offline to and other data
	if (ContactListHandler)
		foreach (Contact contact, buddy.contacts(account()))
			ContactListHandler->updateContactEntry(contact);
}

void GaduProtocol::contactAttached(Contact contact)
{
	if (!ContactListHandler)
		return;

	if (contact.contactAccount() != account())
		return;

	ContactListHandler->addContactEntry(contact);
}

void GaduProtocol::contactAboutToBeDetached(Contact contact)
{
	if (!ContactListHandler)
		return;

	if (contact.contactAccount() != account())
		return;

	ContactListHandler->removeContactEntry(contact);
}

void GaduProtocol::contactIdChanged(Contact contact, const QString &oldId)
{
	if (!ContactListHandler)
		return;

	if (contact.contactAccount() != account())
		return;

	bool ok;
	UinType oldUin = oldId.toInt(&ok);
	if (ok)
		ContactListHandler->removeContactEntry(oldUin);

	ContactListHandler->addContactEntry(contact);
}
