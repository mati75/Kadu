/**
*
* Project:     Water Notify for Kadu
* File:        water_notify.cpp
* Copyright:   (C) 2008 by Korneliusz Jrzebski
* Modified by: Tomasz Rostanski <rozteck at interia dot pl>
* Author:      Korneliusz Jarzebski <korneliusz at jarzebski dot pl>
* Link:        http://www.jarzebski.pl/
* Version:     0.2.1
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*
*/

#include <dbus/dbus.h>
#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "water_notify.h"
#include "chat_manager.h"
#include "message_box.h"
#include "X11/Xlib.h"

WaterNotify* water_notify = NULL;

extern "C" int water_notify_init()
{
    kdebugf();
    water_notify = new WaterNotify();
    kdebugf2();
    return 0;
}

extern "C" void water_notify_close()
{
    kdebugf();
    delete water_notify;
    water_notify = NULL;
    kdebugf2();
}

NotifierConfigurationWidget* WaterNotify::createConfigurationWidget(QWidget*, char*)
{
    return 0;
}

void WaterNotify::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
    connect(mainConfigurationWindow->widgetById("WaterNotify/TestWaterDrop"), SIGNAL(clicked()), this, SLOT(ModuleSelfTest()));
}

WaterNotify::WaterNotify(QObject *parent, const char *name) : Notifier(parent, name),
	     WaterDrop_On(false), WaterDrop_Disable(false)
{
    kdebugf();

    config_file.addVariable("Water Notify", "RaindropDelay", 1);
    config_file.addVariable("Water Notify", "RaindropAmplitude", 10);

    MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/water_notify.ui"), this);
        
    notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "Water Notify"), this);

    timer = new QTimer(this);

    connect( timer, SIGNAL(timeout()), this, SLOT(WaterControl()) );
    connect( this, SIGNAL(searchingForTrayPosition(QPoint &)), kadu, SIGNAL(searchingForTrayPosition(QPoint &)) );
    connect( chat_manager, SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)) );
    connect( &pending, SIGNAL(messageFromUserDeleted(UserListElement)), this, SLOT(messageReceived(UserListElement)) );
    
    DetermineRootWindow();
    
    kdebugf2();
}

WaterNotify::~WaterNotify()
{
    kdebugf();

    disconnect( &pending, SIGNAL(messageFromUserDeleted(UserListElement)), this, SLOT(messageReceived(UserListElement)) );
    disconnect( chat_manager, SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)) );
    disconnect( this, SIGNAL(searchingForTrayPosition(QPoint &)), kadu, SIGNAL(searchingForTrayPosition(QPoint &)) );
    disconnect( timer, SIGNAL(timeout()), this, SLOT(WaterControl()) );
    
    notification_manager->unregisterNotifier("Water Notify");

    MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/water_notify.ui"), this);

    kdebugf2();
}

void WaterNotify::WaterControl(void)
{
    kdebugf();

    if (!WaterDrop_On)
    {
	return;
    }
    
    if (WaterDrop_Disable)
    {
	WaterDrop_On = false;
	WaterDrop_Owner = "";
    } else
    {
	WaterDrop(false);	
    }
    
    if (!WaterDrop_On)
    {
	timer->stop();
    }
        
    kdebugf2();
}

void WaterNotify::chatWidgetActivated( ChatWidget* chat )
{
    kdebugf();

    QList<ChatWidget*>::iterator chatIt = msgChats_.find( chat );

    if (chatIt != msgChats_.end())
    {
        msgChats_.remove(chatIt);
    }

    if (msgChats_.empty())
    {
	if (WaterDrop_Owner == "NewMessage")
	{
	    WaterDrop_Disable = true;
	}
    }

    kdebugf2();
}

void WaterNotify::messageReceived( UserListElement user )
{
    kdebugf();
    
    if (!pending.pendingMsgs())
    {
	if (WaterDrop_Owner == "NewChat")
	{
	    WaterDrop_Disable = true;
	}
    }
    kdebugf2();
}

