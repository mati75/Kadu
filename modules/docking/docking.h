#ifndef DOCKING_H
#define DOCKING_H

#include <qlabel.h>
#include <qpixmap.h>

class DockingManager : public QObject
{
	Q_OBJECT

	private:
		QTimer *icon_timer;
		bool blink;
		void defaultToolTip();
		bool hintmanager_existed_on_start;

	private slots:
		void changeIcon();
		void dockletChange(int id);
		void pendingMessageAdded();
		void pendingMessageDeleted();
		void statusPixmapChanged(QPixmap &);
		void findTrayPosition(QPoint& pos);

	public:
		DockingManager();
		~DockingManager();
		void trayMousePressEvent(QMouseEvent * e);
		QPixmap defaultPixmap();

	public slots:
		/**
			Moduł implementujący dokowanie powinien to ustawic
			na true przy starcie i false przy zamknieciu, aby
			kadu wiedzialo, ze jest zadokowane.
		**/
		void setDocked(bool docked);

	signals:
		void trayPixmapChanged(const QPixmap& pixmap);
		void trayTooltipChanged(const QString& tooltip);
		void searchingForTrayPosition(QPoint& pos);
		void mousePressMidButton();
		void mousePressLeftButton();
		void mousePressRightButton();
};

extern DockingManager* docking_manager;

#endif
