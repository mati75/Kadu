/**
*
* Project:     Logitech MX610 Notify for Kadu
* File:        mx610_notify.cpp
* Copyright:   (C) 2006-2008 by Korneliusz Jrzebski
* Author:      Korneliusz Jarzebski <korneliusz at jarzebski dot pl>
* Modified by: Tomasz Rostanski <rozteck at interia dot pl>
* Link:        http://www.jarzebski.pl/
* Version:     0.4.1
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

#include <fcntl.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/hiddev.h>

#include <QtCore/QMap>
#include <QtCore/QProcess>
#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>
#include <QtGui/QToolTip>
#include <QtGui/QMessageBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

#include "config_file.h"
#include "debug.h"
#include "kadu_parser.h"
#include "misc.h"
#include "mx610_notify.h"
#include "message_box.h"
#include "chat_manager.h"

/* Commands */
unsigned char MX610Notify::MX_Confirm[6] =    { 0x01, 0x80, 0x52, 0x00, 0x00, 0x00 };
unsigned char MX610Notify::MX_IM_On[6] =      { 0x01, 0x80, 0x52, 0x00, 0x05, 0x00 };
unsigned char MX610Notify::MX_IM_FastOn[6] =  { 0x01, 0x80, 0x52, 0x00, 0x02, 0x00 };
unsigned char MX610Notify::MX_IM_Blink[6] =   { 0x01, 0x80, 0x52, 0x00, 0x03, 0x00 };
unsigned char MX610Notify::MX_IM_Pulse[6] =   { 0x01, 0x80, 0x52, 0x00, 0x04, 0x00 };
unsigned char MX610Notify::MX_IM_Off[6] =     { 0x01, 0x80, 0x52, 0x00, 0x06, 0x00 };
unsigned char MX610Notify::MX_IM_FastOff[6] = { 0x01, 0x80, 0x52, 0x00, 0x01, 0x00 };
unsigned char MX610Notify::MX_EM_On[6] =      { 0x01, 0x80, 0x52, 0x05, 0x00, 0x00 };
unsigned char MX610Notify::MX_EM_FastOn[6] =  { 0x01, 0x80, 0x52, 0x02, 0x00, 0x00 };
unsigned char MX610Notify::MX_EM_Blink[6] =   { 0x01, 0x80, 0x52, 0x03, 0x00, 0x00 };
unsigned char MX610Notify::MX_EM_Pulse[6] =   { 0x01, 0x80, 0x52, 0x04, 0x00, 0x00 };
unsigned char MX610Notify::MX_EM_Off[6] =     { 0x01, 0x80, 0x52, 0x06, 0x00, 0x00 };
unsigned char MX610Notify::MX_EM_FastOff[6] = { 0x01, 0x80, 0x52, 0x01, 0x00, 0x00 };

MX610Notify* MX610_notify = NULL;

extern "C" int mx610_notify_init()
{
    kdebugf();
    MX610_notify = new MX610Notify();
    config_file.addVariable("MX610 Notify", "MouseDevice", "/dev/usb/hiddev0");
    config_file.addVariable("MX610 Notify", "BatterySafe", false);
    config_file.addVariable("MX610 Notify", "BatteryTime", 120);
    MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/mx610_notify.ui"), MX610_notify);
    kdebugf2();
    return 0;
}

extern "C" void mx610_notify_close()
{
    kdebugf();
    MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/mx610_notify.ui"), MX610_notify);
    delete MX610_notify;
    MX610_notify = 0;
    kdebugf2();
}

MX610ConfigurationWidget::MX610ConfigurationWidget(QWidget *parent, char *name)
	: NotifierConfigurationWidget(parent, name), currentNotifyEvent("")
{
    QGridLayout *Layout = new QGridLayout(this, 4, 4, 3);
        
    LEDComboBox = new QComboBox(this);
    LEDComboBox -> insertItem(tr("Instant message LED"));
    LEDComboBox -> insertItem(tr("E-Mail LED"));
    QToolTip::add(LEDComboBox, tr("Select LED which was used to this notify"));
	
    ModeComboBox = new QComboBox(this);
    ModeComboBox -> insertItem(tr("Fast"));
    ModeComboBox -> insertItem(tr("Highlight"));
    ModeComboBox -> insertItem(tr("Blink"));
    ModeComboBox -> insertItem(tr("Pulse"));
    QToolTip::add(ModeComboBox, tr("Select mode which was used to this notify"));
	
    Layout -> addWidget(new QLabel(tr("LED") + ":", this), 0, 0, Qt::AlignRight);
    Layout -> addWidget(LEDComboBox, 0, 1);
    Layout -> addWidget(new QLabel(tr("Mode") + ":", this), 0, 2, Qt::AlignRight);
    Layout -> addWidget(ModeComboBox, 0, 3);

    parent->layout()->addWidget(this);
}

