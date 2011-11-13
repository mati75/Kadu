/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QHeaderView>
#include <QtGui/QPainter>
#include <QtGui/QTextDocument>
#include <QtGui/QTextFrame>
#include <QtGui/QTextFrameFormat>
#include <QtGui/QTreeView>

#include "accounts/account.h"
#include "buddies/buddy.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/message/pending-messages-manager.h"
#include "contacts/contact.h"
#include "gui/widgets/avatar-painter.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "identities/identity.h"
#include "model/roles.h"

#include "talkable-painter.h"

TalkablePainter::TalkablePainter(const TalkableDelegateConfiguration &configuration, QStyleOptionViewItemV4 option, const QModelIndex &index, bool useConfigurationColors) :
		Configuration(configuration), Option(option), Index(index),
		UseConfigurationColors(useConfigurationColors),
		FontMetrics(Configuration.font()),
		BoldFontMetrics(Configuration.boldFont()),
		DescriptionFontMetrics(Configuration.descriptionFont()),
		DescriptionDocument(0)
{
	Widget = static_cast<const QTreeView *>(option.widget);
	Q_ASSERT(Widget);

	const QStyle * const style = Widget->style();

	HFrameMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, Widget);
	VFrameMargin = style->pixelMetric(QStyle::PM_FocusFrameVMargin, 0, Widget);
}

TalkablePainter::~TalkablePainter()
{
	delete DescriptionDocument;
	DescriptionDocument = 0;
}

void TalkablePainter::fixColors()
{
#ifdef Q_OS_WIN
	// Kadu bug #1531
	// http://bugreports.qt.nokia.com/browse/QTBUG-15637
	// for windows only
	Option.palette.setColor(QPalette::All, QPalette::HighlightedText, Option.palette.color(QPalette::Active, QPalette::Text));
	Option.palette.setColor(QPalette::All, QPalette::Highlight, Option.palette.base().color().darker(108));
#endif
}

QColor TalkablePainter::textColor() const
{
	const QPalette::ColorGroup colorGroup = drawDisabled()
			? QPalette::Disabled
			: QPalette::Normal;

	const QPalette::ColorRole colorRole = drawSelected()
			? QPalette::HighlightedText
			: QPalette::Text;

	return Option.palette.color(colorGroup, colorRole);
}

bool TalkablePainter::useBold() const
{
	if (showMessagePixmap())
		return true;

	if (!Configuration.showBold())
		return false;

	const Contact &contact = Index.data(ContactRole).value<Contact>();
	if (!contact)
		return false;
	if (contact.isBlocking())
		return false;
	if (contact.ownerBuddy().isBlocked())
		return false;

	const Status &status = Index.data(StatusRole).value<Status>();
	return !status.isDisconnected();
}

bool TalkablePainter::showMessagePixmap() const
{
	if (Index.parent().isValid()) // contact
	{
		const Contact &contact = Index.data(ContactRole).value<Contact>();
		return contact && PendingMessagesManager::instance()->hasPendingMessagesForContact(contact);
	}
	else
	{
		const Buddy &buddy = Index.data(BuddyRole).value<Buddy>();
		return buddy && PendingMessagesManager::instance()->hasPendingMessagesForBuddy(buddy);
	}
}

bool TalkablePainter::showAccountName() const
{
	if (Index.parent().isValid())
		return true;

	return Option.state & QStyle::State_MouseOver && Configuration.showAccountName();
}

bool TalkablePainter::showDescription() const
{
	if (!Configuration.showDescription())
		return false;

	const QString &description = Index.data(DescriptionRole).toString();
	return !description.isEmpty();
}

void TalkablePainter::computeIconRect()
{
	IconRect = QRect(0, 0, 0, 0);

	const QPixmap &paintedIcon = icon();
	if (paintedIcon.isNull())
		return;

	QPoint topLeft = ItemRect.topLeft();
	IconRect = paintedIcon.rect();

	if (!Configuration.alignTop())
		topLeft.setY(topLeft.y() + (ItemRect.height() - paintedIcon.height()) / 2);
	else if (fontMetrics().lineSpacing() > paintedIcon.height())
		topLeft.setY(topLeft.y() + (fontMetrics().lineSpacing() - paintedIcon.height()) / 2);

	IconRect.moveTo(topLeft);
}

void TalkablePainter::computeMessageIconRect()
{
	MessageIconRect = QRect(0, 0, 0, 0);
	if (!showMessagePixmap())
		return;

	const QPixmap &icon = Configuration.messagePixmap();

	QPoint topLeft = ItemRect.topLeft();
	MessageIconRect = icon.rect();

	if (!Configuration.alignTop())
		topLeft.setY(topLeft.y() + (ItemRect.height() - icon.height()) / 2);
	else
		topLeft.setY(topLeft.y() + (fontMetrics().lineSpacing() + 3 - icon.height()) / 2);

	if (!IconRect.isEmpty())
		topLeft.setX(IconRect.right() + VFrameMargin);

	MessageIconRect.moveTo(topLeft);
}

