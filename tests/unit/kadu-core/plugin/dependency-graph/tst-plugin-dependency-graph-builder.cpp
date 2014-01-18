/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "misc/algorithm.h"
#include "misc/memory.h"
#include "plugin/dependency-graph/plugin-dependency-graph.h"
#include "plugin/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugin/metadata/plugin-metadata.h"
#include "plugin/metadata/plugin-metadata-builder.h"

#include <algorithm>
#include <map>
#include <QtTest/QtTest>

class tst_PluginDependencyGraphBuilder : public QObject
{
	Q_OBJECT

private:
	::std::map<QString, PluginMetadata> createPlugins(const QVector<QPair<QString, QStringList>> &plugins);
	PluginMetadata createPluginMetadata(const QPair<QString, QStringList> &plugin);
	void verifyDependencies(const PluginDependencyGraph &graph, const QString &pluginName, const QStringList &dependencies, const QStringList &dependents);

private slots:
	void simpleDependencyTest();
	void selfDependencyTest();
	void pluginOnlyAsDependencyTest();
	void cycleDependencyTest();

};

::std::map<QString, PluginMetadata> tst_PluginDependencyGraphBuilder::createPlugins(const QVector<QPair<QString, QStringList>> &plugins)
{
	auto result = ::std::map<QString, PluginMetadata>{};
	for (auto const &plugin : plugins)
		result.insert({plugin.first, createPluginMetadata(plugin)});
	return result;
}

PluginMetadata tst_PluginDependencyGraphBuilder::createPluginMetadata(const QPair<QString, QStringList> &plugin)
{
	auto builder = PluginMetadataBuilder{};
	return builder
			.setName(plugin.first)
			.setDependencies(plugin.second)
			.create();
}

void tst_PluginDependencyGraphBuilder::verifyDependencies(const PluginDependencyGraph &graph, const QString &pluginName, const QStringList &dependencies, const QStringList &dependents)
{
	auto graphDependencies = graph.directDependencies(pluginName);
	auto graphDependents = graph.directDependents(pluginName);

	QCOMPARE(graphDependencies.toList().toSet(), dependencies.toSet());
	QCOMPARE(graphDependents.toList().toSet(), dependents.toSet());
	QCOMPARE(graphDependencies.size(), dependencies.size());
	QCOMPARE(graphDependents.size(), dependents.size());
}

void tst_PluginDependencyGraphBuilder::simpleDependencyTest()
{
	auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(createPlugins(QVector<QPair<QString, QStringList>>
	{
		qMakePair(QString{"p1"}, QStringList{"p2", "p3", "p4"}),
		qMakePair(QString{"p2"}, QStringList{"p3", "p4"}),
		qMakePair(QString{"p3"}, QStringList{"p4"}),
		qMakePair(QString{"p4"}, QStringList{})
	}));

	QCOMPARE(graph.size(), 4);

	verifyDependencies(graph, "p1", {"p2", "p3", "p4"}, {});
	verifyDependencies(graph, "p2", {"p3", "p4"}, {"p1"});
	verifyDependencies(graph, "p3", {"p4"}, {"p1", "p2"});
	verifyDependencies(graph, "p4", {}, {"p1", "p2", "p3"});
}

void tst_PluginDependencyGraphBuilder::selfDependencyTest()
{
	auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(createPlugins(QVector<QPair<QString, QStringList>>
	{
		qMakePair(QString{"p1"}, QStringList{"p1"})
	}));

	QCOMPARE(graph.size(), 1);

	verifyDependencies(graph, "p1", {}, {});
}

void tst_PluginDependencyGraphBuilder::pluginOnlyAsDependencyTest()
{
	auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(createPlugins(QVector<QPair<QString, QStringList>>
	{
		qMakePair(QString{"p1"}, QStringList{"p2"}),
		qMakePair(QString{"p2"}, QStringList{"p3"}),
		qMakePair(QString{"p3"}, QStringList{"p4"})
	}));

	QCOMPARE(graph.size(), 0);
}

void tst_PluginDependencyGraphBuilder::cycleDependencyTest()
{
	auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(createPlugins(QVector<QPair<QString, QStringList>>
	{
		qMakePair(QString{"p1"}, QStringList{"p2"}),
		qMakePair(QString{"p2"}, QStringList{"p1"})
	}));

	QCOMPARE(graph.size(), 0);
}

QTEST_APPLESS_MAIN(tst_PluginDependencyGraphBuilder)
#include "tst-plugin-dependency-graph-builder.moc"