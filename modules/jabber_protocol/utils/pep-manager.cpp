/*
 * pepmanager.cpp - Classes for PEP
 * Copyright (C) 2006  Remko Troncon
 *
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#include <QtCore/QtDebug>

#include "iris/xmpp_tasks.h"
#include "iris/xmpp_xmlcommon.h"
#include "tasks/pep-get-task.h"
#include "tasks/pep-publish-task.h"
#include "utils/server-info-manager.h"

#include "pep-manager.h"

// TODO: Get affiliations upon startup, and only create nodes based on that.
// (subscriptions is not accurate, since one doesn't subscribe to the
// avatar data node)

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------

class PEPRetractTask : public XMPP::Task
{
public:
	PEPRetractTask(Task* parent, const QString& node, const QString& itemId) : XMPP::Task(parent), node_(node), itemId_(itemId) {
		iq_ = createIQ(doc(), "set", QString(), id());

		QDomElement pubsub = doc()->createElement("pubsub");
		pubsub.setAttribute("xmlns", "http://jabber.org/protocol/pubsub");
		iq_.appendChild(pubsub);

		QDomElement retract = doc()->createElement("retract");
		retract.setAttribute("node", node);
		retract.setAttribute("notify", "1");
		pubsub.appendChild(retract);

		QDomElement item = doc()->createElement("item");
		item.setAttribute("id", itemId);
		retract.appendChild(item);
	}

	bool take(const QDomElement& x) {
		if(!iqVerify(x, QString(), id()))
			return false;

		if(x.attribute("type") == "result") {
			setSuccess();
		}
		else {
			setError(x);
		}
		return true;
	}

	void onGo() {
		send(iq_);
	}

	const QString& node() const {
		return node_;
	}

private:
	QDomElement iq_;
	QString node_;
	QString itemId_;
};

PEPManager::PEPManager(XMPP::Client *client, ServerInfoManager *serverInfo, QObject *parent) :
		QObject(parent), client_(client), serverInfo_(serverInfo)
{
	connect(client_, SIGNAL(messageReceived(const Message &)), SLOT(messageReceived(const Message &)));
}

void PEPManager::publish(const QString& node, const XMPP::PubSubItem& it, Access access)
{
	if (!serverInfo_->hasPEP())
		return;

	PEPPublishTask* tp = new PEPPublishTask(client_->rootTask(),node,it,access);
	connect(tp, SIGNAL(finished()), SLOT(publishFinished()));
	tp->go(true);
}


void PEPManager::retract(const QString& node, const QString& id)
{
	if (!serverInfo_->hasPEP())
		return;

	PEPRetractTask* tp = new PEPRetractTask(client_->rootTask(),node,id);
	// FIXME: add notification of success/failure
	tp->go(true);
}


void PEPManager::publishFinished()
{
	PEPPublishTask* task = (PEPPublishTask*) sender();
	if (task->success()) {
		emit publish_success(task->node(),task->item());
	}
	else {
		qWarning() << QString("[%3] PEP Publish failed: '%1' (%2)").arg(task->statusString()).arg(QString::number(task->statusCode())).arg(client_->jid().full());
		emit publish_error(task->node(),task->item());
	}
}

void PEPManager::get(const XMPP::Jid& jid, const QString& node, const QString& id)
{
	PEPGetTask* g = new PEPGetTask(client_->rootTask(),jid.bare(),node,id);
	connect(g, SIGNAL(finished()), SLOT(getFinished()));
	g->go(true);
}

void PEPManager::messageReceived(const Message &m)
{
	foreach (const XMPP::PubSubRetraction &i, m.pubsubRetractions()) {
		emit itemRetracted(m.from(),m.pubsubNode(), i);
	}
	foreach (const XMPP::PubSubItem &i, m.pubsubItems()) {
		emit itemPublished(m.from(),m.pubsubNode(),i);
	}
}

void PEPManager::getFinished()
{
	PEPGetTask* task = (PEPGetTask*) sender();
	if (task->success()) {
		// Act as if the item was published. This is a convenience
		// implementation, probably should be changed later.
		if (!task->items().isEmpty()) {
			emit itemPublished(task->jid(),task->node(),task->items().at(0));
		}
	}
	else {
		qWarning() << QString("[%3] PEP Get failed: '%1' (%2)").arg(task->statusString()).arg(QString::number(task->statusCode())).arg(client_->jid().full());
	}
}
