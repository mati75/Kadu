#ifndef KADU_COMPLETION_PLUGIN_H
#define KADU_COMPLETION_PLUGIN_H

#include <QtCore/QObject>

#include "plugin/plugin-root-component.h"

namespace KaduCompletion {
	class KaduCompletionPlugin : public QObject, public PluginRootComponent
	{
		Q_OBJECT
		Q_INTERFACES(PluginRootComponent)
		Q_PLUGIN_METADATA(IID "im.kadu.PluginRootComponent")

		public:
			virtual ~KaduCompletionPlugin();

			virtual bool init(bool firstLoad);
			virtual void done();
	};
}

#endif // KADU_COMPLETION_PLUGIN_H