void TalkablePainter::computeAvatarRect()
{
	AvatarRect = QRect(0, 0, 0, 0);
	if (!Configuration.showAvatars())
		return;

	AvatarRect.setWidth(Configuration.defaultAvatarSize().width() + 2 * HFrameMargin);

	if (!avatar().isNull())
		AvatarRect.setHeight(Configuration.defaultAvatarSize().height() + 2 * VFrameMargin);
	else
		AvatarRect.setHeight(1); // just a placeholder

	AvatarRect.moveRight(ItemRect.right());
	AvatarRect.moveTop(ItemRect.top());
}

QString TalkablePainter::getAccountName()
{
	const Account &account = Index.data(AccountRole).value<Account>();
	return account.accountIdentity().name();
}

QString TalkablePainter::getName()
{
	return Index.data(Qt::DisplayRole).toString();
}

bool TalkablePainter::drawSelected() const
{
	return Option.state & QStyle::State_Selected;
}

bool TalkablePainter::drawDisabled() const
{
	const Buddy &buddy = Index.data(BuddyRole).value<Buddy>();
	return buddy.isOfflineTo();
}

QTextDocument * TalkablePainter::createDescriptionDocument(const QString &text, int width, QColor color) const
{
	const QString &description = Qt::escape(text)
	        .replace('\n', Configuration.showMultiLineDescription() ? QLatin1String("<br/>") : QLatin1String(" "));

	QTextDocument * const doc = new QTextDocument();

	doc->setDefaultFont(Configuration.descriptionFont());
	if (color.isValid())
		doc->setDefaultStyleSheet(QString("* { color: %1; }").arg(color.name()));

	doc->setHtml(QString("<span>%1</span>").arg(description));

	QTextOption opt = doc->defaultTextOption();
	opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	doc->setDefaultTextOption(opt);

	QTextFrameFormat frameFormat = doc->rootFrame()->frameFormat();
	frameFormat.setMargin(0);
	doc->rootFrame()->setFrameFormat(frameFormat);

	doc->setTextWidth(width);
	return doc;
}

QTextDocument * TalkablePainter::getDescriptionDocument(int width)
{
	if (DescriptionDocument)
		return DescriptionDocument;

	fixColors();

	const QColor &color = drawSelected() || drawDisabled() || !UseConfigurationColors
			? textColor()
			: Configuration.descriptionColor();
	DescriptionDocument = createDescriptionDocument(Index.data(DescriptionRole).toString(), width, color);
	return DescriptionDocument;
}

void TalkablePainter::computeAccountNameRect()
{
	AccountNameRect = QRect(0, 0, 0, 0);
	if (!showAccountName())
		return;

	const QString &accountDisplay = getAccountName();

	const int accountDisplayWidth = DescriptionFontMetrics.width(accountDisplay);

	AccountNameRect.setWidth(accountDisplayWidth);
	AccountNameRect.setHeight(DescriptionFontMetrics.height());

	AccountNameRect.moveRight(ItemRect.right() - AvatarRect.width() - HFrameMargin);
	AccountNameRect.moveTop(ItemRect.top());
}

void TalkablePainter::computeNameRect()
{
	NameRect = QRect(0, 0, 0, 0);

	int left = qMax(IconRect.right(), MessageIconRect.right());
	if (!IconRect.isEmpty() || !MessageIconRect.isEmpty())
		left += HFrameMargin;
	else
		left = ItemRect.left();

	int right;
	if (!AccountNameRect.isEmpty())
		right = AccountNameRect.left() - HFrameMargin;
	else if (!AvatarRect.isEmpty())
		right = AvatarRect.left() - HFrameMargin;
	else
		right = ItemRect.right() - HFrameMargin;

	int top = ItemRect.top();
	if (Configuration.alignTop())
		if (fontMetrics().lineSpacing() < IconRect.height())
			top += (IconRect.height() - fontMetrics().lineSpacing()) / 2;

	NameRect.moveTop(top);
	NameRect.setLeft(left);
	NameRect.setRight(right);
	NameRect.setHeight(fontMetrics().height());
}

void TalkablePainter::computeDescriptionRect()
{
	DescriptionRect = QRect(0, 0, 0, 0);

	if (!showDescription())
		return;

	DescriptionRect.setTop(NameRect.bottom() + VFrameMargin);
	DescriptionRect.setLeft(NameRect.left());

	if (!AvatarRect.isEmpty())
		DescriptionRect.setRight(AvatarRect.left() - HFrameMargin);
	else
		DescriptionRect.setRight(ItemRect.right() - HFrameMargin);

	DescriptionRect.setHeight((int)getDescriptionDocument(DescriptionRect.width())->size().height());
}

