/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SOUND_H
#define SOUND_H

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QSemaphore>

#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"
#include "notify/notifier.h"
#include "modules.h"
#include "themes.h"

#include "sound-exports.h"

class Notification;
class PathListEdit;
class SamplePlayThread;
class SampleRecordThread;
class SoundConfigurationWidget;
class SoundPlayThread;

/**
 * @defgroup sound Sound
 * The sound module.
 * @{
 */

typedef void *SoundDevice;

/**
**/
enum SoundDeviceType
{
	SoundDeviceRecordOnly,
	SoundDevicePlayOnly,
	SoundDevicePlayAndRecord
};


class SOUNDAPI SoundManager : public Notifier, public ConfigurationUiHandler
{
    Q_OBJECT
	
	friend class SamplePlayThread;
	friend class SampleRecordThread;

	Themes *MyThemes;
	ConfigComboBox *ThemesComboBox;
	PathListEdit *ThemesPaths;
	SoundConfigurationWidget *ConfigurationWidget;

	QTime LastSoundTime;
	bool Mute;

	QMap<SoundDevice, SamplePlayThread *> PlayingThreads;
	QMap<SoundDevice, SampleRecordThread *> RecordingThreads;
	SoundPlayThread *PlayThread;

	int SimplePlayerCount;

	void connectNotify(const char *signal);
	void disconnectNotify(const char *signal);

	void applyTheme(const QString &themeName);
	void playSound(const QString &soundName);

	void import_0_6_5_configuration();
	void createDefaultConfiguration();

private slots:
	void setSoundThemes();
	void configurationWindowApplied();
	void soundFileEdited();

public slots:
	void play(const QString &path, bool force = false);
	void play(const QString &path, bool volumeControl, double volume);
	void setMute(const bool& enable);
	void stop();

public:
	SoundManager(bool firstLoad, const QString &name, const QString &configname);
	virtual ~SoundManager();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	virtual NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = 0);

	virtual void notify(Notification *notification);

	Themes * theme();

	bool isMuted() const;
	int timeAfterLastSound() const;

	SoundDevice openDevice(SoundDeviceType type, int sampleRate, int channels = 1);
	void closeDevice(SoundDevice device);
	void enableThreading(SoundDevice device);
	void setFlushingEnabled(SoundDevice device, bool enabled);
	bool playSample(SoundDevice device, const qint16 *data, int length);
	bool recordSample(SoundDevice device, qint16 *data, int length);

signals:
	void playSound(const QString &sound, bool volumeControl, double vol);
	void samplePlayed(SoundDevice device);
	void sampleRecorded(SoundDevice device);
	void openDeviceImpl(SoundDeviceType type, int sampleRate, int channels, SoundDevice *device);
	void closeDeviceImpl(SoundDevice device);
	void playSampleImpl(SoundDevice device, const qint16 *data, int length, bool *result);
	void recordSampleImpl(SoundDevice device, qint16 *data, int length, bool *result);
	void setFlushingEnabledImpl(SoundDevice device, bool enabled);

};

extern SOUNDAPI SoundManager *sound_manager;
/** @} */

#endif
