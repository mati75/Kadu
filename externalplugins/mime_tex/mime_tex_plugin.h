#ifndef MIME_TEX_PLUGIN_H
#define MIME_TEX_PLUGIN_H

#include <QtCore/QObject>

#include "plugins/generic-plugin.h"

namespace MimeTeX {
	class MimeTeXPlugin : public QObject, public GenericPlugin
	{
		Q_OBJECT
		Q_INTERFACES(GenericPlugin)

		public:
			virtual ~MimeTeXPlugin();

			virtual int init(bool firstLoad);
			virtual void done();
	};
}

#endif