void TalkablePainter::computeLayout()
{
	computeIconRect();
	computeMessageIconRect();
	computeAvatarRect();
	computeAccountNameRect();
	computeNameRect();
	computeDescriptionRect();
}

QPixmap TalkablePainter::avatar() const
{
	return Index.data(AvatarRole).value<QPixmap>();
}

QPixmap TalkablePainter::icon() const
{
	return Index.data(Qt::DecorationRole).value<QIcon>().pixmap(16, 16);
}

const QFontMetrics & TalkablePainter::fontMetrics()
{
	if (useBold())
		return BoldFontMetrics;
	else
		return FontMetrics;
}

int TalkablePainter::itemIndentation()
{
	int level = 0;

	if (Widget->rootIsDecorated())
		level++;

	if (Index.parent().isValid())
		level++;

	return level * Widget->indentation();
}

int TalkablePainter::height()
{
	ItemRect = QRect(0, 0, 0, 20);

	const QHeaderView * const header = Widget->header();
	if (header)
		ItemRect.setWidth(header->sectionSize(0) - itemIndentation());

	ItemRect.adjust(HFrameMargin, VFrameMargin, -HFrameMargin, -VFrameMargin);

	computeLayout();

	QRect wholeRect = IconRect;
	wholeRect |= MessageIconRect;
	wholeRect |= AvatarRect;
	wholeRect |= AccountNameRect;
	wholeRect |= NameRect;
	wholeRect |= DescriptionRect;

	return wholeRect.height() + 2 * VFrameMargin;
}

void TalkablePainter::paintDebugRect(QPainter *painter, QRect rect, QColor color) const
{
	painter->save();
	painter->setPen(color);
	painter->drawRect(rect);
	painter->restore();
}

void TalkablePainter::paintIcon(QPainter *painter)
{
	const QPixmap &paintedIcon = icon();
	if (paintedIcon.isNull())
		return;

	painter->drawPixmap(IconRect, paintedIcon);
}

void TalkablePainter::paintMessageIcon(QPainter *painter)
{
	if (!showMessagePixmap())
		return;

	painter->drawPixmap(MessageIconRect, Configuration.messagePixmap());
}

void TalkablePainter::paintAvatar(QPainter *painter)
{
	const QRect &rect = AvatarRect.adjusted(VFrameMargin, HFrameMargin, -VFrameMargin, -HFrameMargin);

	AvatarPainter avatarPainter(Configuration, Option, rect, Index);
	avatarPainter.paint(painter);
}

void TalkablePainter::paintAccountName(QPainter *painter)
{
	if (!showAccountName())
		return;

	painter->setFont(Configuration.descriptionFont());
	painter->drawText(AccountNameRect, getAccountName());
}

void TalkablePainter::paintName(QPainter *painter)
{
	if (useBold())
		painter->setFont(Configuration.boldFont());
	else
		painter->setFont(Configuration.font());

	painter->drawText(NameRect, fontMetrics().elidedText(getName(), Qt::ElideRight, NameRect.width()));
}

void TalkablePainter::paintDescription(QPainter *painter)
{
	if (!showDescription())
		return;

	painter->setFont(Configuration.descriptionFont());
	painter->save();
	painter->translate(DescriptionRect.topLeft());
	getDescriptionDocument(DescriptionRect.width())->drawContents(painter);
	painter->restore();
}

void TalkablePainter::paint(QPainter *painter)
{
	ItemRect = Option.rect;
	ItemRect.adjust(HFrameMargin, VFrameMargin, -HFrameMargin, -VFrameMargin);

	computeLayout();

	fixColors();

	// some bit of broken logic
	if (drawSelected() || drawDisabled() || !UseConfigurationColors)
		painter->setPen(textColor());
	else
	{
		Buddy buddy = Index.data(BuddyRole).value<Buddy>();
		Contact contact = Index.data(ContactRole).value<Contact>();
		if (buddy.isBlocked() || contact.isBlocking())
			painter->setPen(QColor(255, 0, 0));
		else
			painter->setPen(Configuration.fontColor());
	}

	paintIcon(painter);
	paintMessageIcon(painter);
	paintAvatar(painter);
	paintAccountName(painter);
	paintName(painter);
	paintDescription(painter);

	/*
	paintDebugRect(painter, ItemRect, QColor(255, 0, 0));
	paintDebugRect(painter, IconRect, QColor(0, 255, 0));
	paintDebugRect(painter, MessageIconRect, QColor(255, 255, 0));
	paintDebugRect(painter, AvatarRect, QColor(0, 0, 255));
	paintDebugRect(painter, AccountNameRect, QColor(255, 0, 255));
	paintDebugRect(painter, NameRect, QColor(0, 255, 255));
	paintDebugRect(painter, DescriptionRect, QColor(0, 0, 0));
	*/
}
