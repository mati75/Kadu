#ifndef DOCKAREA_H
#define DOCKAREA_H

#include <qglobal.h>

#include <q3dockarea.h>
#include <q3popupmenu.h>
#include <qstring.h>
#include <q3valuelist.h>
//Added by qt3to4:
#include <QChildEvent>
#include <QContextMenuEvent>

#include "usergroup.h"

class DockArea : public Q3DockArea
{
	Q_OBJECT

	private:
		static bool Blocked;
		int SupportedActions;
		void setBlockToolbars(bool);
		static QList<DockArea *> AllDockAreas;

	protected:
		void contextMenuEvent(QContextMenuEvent* e);
		void childEvent(QChildEvent* e);

	private slots:
		void createNewToolbar();
		void blockToolbars();
		void toolbarPlaceChanged();

	public:
		DockArea(Qt::Orientation o, HandlePosition h,
			QWidget* parent,
			const char* name, int supportedActions);
		~DockArea();
		bool loadFromConfig(QWidget* toolbars_parent);
		/**
			Returns list of users that will be affected by activated action.
			It depends on where the dockarea is located. If dockarea is in chat
			window, selected users are the users in chat. If dockarea is the
			main window, selected users are the selected ones in contact
			list, and so on...
			Returns NULL if toolbar is no connected to user list.
		**/
		const UserGroup* selectedUsers();
		static bool blocked();
		Q3PopupMenu* createContextMenu(QWidget* parent);

		bool supportsAction(int actionType);

	public slots:
		void writeToConfig();
		void usersChangedSlot();

	signals:
		/**
			TODO: fix it, this is stupid

			Signal is emited when dockarea needs to know what users
			will be affected by activated action. It depends on
			where the dockarea is located. If dockarea is in chat window,
			selected users are the users in chat. If dockarea is the
			main window, selected users are the selected ones in contact
			list, and so on...
			Slot should change users pointer. NULL (default) means: do not
			execute action.
		**/
		void selectedUsersNeeded(const UserGroup*& users);

		/**
			New toolbar was attached.
		 **/
		void toolbarAttached();

};

#endif
