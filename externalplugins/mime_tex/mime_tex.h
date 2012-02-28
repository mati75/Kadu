#ifndef MIME_TEX_H
#define MIME_TEX_H

#include <QObject>
#include <QStringList>

class UserGroup;
class QWidget;
class QAction;
class ActionDescription;

namespace MimeTeX {
	//! This class provides "TeX formula" action
	class MimeTeX : public QObject
	{
		Q_OBJECT

		public:
			static void createInstance();
			static void destroyInstance();
			static MimeTeX * instance();

			//! Default font size used by MimeTeX (see MimeTeX tab in config dialog for details).
			static int defaultFontSize();

		signals:
			//! This signal is emmited when the MimeTeX is deleted
			void deleting();

		public slots:
				/*!
				 * \brief Called each time, "TeX formula" actions button is being pressed
				 * \param *group interlocutors
				 * \param *actionButton "TeX formula" actions button
				 * \param activated ignored
				 */
				//void TeXActionActivated(const UserGroup *group, const QWidget *actionButton, bool activated);
				void TeXActionActivated(QAction *action, bool toggled);

		private:
			static MimeTeX *Instance;

			/*!
			 * \brief Default constructor
			 * \param *parent parent object
			 */
			MimeTeX(QObject *parent = 0);

			//! Default destructor
			virtual ~MimeTeX();

			//! temp files names to be deleted in the destructor
			QStringList tmpFiles;

			//! Description of TeXAction
			ActionDescription *TeXActionDescription;
	};
}

#endif

