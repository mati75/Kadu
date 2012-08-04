/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QDir>
#include <QFile>
#include <QIcon>
#include <QLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>

#include "tex_formula_dialog.h"
#include "formula_view.h"
#include "mime_tex.h"

#include "kadu-core/gui/widgets/chat-widget.h"
#include "kadu-core/configuration/configuration-file.h"
#include "kadu-core/gui/widgets/custom-input.h"
#include "kadu-core/misc/kadu-paths.h"
#include "kadu-core/debug.h"

const int formulaMargin = 5;
const int componentsViewMinWidht = 401;
const int formulaTextEditMinimunHeight = 200;

MimeTeX::TeXFormulaDialog::TeXFormulaDialog(ChatWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	kdebugf();
	
	chat = parent;
	
	setAttribute(Qt::WA_DeleteOnClose);
	// Check if .kadu/tex directory exists. Create one if not.
	QDir TeXDir(KaduPaths::instance()->profilePath() + "tex");
	if(!TeXDir.exists())
		if(TeXDir.mkdir(KaduPaths::instance()->profilePath() + "tex")) {
			kdebugm(KDEBUG_INFO, "Error creating directory %s\n", QString(KaduPaths::instance()->profilePath() + "tex").toLatin1().constData());
                }
	
	QDateTime curTime = QDateTime::currentDateTime();
	tmpFileName = KaduPaths::instance()->profilePath() + QString("tex/formula_%1.gif").arg(curTime.toString("yyyy-MM-dd_hh-mm"));
	if(QFile(tmpFileName).exists())
	{
		kdebugm(KDEBUG_INFO, "File %s already exists!!\n", tmpFileName.toLatin1().constData());
		int i = 1;
		QString tmpStr;
		do
		{
			tmpStr = KaduPaths::instance()->profilePath() + QString("tex/formula_%1_%2.gif").arg(curTime.toString("yyyy-MM-dd_hh:mm")).arg(i++);
			kdebugm(KDEBUG_INFO, "tmpStr: %s\n", tmpStr.toLatin1().constData());
		}
		while(QFile(tmpStr).exists());
		tmpFileName = tmpStr;
	}
	kdebugm(KDEBUG_INFO, "tmpFileName: %s\n", tmpFileName.toLatin1().constData());
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QHBoxLayout *upperLayout = new QHBoxLayout();
	mainLayout->addLayout(upperLayout);
	QVBoxLayout *formulaLayout = new QVBoxLayout();
	upperLayout->addLayout(formulaLayout);
	QVBoxLayout *componentsLayout = new QVBoxLayout();
	upperLayout->addLayout(componentsLayout);
	
	QLabel *formulaLabel = new QLabel(tr("Formula image"), this);
	formulaLayout->addWidget(formulaLabel);
	QLabel *componentsLabel = new QLabel(tr("Components"), this);
	componentsLayout->addWidget(componentsLabel);
	formulaView = new FormulaView(tmpFileName, this);
	formulaLayout->addWidget(formulaView);
	formulaLayout->addStretch();
	
	QTabWidget *componentsView = new QTabWidget(this);
	componentsLayout->addWidget(componentsView);
	QListWidget *relationsView = new QListWidget(componentsView);
	QListWidget *greekLettersView = new QListWidget(componentsView);
	QListWidget *arrowsView = new QListWidget(componentsView);
	QListWidget *delimitersView = new QListWidget(componentsView);
	QListWidget *symbolsView = new QListWidget(componentsView);
	
	relationsView->setMinimumWidth(componentsViewMinWidht);
	relationsView->setViewMode(QListWidget::IconMode);
	greekLettersView->setMinimumWidth(componentsViewMinWidht);
	greekLettersView->setViewMode(QListWidget::IconMode);
	arrowsView->setMinimumWidth(componentsViewMinWidht);
	arrowsView->setViewMode(QListWidget::IconMode);
	delimitersView->setMinimumWidth(componentsViewMinWidht);
	delimitersView->setViewMode(QListWidget::IconMode);
	symbolsView->setMinimumWidth(componentsViewMinWidht);
	symbolsView->setViewMode(QListWidget::IconMode);
	
	componentsView->addTab(relationsView, 
				   QPixmap(KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_icon.png"),
			       tr("&Relations"));
	componentsView->addTab(greekLettersView,
				   QPixmap(KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_icon.png"),
			       tr("&Greek Letters"));
	componentsView->addTab(arrowsView,
				   QPixmap(KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_icon.png"),
			       tr("&Arrows"));
	componentsView->addTab(delimitersView,
				   QPixmap(KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_icon.png"),
			       tr("&Delimiters"));
	componentsView->addTab(symbolsView,
				   QPixmap(KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_icon.png"),
			       tr("&Symbols"));
	
	connect(relationsView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(insertComponentSlot(QListWidgetItem *)));
	connect(greekLettersView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(insertComponentSlot(QListWidgetItem *)));
	connect(arrowsView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(insertComponentSlot(QListWidgetItem *)));
	connect(delimitersView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(insertComponentSlot(QListWidgetItem *)));
	connect(symbolsView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(insertComponentSlot(QListWidgetItem *)));
	
        kdebugm(KDEBUG_INFO, "%s\n", KaduPaths::instance()->dataPath().toLocal8Bit().constData());
        kdebugm(KDEBUG_INFO, "%s\n", QString(KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_frac.png").toLocal8Bit().constData());
	// Filling Relations tab
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_frac.png")),
		"\\frac{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_index_upper.png")),
		"^{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_index_lower.png")),
		"_{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_sqrt.png")),
		"\\sqrt{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_sqrt_n.png")),
		"\\sqrt[]{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_geq.png")),
		"\\geq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_leq.png")),
		"\\leq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_sim.png")),
		"\\sim", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_neq.png")),
		"\\neq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_equiv.png")),
		"\\equiv", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_approx.png")),
		"\\approx", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_simeq.png")),
		"\\simeq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_ll.png")),
		"\\ll", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_gg.png")),
		"\\gg", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_pm.png")),
		"\\pm", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_in.png")),
		"\\in", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_ni.png")),
		"\\ni", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_cap.png")),
		"\\cap", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_cup.png")),
		"\\cup", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_bigcap.png")),
		"\\bigcap", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_bigcup.png")),
		"\\bigcup", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_subset.png")),
		"\\subset", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_supset.png")),
		"\\supset", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_subseteq.png")),
		"\\subseteq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_supseteq.png")),
		"\\supseteq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_sum.png")),
		"\\sum{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_sum_from.png")),
		"\\sum\\limits_{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_sum_from_to.png")),
		"\\sum\\limits_{}^{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_prod.png")),
		"\\prod{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_prod_from.png")),
		"\\prod\\limits_{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_prod_from_to.png")),
		"\\prod\\limits_{}^{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_int.png")),
		"\\int{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_int_from.png")),
		"\\int\\limits_{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_int_from_to.png")),
		"\\int\\limits_{}^{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_oint.png")),
		"\\oint{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_oint_from.png")),
		"\\oint\\limits_{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_oint_from_to.png")),
		"\\oint\\limits_{}^{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/relations_tab_lim.png")),
		"\\lim_{}{}", relationsView);
	
	// Filling Greek Letters tab
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_alpha.png")),
		"\\alpha", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_beta.png")),
		"\\beta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_gamma.png")),
		"\\gamma", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_Gamma.png")),
		"\\Gamma", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_delta.png")),
		"\\delta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_Delta.png")),
		"\\Delta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_epsilon.png")),
		"\\epsilon", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_varepsilon.png")),
		"\\varepsilon", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_zeta.png")),
		"\\zeta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_eta.png")),
		"\\eta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_theta.png")),
		"\\theta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_vartheta.png")),
		"\\vartheta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_Theta.png")),
		"\\Theta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_iota.png")),
		"\\iota", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_kappa.png")),
		"\\kappa", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_lambda.png")),
		"\\lambda", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_Lambda.png")),
			"\\Lambda", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_mu.png")),
		"\\mu", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_nu.png")),
		"\\nu", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_xi.png")),
		"\\xi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_Xi.png")),
		"\\Xi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_pi.png")),
		"\\pi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_varpi.png")),
		"\\varpi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_Pi.png")),
		"\\Pi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_rho.png")),
		"\\rho", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_varrho.png")),
		"\\varrho", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_sigma.png")),
		"\\sigma", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_varsigma.png")),
		"\\varsigma", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_Sigma.png")),
		"\\Sigma", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_tau.png")),
		"\\tau", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_upsilon.png")),
		"\\upsilon", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_phi.png")),
		"\\phi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_varphi.png")),
		"\\varphi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_Phi.png")),
		"\\Phi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_chi.png")),
		"\\chi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_psi.png")),
		"\\psi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_Psi.png")),
		"\\Psi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_omega.png")),
		"\\omega", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/greek_letters_tab_Omega.png")),
		"\\Omega", greekLettersView);

	// Filling arrows tab
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_vec.png")),
		"\\vec{}", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_swarrow.png")),
		"\\swarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_leftarrow.png")),
		"\\leftarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_nwarrow.png")),
		"\\nwarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_uparrow.png")),
		"\\uparrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_nearrow.png")),
		"\\nearrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_rightarrow.png")),
		"\\rightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_searrow.png")),
		"\\searrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_leftrightarrow.png")),
		"\\leftrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_updownarrow.png")),
		"\\updownarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_longleftarrow.png")),
		"\\longleftarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_longrightarrow.png")),
		"\\longrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_longleftrightarrow.png")),
		"\\longleftrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_Leftarrow.png")),
		"\\Leftarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_Rightarrow.png")),
		"\\Rightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_Uparrow.png")),
		"\\Uparrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_Downarrow.png")),
		"\\Downarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_Leftrightarrow.png")),
		"\\Leftrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_Longleftarrow.png")),
		"\\Longleftarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_Longrightarrow.png")),
		"\\Longrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_Longleftrightarrow.png")),
		"\\Longleftrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_leftharpoonup.png")),
		"\\leftharpoonup", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_leftharpoondown.png")),
		"\\leftharpoondown", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_rightharpoonup.png")),
		"\\rightharpoonup", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_rightharpoondown.png")),
		"\\rightharpoondown", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/arrows_tab_rightleftharpoons.png")),
		"\\rightleftharpoons", arrowsView);

	// Filling Delimiters tab
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_lbrace.png")),
		"\\lbrace", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_rbrace.png")),
		"\\rbrace", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_langle.png")),
		"\\langle", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_rangle.png")),
		"\\rangle", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_hat.png")),
		"\\hat", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_overbrace.png")),
		"\\overbrace{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_overbrace_desc.png")),
		"\\overbrace{}^{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_underbrace.png")),
		"\\underbrace{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_underbrace_desc.png")),
		"\\underbrace{}_{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_overline.png")),
		"\\overline{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_underline.png")),
		"\\underline{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_left_bracket.png")),
		"\\left(", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_right_bracket.png")),
		"\\right)", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_left_square_bracket.png")),
		"\\left[", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_right_square_bracket.png")),
		"\\right]", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_left_lbrace.png")),
		"\\left{", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/delimiters_tab_right_rbrace.png")),
		"\\right}", delimitersView);

	// Filling Symbols tab
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_infty.png")),
		"\\infty", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_cdot.png")),
		"\\cdot", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_cdots.png")),
		"\\cdots", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_ldots.png")),
		"\\ldots", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_ddots.png")),
		"\\ddots", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_hbar.png")),
		"\\hbar", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_nabla.png")),
		"\\nabla", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_partial.png")),
		"\\partial", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_aleph.png")),
		"\\aleph", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_Re.png")),
		"\\Re", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_Im.png")),
		"\\Im", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_bot.png")),
		"\\bot", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_emptyset.png")),
		"\\emptyset", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_prime.png")),
		"\\prime", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_surd.png")),
		"\\surd", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_angle.png")),
		"\\angle", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/symbols_tab_forall.png")),
		"\\forall", symbolsView);

	
	undoButton = new QPushButton(QIcon(QPixmap(KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/editor_icons/undo.png")),
			tr("&Undo"), this);
	redoButton = new QPushButton(QIcon(QPixmap(KaduPaths::instance()->dataPath() + "plugins/data/mime_tex/editor_icons/redo.png")),
			tr("&Redo"), this);
	QPushButton *okButton = new QPushButton(tr("&Ok"), this);
	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);
	
	formulaTextEdit = new QTextEdit(this);
	formulaTextEdit->setMinimumHeight(formulaTextEditMinimunHeight);
	mainLayout->addWidget(formulaTextEdit);
	connect(&timer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
	
	QHBoxLayout *buttonsLayout = new QHBoxLayout();
	mainLayout->addLayout(buttonsLayout);
	buttonsLayout->addWidget(undoButton);
	buttonsLayout->addWidget(redoButton);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(okButton);
	buttonsLayout->addWidget(cancelButton);
	
	connect(undoButton, SIGNAL(clicked()), formulaTextEdit, SLOT(undo()));
	connect(redoButton, SIGNAL(clicked()), formulaTextEdit, SLOT(redo()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(okClickedSlot()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(&mimeTeXProcess, SIGNAL(finished(int, QProcess::ExitStatus)),formulaView, SLOT(fillView()));
	connect(&mimeTeXProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorMessage(QProcess::ProcessError)));
	connect(formulaTextEdit, SIGNAL(textChanged()), this, SLOT(updateButtons()));
	connect(MimeTeX::instance(), SIGNAL(deleting()), this, SLOT(deleteLater()));

	undoButton->setEnabled(false);
	redoButton->setEnabled(false);
	
	timer.start(config_file.readNumEntry("MimeTeX", "mimetex_refreshment_interval", 2000));
	formulaTextEdit->setFocus();
	
	kdebugf2();
}
MimeTeX::TeXFormulaDialog::~TeXFormulaDialog()
{
	kdebugf();
}

void MimeTeX::TeXFormulaDialog::okClickedSlot()
{
	kdebugf();
	timer.stop();
	
	QFile file(tmpFileName);
	if(file.exists())
		chat->edit()->insertPlainText(QString("[IMAGE %1]").arg(tmpFileName));
	
	accept();
	kdebugf2();
}

void MimeTeX::TeXFormulaDialog::timeoutSlot()
{
	kdebugf();
	if(QProcess::Running != mimeTeXProcess.state())
	{
		QFile file(tmpFileName);
		QStringList arguments;
		if(file.exists())
			file.remove();
		QString formulaStr = formulaTextEdit->toPlainText();
		formulaStr.replace(QChar('\n'), QChar(' '));
		if(!config_file.readBoolEntry("MimeTeX", "mimetex_transparent_bg", false))
			arguments << "-o";
		arguments << "-s";
		arguments << QString::number(config_file.readNumEntry("MimeTeX", "mimetex_font_size",
			MimeTeX::defaultFontSize()));
		arguments << "-e";
		arguments << tmpFileName;
		arguments << formulaStr;

		QStringList::const_iterator it;
#ifdef DEBUG_ENABLED
		int i = 0;
		for(it = arguments.constBegin(); it != arguments.constEnd(); ++it)
			kdebugm(KDEBUG_INFO, "Arg[%d]=%s\n", i++, (*it).toLocal8Bit().constData());
#endif
		//mimeTeXProcess.start(libPath("bin/mime_tex/mimetex"),
		mimeTeXProcess.start(KaduPaths::instance()->pluginsLibPath() + "bin/mime_tex/mimetex",
			arguments);
		mimeTeXProcess.waitForStarted();
	}

	kdebugf2();
}

void MimeTeX::TeXFormulaDialog::insertComponentSlot(QListWidgetItem *item)
{
	kdebugf();
	if(item)
		formulaTextEdit->insertPlainText(item->text());
	formulaTextEdit->setFocus();
	kdebugf2();
}

void MimeTeX::TeXFormulaDialog::updateButtons()
{
	undoButton->setEnabled(formulaTextEdit->isUndoRedoEnabled());
	redoButton->setEnabled(formulaTextEdit->isUndoRedoEnabled());
}

void MimeTeX::TeXFormulaDialog::errorMessage(QProcess::ProcessError)
{
	QMessageBox::critical(this, tr("TeX formula creator"), tr("Unable to run mimetex binary!"),
		QMessageBox::Ok, 0);
}

int MimeTeX::TeXFormulaDialog::tmpFileNumber = 0;
