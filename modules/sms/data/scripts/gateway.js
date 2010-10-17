/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

GatewayManager.prototype = {
	addItem: function(gateway) {
		this.items[gateway.id()] = gateway;
	},

	byId: function(gatewayId) {
		return this.items[gatewayId];
	},

	sendSms: function(gatewayId, recipient, sender, signature, content, callbackObject) {
		if (this.items[gatewayId]) {
			this.items[gatewayId].sendSms(recipient, sender, signature, content, callbackObject);
		} else {
			callbackObject.failure("No valid gateway found");
		}
	},
};

function GatewayManager() {
	this.items = new Object();
	return this;
}

gatewayManager = new GatewayManager();

GatewayQuery.prototype = {
	getGateway: function(phoneNumber, callbackObject) {
		this.callbackObject = callbackObject;

		if (!network) {
			this.result("");
			return;
		}

		var gatewayCheckerUrl = "http://is.eranet.pl/updir/check.cgi?t=" + phoneNumber;

		this.reply = network.get(gatewayCheckerUrl);
		this.reply.finished.connect(this, this.replyFinished);
	},

	replyFinished: function() {
		if (!this.reply.ok()) {
			this.result("");
			return;
		}

		var content = this.reply.content();
		var pattern = new RegExp("260 ([0-9]{2})");
		var match = pattern.exec(content);

		if (null == match) {
			this.result("");
			return;
		}

		this.result(match[1]);
	},

	result: function(gateway) {
		this.callbackObject.queryFinished(gateway);
	},

	gatewayFromId: function(id) {
		var gateway = gatewayManager.byId(id);
		if (gateway) {
			return gateway.name();
		} else {
			return "";
		}
	}
};

function GatewayQuery() {
	return this;
}