MX610ConfigurationWidget::~MX610ConfigurationWidget()
{
}

void MX610Notify::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
    connect(mainConfigurationWindow->widgetById("MX610/TestLED"), SIGNAL(clicked()), this, SLOT(ModuleSelfTest()));
}

void MX610ConfigurationWidget::saveNotifyConfigurations()
{
    if (currentNotifyEvent != "")
    {
	LEDSelects[currentNotifyEvent] = LEDComboBox->currentItem();
	ModeSelects[currentNotifyEvent] =  ModeComboBox->currentItem();
    }

    QMapIterator<QString, int> i(LEDSelects);
    while (i.hasNext())
    {
	i.next();
	config_file.writeEntry("MX610 Notify", i.key() + "/LED", i.value());
    }

    QMapIterator<QString, int> j(ModeSelects);
    while (j.hasNext())
    {
	j.next();
	config_file.writeEntry("MX610 Notify", j.key() + "/Mode", j.value());
    }
}

void MX610ConfigurationWidget::switchToEvent(const QString &event)
{

    if (currentNotifyEvent != "")
    {
	LEDSelects[currentNotifyEvent] = LEDComboBox->currentItem();
	ModeSelects[currentNotifyEvent] = ModeComboBox->currentItem();
    }
        
    currentNotifyEvent = event;

    if (LEDSelects.contains(event))
    {
	LEDComboBox->setCurrentItem(LEDSelects[event]);
    } else
    {
	LEDComboBox->setCurrentItem(config_file.readNumEntry("MX610 Notify", event + "/LED"));
    }

    if (ModeSelects.contains(event))
    {
	ModeComboBox->setCurrentItem(ModeSelects[event]);
    } else
    {
	ModeComboBox->setCurrentItem(config_file.readNumEntry("MX610 Notify", event + "/Mode"));
    }
}


MX610Notify::MX610Notify(QObject *parent, const char *name) : Notifier(parent, name),
	     IM_LED_On(false), EM_LED_On(false), IM_LED_Disable(false), EM_LED_Disable(false)
{
    kdebugf();

    config_file.addVariable("MX610 Notify", "NewChat/LED", 0);
    config_file.addVariable("MX610 Notify", "NewChat/Mode", 0);
    config_file.addVariable("MX610 Notify", "NewMessage/LED", 0);
    config_file.addVariable("MX610 Notify", "NewMessage/Mode", 0);

    /* TODO
    config_file.addVariable("MX610 Notify", "ConnectionError/LED", 0);
    config_file.addVariable("MX610 Notify", "ConnectionError/Mode", 0);
    config_file.addVariable("MX610 Notify", "StatusChanged/ToOnline/LED", 0);
    config_file.addVariable("MX610 Notify", "StatusChanged/ToOnline/Mode", 0);
    config_file.addVariable("MX610 Notify", "StatusChanged/ToBusy/LED", 0);
    config_file.addVariable("MX610 Notify", "StatusChanged/ToBusy/Mode", 0);
    config_file.addVariable("MX610 Notify", "StatusChanged/ToInvisible/LED", 0);
    config_file.addVariable("MX610 Notify", "StatusChanged/ToInvisible/Mode", 0);
    config_file.addVariable("MX610 Notify", "StatusChanged/ToOffline/LED", 0);
    config_file.addVariable("MX610 Notify", "StatusChanged/ToOffline/Mode", 0);
    config_file.addVariable("MX610 Notify", "FileTransfer/IncomingFile/LED", 0);
    config_file.addVariable("MX610 Notify", "FileTransfer/IncomingFile/Mode", 0);
    config_file.addVariable("MX610 Notify", "FileTransfer/Finished/LED", 0);
    config_file.addVariable("MX610 Notify", "FileTransfer/Finished/Mode", 0);
    */
        
    notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "MX610 Notify"), this);

    timer = new QTimer(this);

    connect( timer, SIGNAL(timeout()), this, SLOT(LEDControl()) );
		
    connect( chat_manager, SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)));

    connect( &pending, SIGNAL(messageFromUserDeleted(UserListElement)), this, SLOT(messageReceived(UserListElement)) );

    kdebugf2();
}

