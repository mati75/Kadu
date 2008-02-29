#ifndef CHAT_MESSAGES_VIEW
#define CHAT_MESSAGES_VIEW

#include <qglobal.h>

#include "configuration_aware_object.h"
#include "gadu.h"
#include "kadu_text_browser.h"
#include "userlistelement.h"
//Added by qt3to4:
#include <QResizeEvent>
#include <QList>

class ChatMessage;
class QResizeEvent;

class ChatMessagesView : public KaduTextBrowser, ConfigurationAwareObject
{
	Q_OBJECT

	QList<ChatMessage *> Messages;
	unsigned int Prune;

	bool CfgNoHeaderRepeat;
	unsigned int CfgHeaderSeparatorHeight;
	unsigned int CfgNoHeaderInterval;
	unsigned int ParagraphSeparator;

	bool NoServerTime;
	int NoServerTimeDiff;

	QString ChatSyntaxWithHeader;
	QString ChatSyntaxWithoutHeader;

	Q3MimeSourceFactory *bodyformat;

	void repaintMessages();
	void updateBackgrounds();
	QString formatMessage(ChatMessage *message, ChatMessage *after);

	void pruneMessages();

protected:
	virtual void configurationUpdated();
	virtual void resizeEvent(QResizeEvent *e);

private slots:
	void pageUp();
	void pageDown();

	/**
		\fn void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path)
		TODO: zmieni� nag��wek
		TODO; przesun�� do klasy ChatMessage
		Slot obs�uguj�cy odebranie i zapis obrazka
		\param sender osoba, kt�ra wys�a�a obrazek
		\param size rozmiar obrazka
		\param crc32 suma kontrolna obrazka
		\param path �cie�ka do obrazka
	**/
	void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString& path);

public:
	ChatMessagesView(QWidget *parent = 0, const char *name = 0);
	virtual ~ChatMessagesView();

	void appendMessage(ChatMessage *message);
	void appendMessages(QList<ChatMessage *> messages);

	unsigned int countMessages();

	void setPrune(unsigned int prune);

public slots:
	void clearMessages();

};

#endif // CHAT_MESSAGES_VIEW
