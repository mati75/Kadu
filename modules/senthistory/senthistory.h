#ifndef SENTHISTORY_H
	#define SENTHISTORY_H


#include <QList>
#include <QObject>

#include <chat_widget.h>
#include <custom_input.h>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"


#define  SENTHISTORY_DEFAULTSHORTCUT_PREVIOUSMESSAGE                     "Ctrl+Up"
#define  SENTHISTORY_DEFAULTSHORTCUT_NEXTMESSAGE                       "Ctrl+Down"
#define  SENTHISTORY_DEFAULTSHORTCUT_PREVIOUSMESSAGEFROMALLCHATS     "Ctrl+Alt+Up"
#define  SENTHISTORY_DEFAULTSHORTCUT_NEXTMESSAGEFROMALLCHATS       "Ctrl+Alt+Down"


class SentHistory : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	public:
		SentHistory();
		~SentHistory();
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
	protected:
		void configurationUpdated();
	private slots:
		void chatCreated( ChatWidget *chatwidget );
		void chatDestroying( ChatWidget *chatwidget );
		void messageSendRequested( ChatWidget *chat );
		void editKeyPressed( QKeyEvent* e, ChatWidget* sender, bool &handled );
		void inputMessage( ChatWidget* edit );
	private:
		void createDefaultConfiguration();
		static QList< QPair<UserListElements,QString> > sentmessages;
		ChatWidget *lastChatWidget;
		int message_n;
		bool thischatonly;
};


extern SentHistory *senthistory;


#endif