MX610Notify::~MX610Notify()
{
    kdebugf();

    disconnect( &pending, SIGNAL(messageFromUserDeleted(UserListElement)), this, SLOT(messageReceived(UserListElement)) );

    disconnect(chat_manager, SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)));

    disconnect(timer, SIGNAL(timeout()), this, SLOT(LEDControl()));

    notification_manager->unregisterNotifier("MX610 Notify");

    kdebugf2();
}

/*  void MX610Notify::debug(Notification *notification)
{
    int LEDtype = config_file.readNumEntry("MX610 Notify", notification->type() + "/LED");
    int LEDmode = config_file.readNumEntry("MX610 Notify", notification->type() + "/Mode");
    const UserListElements &senders = notification->userListElements();
    UserListElement ule;
    if (senders.count()) ule = notification->userListElements()[0];
    QString uin;
    if (ule.usesProtocol("Gadu"))
    uin = ule.ID("Gadu");
    MessageBox::msg(tr("NOTIFY form %1 type: %2, led: %3, mode: %4").arg(uin).arg(notification->type()).arg(LEDtype).arg(LEDmode));
} */

void MX610Notify::BatteryControl(void)
{
    kdebugf();

    QDateTime currentdatetime = QDateTime::currentDateTime();

    QTime actualTime = currentdatetime.time();

    actualTime = actualTime.addSecs(config_file.readNumEntry("MX610 Notify", "BatteryTime")*(-1));
    
    if (IM_LED_On && (actualTime > IM_LED_Timestamp))
    {
	IM_LED_Disable = true;
    }

    if (EM_LED_On && (actualTime > EM_LED_Timestamp))
    {
	EM_LED_Disable = true;
    }

    kdebugf2();
}

void MX610Notify::LEDControl(void)
{
    kdebugf();

    if (!EM_LED_On && !IM_LED_On)
    {
	return;
    }
    
    if (config_file.readBoolEntry("MX610 Notify", "BatterySafe"))
    {
	BatteryControl();
    }

    if (IM_LED_Disable)
    {
	IM_LED_On = false;
	IM_LED_Owner = "";
        SendToMX610(MX_IM_Off);
    }
    
    if (EM_LED_Disable)
    {
	EM_LED_On = false;
	EM_LED_Owner = "";
        SendToMX610(MX_EM_Off);
    }
    
    if (!EM_LED_On && !IM_LED_On)
    {
	timer->stop();
    }
        
    kdebugf2();
}

void MX610Notify::chatWidgetActivated( ChatWidget* chat )
{
    kdebugf();

    QList<ChatWidget*>::iterator chatIt = msgChats_.find( chat );

    if (chatIt != msgChats_.end())
    {
        msgChats_.remove(chatIt);
    }

    if (msgChats_.empty())
    {
	if (IM_LED_Owner == "NewMessage")
	{
	    IM_LED_Disable = true;
	}
		
	if (EM_LED_Owner == "NewMessage")
	{
	    EM_LED_Disable = true;
	}
    }

    kdebugf2();
}

void MX610Notify::messageReceived( UserListElement user )
{
    kdebugf();
    
    if (!pending.pendingMsgs())
    {
	if (IM_LED_Owner == "NewChat")
	{
	    IM_LED_Disable = true;
	}
	
	if (EM_LED_Owner == "NewChat")
	{
	    EM_LED_Disable = true;
	}
    }

        kdebugf2();
}

void MX610Notify::SendToMX610(unsigned char *command)
{
    int devhandle;

    unsigned int i;

    struct hiddev_usage_ref h_usage_ref;

    struct hiddev_report_info h_report_info;

    QString devnode = config_file.readEntry("MX610 Notify", "MouseDevice");

    if ((devhandle = open(devnode, O_RDONLY)) < 0)
    {
        return;
    }

    if (ioctl(devhandle, HIDIOCINITREPORT, 0) < 0)
    {
        close(devhandle);
        return;
    }

    for (i = 0; i < 6; ++i)
    {
        h_usage_ref.field_index = 0;
        h_usage_ref.report_id = 0x10;
        h_usage_ref.report_type = HID_REPORT_TYPE_OUTPUT;
        h_usage_ref.usage_code = 0xff000001;
        h_usage_ref.usage_index = i;
        h_usage_ref.value = command[i];

        int ret = ioctl(devhandle, HIDIOCSUSAGE, &h_usage_ref);

        if (ret < 0)
        {
            close(devhandle);
            return;
        }

        h_usage_ref.usage_index++;
    }

    h_report_info.report_id = 0x10;
    h_report_info.report_type = HID_REPORT_TYPE_OUTPUT;
    h_report_info.num_fields = 1;

    ioctl(devhandle, HIDIOCSREPORT, &h_report_info);

    close(devhandle);

    if (command == MX_Confirm)
    {
        usleep(100*1000);
    } else
    {
        SendToMX610(MX_Confirm);
    }

    return;

}

