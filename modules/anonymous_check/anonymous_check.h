#ifndef CHECK_ANONYMOUS_H
#define CHECK_ANONYMOUS_H

#include <QObject>
#include <QMultiHash>

#include "gadu.h"
#include "main_configuration_window.h"

class ChatWidget;

namespace AnonCheck {
	/*!
	 * This class provides functionality to automaticly get informations
	 * about anonymous interlocutor from the public directory.
	 * \brief Automaticly anonymous checking class
	 */
	class AnonymousCheck : public ConfigurationUiHandler
	{
		Q_OBJECT
		public:
			//! 
			/*!
			 * \brief Default constructor.
			 * \param parent points to patent's object.
			 * \param name is an object's name.
			 */
			AnonymousCheck(QObject *parent = 0, const char *name = 0);

			//! Default destructor.
			~AnonymousCheck();
			virtual void configurationUpdated();

		public slots:
			/*!
			 * \brief This slot is called wthen new chat is created.
			 * \param chat Pointer to newly created chat window.
			 */
			void onNewChatSlot(ChatWidget *chat);

			virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

		private slots:
			/*!
			 * \brief This slot is called when search in the public directory finishes finished.
			 * \param searchResults holds the search result.
			 * \param seq %Ignored here
			 * \param lastUin %Ignored here
			 */
			void onSearchFinishedSlot(SearchResults &searchResults, int seq, int lastUin);

		private:
			void showMsg(ChatWidget *chat, const QString &msg);
			QString HTMLColorStr(const QColor &color);

		protected:
			/*!
			 * The keys are Uins of checked interlocutors and the values are pointers to
			 * UserGroup groups on chat windows we're displaying results on.
			 * \brief Holds pointers to groups of interlocutors we're chatting with.
			 */
			QMultiHash<QString, const UserGroup *> groups;
	};
}

#endif