void WaterNotify::notify(Notification *notification)
{
    if (WaterDrop_On || ((notification->type() != "NewChat") && (notification->type() != "NewMessage")))
    {
	return;
    } 

    if (notification->type() == "NewChat")
    {
	if (!config_file.readBoolEntry("Chat", "OpenChatOnMessage"))
    	{
    	    WaterDrop_On = true;
        }
    } else
    { 
        const UserListElements& senders = notification->userListElements();
        ChatWidget* chat = chat_manager->findChatWidget(senders);

        if (chat != NULL && !chat->hasFocus())
        {
            msgChats_.push_back( chat );
            WaterDrop_On = true;
        }
    }
    
    if (WaterDrop_On)
    {
	WaterDrop_Disable = false;
	WaterDrop_Owner = notification->type();
	WaterControl();
	int delay = config_file.readNumEntry("Water Notify", "RaindropDelay");
	timer->start(delay * 1000);
    }
    
    return;
}

void WaterNotify::AppendArgument_STRING(DBusMessageIter *iter, const char *value)
{
     dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &value);
}

void WaterNotify::AppendArgument_INT32(DBusMessageIter *iter, int value)
{
     dbus_message_iter_append_basic (iter, DBUS_TYPE_INT32, &value);
}

void WaterNotify::AppendArgument_DOUBLE(DBusMessageIter *iter, double value)
{
     dbus_message_iter_append_basic (iter, DBUS_TYPE_DOUBLE, &value);
}

void WaterNotify::WaterDrop(bool isTest)
{

  if (RootWindow_ID == 0)
  {
      if (isTest)
      {
          MessageBox::msg(tr("Root window failed"), false, "Error");
      }
      return;
  }

  DBusConnection *connection;
  DBusError error;
  DBusMessage *message;
  DBusMessageIter iter;
  DBusBusType type = DBUS_BUS_SESSION;

  dbus_error_init (&error);

  connection = dbus_bus_get (type, &error);

  if (connection == NULL)
  {
      if (isTest)
      {
          MessageBox::msg(tr("Failed to open connection : %s").arg(error.message), false, "Error");
      }
      dbus_error_free(&error);
      return;
  }

  message = dbus_message_new_method_call (NULL, "/org/freedesktop/compiz/water/allscreens/point", "org.freedesktop.compiz", "activate");

  dbus_message_set_auto_start (message, TRUE);
    
  if (message == NULL)
  {
      if (isTest)
      {
          MessageBox::msg(tr("Couldn't allocate D-Bus message"), false, "Error");
      }
      return;
  }

  if (!dbus_message_set_destination (message, "org.freedesktop.compiz"))
  {
      if (isTest)
      {
          MessageBox::msg(tr("Couldn't set message destination\n"), false, "Error");
      }
      return;
  }
  
  QPoint trayPosition;
  emit searchingForTrayPosition(trayPosition);
  double amplitude = config_file.readNumEntry("Water Notify", "RaindropAmplitude") * 0.01;

  dbus_message_iter_init_append (message, &iter);

  AppendArgument_STRING(&iter, "root");
  AppendArgument_INT32(&iter, RootWindow_ID);
  AppendArgument_STRING(&iter, "amplitude");
  AppendArgument_DOUBLE(&iter, amplitude);  
  AppendArgument_STRING(&iter, "x");
  AppendArgument_INT32(&iter, (trayPosition.x() + 10));
  AppendArgument_STRING(&iter, "y");
  AppendArgument_INT32(&iter, (trayPosition.y() + 13));

  dbus_connection_send (connection, message, NULL);
  dbus_connection_flush (connection);
  dbus_message_unref (message);
  dbus_connection_unref (connection);

}

void WaterNotify::DetermineRootWindow(void)
{
    Display *dpy = NULL;

    Window RootWindow = 0;

    dpy = XOpenDisplay(NULL);
    
    if (dpy == NULL)
    {
	RootWindow_ID = 0;
	return;
    }
    
    RootWindow = RootWindow(dpy, 0);
    
    RootWindow_ID = RootWindow;

    XCloseDisplay(dpy);
    
    dpy = NULL;
    
    return;
}

void WaterNotify::ModuleSelfTest()
{
    WaterDrop(true);
}