void MX610Notify::ModuleSelfTest(void)
{
    int devhandle, version, result;
    struct hiddev_devinfo device_info;
    QString devnode, vendor, product;

    devnode = config_file.readEntry("MX610 Notify", "MouseDevice");

    devhandle = open(devnode, O_RDONLY);

    if (devhandle < 0)
    {
        MessageBox::msg(tr("Error : %1").arg(strerror(errno)), false, "Error");
        return;
    }

    result = ioctl(devhandle, HIDIOCINITREPORT, 0);

    if (result < 0)
    {
        MessageBox::msg(tr("Error : %1").arg(strerror(errno)), false, "Error");
        close(devhandle);
        return;
    }

    ioctl(devhandle, HIDIOCGVERSION, &version);
    ioctl(devhandle, HIDIOCGDEVINFO, &device_info);
    vendor.sprintf("%04x", device_info.vendor);
    product.sprintf("%04x", device_info.product);
    
    if ((vendor != "046d") && (product != "ffffc518"))
    {
	MessageBox::msg(tr("Device node : %1\n\nThis device is not Logitech MX610.").arg(config_file.readEntry("MX610 Notify", "MouseDevice")), false, "Error");
        close(devhandle);
	return;
    }
        
    SendToMX610(MX_IM_FastOn);
    SendToMX610(MX_EM_FastOn);

    switch (QMessageBox::information(0,
                                     tr("Logitech MX610 Selftest"),
                                     tr("Device node : %1\nHiddev driver version : %2.%3.%4\n\nVendor : Logitech\nProduct : MX610").
                                     arg(config_file.readEntry("MX610 Notify", "MouseDevice")).arg(version >> 16).arg((version >> 8) & 0xff).arg(version & 0xff),
                                     tr("OK")))
    {
        default:
            SendToMX610(MX_IM_FastOff);
            SendToMX610(MX_EM_FastOff);
    }
    close(devhandle);
}

void MX610Notify::notify(Notification *notification)
{

    if ((notification->type() != "NewChat") && (notification->type() != "NewMessage"))
    {
	return;
    } 

    int LEDtype = config_file.readNumEntry("MX610 Notify", notification->type() + "/LED");

    if ((LEDtype == 0) && IM_LED_On)
    {
	return;
    }
    
    if ((LEDtype == 1) && EM_LED_On)
    {
	return;
    }
    
    timer->start(1000);
    
    LEDControl();

    unsigned char *LEDMode;

    QString NotificationType = notification->type();

    switch (config_file.readNumEntry("MX610 Notify", NotificationType  + "/Mode"))
    {
       case 1 :
           LEDMode = (LEDtype == 0) ? MX_IM_On : MX_EM_On;
           break;
           
       case 2 :
           LEDMode = (LEDtype == 0) ? MX_IM_Blink : MX_EM_Blink;
           break;

       case 3 :
           LEDMode = (LEDtype == 0) ? MX_IM_Pulse : MX_EM_Pulse;
           break;

       default :
           LEDMode = (LEDtype == 0) ? MX_IM_FastOn : MX_EM_FastOn;
    }

    QDateTime currentdatetime = QDateTime::currentDateTime();
        
    switch (LEDtype)
    {
	case 0:
	    IM_LED_On = true;
	    IM_LED_Owner = NotificationType;
	    IM_LED_Disable = false;
	    IM_LED_Timestamp = currentdatetime.time();
	    break;

	case 1:
	    EM_LED_On = true;
	    EM_LED_Owner = NotificationType;
	    EM_LED_Disable = false;
	    EM_LED_Timestamp = currentdatetime.time();
	    break;
    }
    
    SendToMX610(LEDMode);
    
    return;
    
}

NotifierConfigurationWidget *MX610Notify::createConfigurationWidget(QWidget *parent , char *name )
{
    return new MX610ConfigurationWidget(parent, name);
}
