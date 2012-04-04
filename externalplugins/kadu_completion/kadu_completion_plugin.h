#ifndef KADU_COMPLETION_PLUGIN_H
#define KADU_COMPLETION_PLUGIN_H

#include <QtCore/QObject>

#include "plugins/generic-plugin.h"

namespace KaduCompletion {
	class KaduCompletionPlugin : public QObject, public GenericPlugin
	{
		Q_OBJECT
		Q_INTERFACES(GenericPlugin)

		public:
			virtual ~KaduCompletionPlugin();

			virtual int init(bool firstLoad);
			virtual void done();
	};
}

#endif // KADU_COMPLETION_PLUGIN_H
