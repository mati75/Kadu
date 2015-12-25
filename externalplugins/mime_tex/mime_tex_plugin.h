#ifndef MIME_TEX_PLUGIN_H
#define MIME_TEX_PLUGIN_H

#include <QtCore/QObject>

#include "plugin/plugin-root-component.h"

namespace MimeTeX {
	class MimeTeXPlugin : public QObject, public PluginRootComponent
	{
	Q_OBJECT
	Q_INTERFACES(PluginRootComponent)
	Q_PLUGIN_METADATA(IID "im.kadu.PluginRootComponent")

		public:
			virtual ~MimeTeXPlugin();

			virtual bool init(bool firstLoad);
			virtual void done();
	};
}

#endif
