/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_CONTAINER_MANAGER_H
#define STATUS_CONTAINER_MANAGER_H

#include "accounts/accounts-aware-object.h"
#include "configuration/configuration-aware-object.h"
#include "identities/identities-aware-object.h"
#include "status/status-container.h"

class AllAccountsStatusContainer;

class KADUAPI StatusContainerManager : public StatusContainer,
		public ConfigurationAwareObject, private AccountsAwareObject, private IdentitiesAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(StatusContainerManager)

	static StatusContainerManager *Instance;

	bool AllowSetDefaultStatus;

	StatusContainerManager();
	virtual ~StatusContainerManager();

	void init();

	QList<StatusContainer *> StatusContainers;
	StatusContainer *DefaultStatusContainer;
	AllAccountsStatusContainer *AllAccountsContainer;

	QString StartupStatus;
	QString StartupDescription;
	bool StartupLastDescription;
	bool OfflineToInvisible;

	void cleanStatusContainers();
	void addAllAccounts();
	void addAllIdentities();

	void setDefaultStatusContainer(StatusContainer *defaultStatusContainer);

	void registerStatusContainer(StatusContainer *statusContainer);
	void unregisterStatusContainer(StatusContainer *statusContainer);

private slots:
	void updateIdentities();
	void setStatusModeChanged();

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

	virtual void identityAdded(Identity identity);
	virtual void identityRemoved(Identity identity);

	virtual void configurationUpdated();

public:
	static StatusContainerManager * instance();

	unsigned int count() const { return StatusContainers.count(); }
	const QList<StatusContainer *> & statusContainers() const { return StatusContainers; }
	//StatusContainer Implementation
	virtual QString statusContainerName();

	// TODO: rethink it, it is needed by status-change action in main window
	StatusContainer * defaultStatusContainer() const { return DefaultStatusContainer; }

	virtual void setStatus(Status newStatus, bool flush = true);
	virtual void setDescription(const QString &description, bool flush = true);
	virtual Status status();
	virtual bool isStatusSettingInProgress();

	bool allStatusEqual(StatusType *type);

	virtual QString statusDisplayName();

	virtual KaduIcon statusIcon();
	virtual KaduIcon statusIcon(const Status &status);
	virtual KaduIcon statusIcon(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	virtual int maxDescriptionLength();

	virtual QString statusNamePrefix();

	virtual void setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
				      const QString &startupDescription, bool startupLastDescription)
	{
		Q_UNUSED(startupStatus)
		Q_UNUSED(offlineToInvisible)
		Q_UNUSED(startupDescription)
		Q_UNUSED(startupLastDescription)
	}

	virtual void storeStatus(Status status);

	void setAllowSetDefaultStatus(bool allowSetDefaultStatus);

signals:
	void statusContainerAboutToBeRegistered(StatusContainer *);
	void statusContainerRegistered(StatusContainer *);
	void statusContainerAboutToBeUnregistered(StatusContainer *);
	void statusContainerUnregistered(StatusContainer *);

};

#endif // STATUS_CONTAINER_MANAGER_H
