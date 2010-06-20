/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QLayout>
#include <QLabel>
#include <QTabWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QPixmap>
#include <QFile>
#include <QSpinBox>

#include "tex_formula_dialog.h"
#include "formula_view.h"
#include "mime_tex.h"

#include "chat_widget.h"
#include "config_file.h"
#include "custom_input.h"
#include "misc.h"
#include "modules.h"
#include "debug.h"

const int formulaMargin = 5;
const int componentsViewMinWidht = 401;
const int formulaTextEditMinimunHeight = 200;

MimeTeX::TeXFormulaDialog::TeXFormulaDialog(ChatWidget *parent, const char *name, Qt::WindowFlags f)
	: QDialog(parent, name, false, f)
{
	kdebugf();
	
	chat = parent;
	
	setAttribute(Qt::WA_DeleteOnClose);
	// Check if .kadu/tex directory exists. Create one if not.
	QDir TeXDir(ggPath("tex"));
	if(!TeXDir.exists())
		if(TeXDir.mkdir(ggPath("tex")))
			kdebugm(KDEBUG_INFO, "Error creating directory %s\n", ggPath("tex").latin1());
	
	QDateTime curTime = QDateTime::currentDateTime();
	tmpFileName = ggPath("tex/formula_%1.gif").arg(curTime.toString("yyyy-MM-dd_hh-mm"));
	if(QFile(tmpFileName).exists())
	{
		kdebugm(KDEBUG_INFO, "File %s already exists!!\n", tmpFileName.latin1());
		int i = 1;
		QString tmpStr;
		do
		{
			tmpStr = ggPath("tex/formula_%1_%2.gif").arg(curTime.toString("yyyy-MM-dd_hh:mm")).arg(i++);
			kdebugm(KDEBUG_INFO, "tmpStr: %s\n", tmpStr.latin1());
		}
		while(QFile(tmpStr).exists());
		tmpFileName = tmpStr;
	}
	kdebugm(KDEBUG_INFO, "tmpFileName: %s\n", tmpFileName.latin1());
	QVBoxLayout *mainLayout = new QVBoxLayout(this, 3, 3, "main_layout");
	QHBoxLayout *upperLayout = new QHBoxLayout(mainLayout, 3, "upper_layout");
	QVBoxLayout *formulaLayout = new QVBoxLayout(upperLayout, 3, "formula_layout");
	QVBoxLayout *componentsLayout = new QVBoxLayout(upperLayout, 3, "formula_layout");
	
	QLabel *formulaLabel = new QLabel(tr("Formula image"), this, "formula_label");
	formulaLayout->addWidget(formulaLabel);
	QLabel *componentsLabel = new QLabel(tr("Components"), this, "components_label");
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
			       QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_icon.png")),
			       tr("&Relations"));
	componentsView->addTab(greekLettersView,
			       QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_icon.png")),
			       tr("&Greek Letters"));
	componentsView->addTab(arrowsView,
			       QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_icon.png")),
			       tr("&Arrows"));
	componentsView->addTab(delimitersView,
			       QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_icon.png")),
			       tr("&Delimiters"));
	componentsView->addTab(symbolsView,
			       QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_icon.png")),
			       tr("&Symbols"));
	
	connect(relationsView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(insertComponentSlot(QListWidgetItem *)));
	connect(greekLettersView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(insertComponentSlot(QListWidgetItem *)));
	connect(arrowsView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(insertComponentSlot(QListWidgetItem *)));
	connect(delimitersView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(insertComponentSlot(QListWidgetItem *)));
	connect(symbolsView, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(insertComponentSlot(QListWidgetItem *)));
	
	// Filling Relations tab
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_frac.png"))),
		"\\frac{}{}", relationsView);
	//(void) new QListWidgetItem(relationsView, "\\frac{}{}",
	//		QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_frac.png")));
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_index_upper.png"))),
		"^{}", relationsView);
	//(void) new QListWidgetItem(relationsView, "^{}",
	//		QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_index_upper.png")));
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_index_lower.png"))),
		"_{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sqrt.png"))),
		"\\sqrt{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sqrt_n.png"))),
		"\\sqrt[]{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_geq.png"))),
		"\\geq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_leq.png"))),
		"\\leq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sim.png"))),
		"\\sim", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_neq.png"))),
		"\\neq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_equiv.png"))),
		"\\equiv", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_approx.png"))),
		"\\approx", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_simeq.png"))),
		"\\simeq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_ll.png"))),
		"\\ll", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_gg.png"))),
		"\\gg", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_pm.png"))),
		"\\pm", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_in.png"))),
		"\\in", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_ni.png"))),
		"\\ni", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_cap.png"))),
		"\\cap", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_cup.png"))),
		"\\cup", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_bigcap.png"))),
		"\\bigcap", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_bigcup.png"))),
		"\\bigcup", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_subset.png"))),
		"\\subset", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_supset.png"))),
		"\\supset", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_subseteq.png"))),
		"\\subseteq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_supseteq.png"))),
		"\\supseteq", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sum.png"))),
		"\\sum{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sum_from.png"))),
		"\\sum\\limits_{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sum_from_to.png"))),
		"\\sum\\limits_{}^{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_prod.png"))),
		"\\prod{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_prod_from.png"))),
		"\\prod\\limits_{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_prod_from_to.png"))),
		"\\prod\\limits_{}^{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_int.png"))),
		"\\int{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_int_from.png"))),
		"\\int\\limits_{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_int_from_to.png"))),
		"\\int\\limits_{}^{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_oint.png"))),
		"\\oint{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_oint_from.png"))),
		"\\oint\\limits_{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_oint_from_to.png"))),
		"\\oint\\limits_{}^{}{}", relationsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_lim.png"))),
		"\\lim_{}{}", relationsView);
	/*
	(void) new QListWidgetItem(relationsView, "_{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_index_lower.png")));
	(void) new QListWidgetItem(relationsView, "\\sqrt{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sqrt.png")));
	(void) new QListWidgetItem(relationsView, "\\sqrt[]{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sqrt_n.png")));
	(void) new QListWidgetItem(relationsView, "\\geq",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_geq.png")));
	(void) new QListWidgetItem(relationsView, "\\leq",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_leq.png")));
	(void) new QListWidgetItem(relationsView, "\\sim",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sim.png")));
	(void) new QListWidgetItem(relationsView, "\\neq",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_neq.png")));
	(void) new QListWidgetItem(relationsView, "\\equiv",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_equiv.png")));
	(void) new QListWidgetItem(relationsView, "\\approx",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_approx.png")));
	(void) new QListWidgetItem(relationsView, "\\simeq",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_simeq.png")));
	(void) new QListWidgetItem(relationsView, "\\ll",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_ll.png")));
	(void) new QListWidgetItem(relationsView, "\\gg",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_gg.png")));
	(void) new QListWidgetItem(relationsView, "\\pm",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_pm.png")));
	(void) new QListWidgetItem(relationsView, "\\in",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_in.png")));
	(void) new QListWidgetItem(relationsView, "\\ni",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_ni.png")));
	(void) new QListWidgetItem(relationsView, "\\cap",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_cap.png")));
	(void) new QListWidgetItem(relationsView, "\\cup",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_cup.png")));
	(void) new QListWidgetItem(relationsView, "\\bigcap",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_bigcap.png")));
	(void) new QListWidgetItem(relationsView, "\\bigcup",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_bigcup.png")));
	(void) new QListWidgetItem(relationsView, "\\subset",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_subset.png")));
	(void) new QListWidgetItem(relationsView, "\\supset",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_supset.png")));
	(void) new QListWidgetItem(relationsView, "\\subseteq",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_subseteq.png")));
	(void) new QListWidgetItem(relationsView, "\\supseteq",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_supseteq.png")));
	(void) new QListWidgetItem(relationsView, "\\sum{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sum.png")));
	(void) new QListWidgetItem(relationsView, "\\sum\\limits_{}{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sum_from.png")));
	(void) new QListWidgetItem(relationsView, "\\sum\\limits_{}^{}{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_sum_from_to.png")));
	(void) new QListWidgetItem(relationsView, "\\prod{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_prod.png")));
	(void) new QListWidgetItem(relationsView, "\\prod\\limits_{}{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_prod_from.png")));
	(void) new QListWidgetItem(relationsView, "\\prod\\limits_{}^{}{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_prod_from_to.png")));
	(void) new QListWidgetItem(relationsView, "\\int{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_int.png")));
	(void) new QListWidgetItem(relationsView, "\\int\\limits_{}{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_int_from.png")));
	(void) new QListWidgetItem(relationsView, "\\int\\limits_{}^{}{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_int_from_to.png")));
	(void) new QListWidgetItem(relationsView, "\\oint{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_oint.png")));
	(void) new QListWidgetItem(relationsView, "\\oint\\limits_{}{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_oint_from.png")));
	(void) new QListWidgetItem(relationsView, "\\oint\\limits_{}^{}{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_oint_from_to.png")));
	(void) new QListWidgetItem(relationsView, "\\lim_{}{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/relations_tab_lim.png")));
	*/
	
	// Filling Greek Letters tab
	/*
	(void) new QListWidgetItem(greekLettersView, "\\alpha",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_alpha.png")));
	(void) new QListWidgetItem(greekLettersView, "\\beta",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_beta.png")));
	(void) new QListWidgetItem(greekLettersView, "\\gamma",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_gamma.png")));
	(void) new QListWidgetItem(greekLettersView, "\\Gamma",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Gamma.png")));
	(void) new QListWidgetItem(greekLettersView, "\\delta",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_delta.png")));
	(void) new QListWidgetItem(greekLettersView, "\\Delta",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Delta.png")));
	(void) new QListWidgetItem(greekLettersView, "\\epsilon",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_epsilon.png")));
	(void) new QListWidgetItem(greekLettersView, "\\varepsilon",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_varepsilon.png")));
	(void) new QListWidgetItem(greekLettersView, "\\zeta",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_zeta.png")));
	(void) new QListWidgetItem(greekLettersView, "\\eta",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_eta.png")));
	(void) new QListWidgetItem(greekLettersView, "\\theta",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_theta.png")));
	(void) new QListWidgetItem(greekLettersView, "\\vartheta",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_vartheta.png")));
	(void) new QListWidgetItem(greekLettersView, "\\Theta",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Theta.png")));
	(void) new QListWidgetItem(greekLettersView, "\\iota",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_iota.png")));
	(void) new QListWidgetItem(greekLettersView, "\\kappa",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_kappa.png")));
	(void) new QListWidgetItem(greekLettersView, "\\lambda",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_lambda.png")));
	(void) new QListWidgetItem(greekLettersView, "\\Lambda",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Lambda.png")));
	(void) new QListWidgetItem(greekLettersView, "\\mu",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_mu.png")));
	(void) new QListWidgetItem(greekLettersView, "\\nu",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_nu.png")));
	(void) new QListWidgetItem(greekLettersView, "\\xi",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_xi.png")));
	(void) new QListWidgetItem(greekLettersView, "\\Xi",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Xi.png")));
	(void) new QListWidgetItem(greekLettersView, "\\pi",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_pi.png")));
	(void) new QListWidgetItem(greekLettersView, "\\varpi",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_varpi.png")));
	(void) new QListWidgetItem(greekLettersView, "\\Pi",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Pi.png")));
	(void) new QListWidgetItem(greekLettersView, "\\rho",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_rho.png")));
	(void) new QListWidgetItem(greekLettersView, "\\varrho",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_varrho.png")));
	(void) new QListWidgetItem(greekLettersView, "\\sigma",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_sigma.png")));
	(void) new QListWidgetItem(greekLettersView, "\\varsigma",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_varsigma.png")));
	(void) new QListWidgetItem(greekLettersView, "\\Sigma",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Sigma.png")));
	(void) new QListWidgetItem(greekLettersView, "\\tau",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_tau.png")));
	(void) new QListWidgetItem(greekLettersView, "\\upsilon",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_upsilon.png")));
	(void) new QListWidgetItem(greekLettersView, "\\phi",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_phi.png")));
	(void) new QListWidgetItem(greekLettersView, "\\varphi",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_varphi.png")));
	(void) new QListWidgetItem(greekLettersView, "\\Phi",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Phi.png")));
	(void) new QListWidgetItem(greekLettersView, "\\chi",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_chi.png")));
	(void) new QListWidgetItem(greekLettersView, "\\psi",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_psi.png")));
	(void) new QListWidgetItem(greekLettersView, "\\Psi",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Psi.png")));
	(void) new QListWidgetItem(greekLettersView, "\\omega",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_omega.png")));
	(void) new QListWidgetItem(greekLettersView, "\\Omega",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Omega.png")));
	*/
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_alpha.png"))),
		"\\alpha", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_beta.png"))),
		"\\beta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_gamma.png"))),
		"\\gamma", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Gamma.png"))),
		"\\Gamma", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_delta.png"))),
		"\\delta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Delta.png"))),
		"\\Delta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_epsilon.png"))),
		"\\epsilon", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_varepsilon.png"))),
		"\\varepsilon", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_zeta.png"))),
		"\\zeta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_eta.png"))),
		"\\eta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_theta.png"))),
		"\\theta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_vartheta.png"))),
		"\\vartheta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Theta.png"))),
		"\\Theta", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_iota.png"))),
		"\\iota", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_kappa.png"))),
		"\\kappa", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_lambda.png"))),
		"\\lambda", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Lambda.png"))),
			"\\Lambda", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_mu.png"))),
		"\\mu", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_nu.png"))),
		"\\nu", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_xi.png"))),
		"\\xi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Xi.png"))),
		"\\Xi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_pi.png"))),
		"\\pi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_varpi.png"))),
		"\\varpi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Pi.png"))),
		"\\Pi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_rho.png"))),
		"\\rho", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_varrho.png"))),
		"\\varrho", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_sigma.png"))),
		"\\sigma", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_varsigma.png"))),
		"\\varsigma", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Sigma.png"))),
		"\\Sigma", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_tau.png"))),
		"\\tau", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_upsilon.png"))),
		"\\upsilon", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_phi.png"))),
		"\\phi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_varphi.png"))),
		"\\varphi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Phi.png"))),
		"\\Phi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_chi.png"))),
		"\\chi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_psi.png"))),
		"\\psi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Psi.png"))),
		"\\Psi", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_omega.png"))),
		"\\omega", greekLettersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/greek_letters_tab_Omega.png"))),
		"\\Omega", greekLettersView);

	// Filling arrows tab
	/*
	(void) new QListWidgetItem(arrowsView, "\\vec{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_vec.png")));
	(void) new QListWidgetItem(arrowsView, "\\swarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_swarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\leftarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_leftarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\nwarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_nwarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\uparrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_uparrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\nearrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_nearrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\rightarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_rightarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\searrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_searrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\leftrightarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_leftrightarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\updownarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_updownarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\longleftarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_longleftarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\longrightarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_longrightarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\longleftrightarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_longleftrightarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\Leftarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Leftarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\Rightarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Rightarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\Uparrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Uparrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\Downarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Downarrow.png")));	
	(void) new QListWidgetItem(arrowsView, "\\Leftrightarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Leftrightarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\Longleftarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Longleftarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\Longrightarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Longrightarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\Longleftrightarrow",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Longleftrightarrow.png")));
	(void) new QListWidgetItem(arrowsView, "\\leftharpoonup",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_leftharpoonup.png")));
	(void) new QListWidgetItem(arrowsView, "\\leftharpoondown",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_leftharpoondown.png")));
	(void) new QListWidgetItem(arrowsView, "\\rightharpoonup",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_rightharpoonup.png")));
	(void) new QListWidgetItem(arrowsView, "\\rightharpoondown",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_rightharpoondown.png")));
	(void) new QListWidgetItem(arrowsView, "\\rightleftharpoons",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_rightleftharpoons.png")));
	*/
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_vec.png"))),
		"\\vec{}", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_swarrow.png"))),
		"\\swarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_leftarrow.png"))),
		"\\leftarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_nwarrow.png"))),
		"\\nwarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_uparrow.png"))),
		"\\uparrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_nearrow.png"))),
		"\\nearrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_rightarrow.png"))),
		"\\rightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_searrow.png"))),
		"\\searrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_leftrightarrow.png"))),
		"\\leftrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_updownarrow.png"))),
		"\\updownarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_longleftarrow.png"))),
		"\\longleftarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_longrightarrow.png"))),
		"\\longrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_longleftrightarrow.png"))),
		"\\longleftrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Leftarrow.png"))),
		"\\Leftarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Rightarrow.png"))),
		"\\Rightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Uparrow.png"))),
		"\\Uparrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Downarrow.png"))),
		"\\Downarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Leftrightarrow.png"))),
		"\\Leftrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Longleftarrow.png"))),
		"\\Longleftarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Longrightarrow.png"))),
		"\\Longrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_Longleftrightarrow.png"))),
		"\\Longleftrightarrow", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_leftharpoonup.png"))),
		"\\leftharpoonup", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_leftharpoondown.png"))),
		"\\leftharpoondown", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_rightharpoonup.png"))),
		"\\rightharpoonup", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_rightharpoondown.png"))),
		"\\rightharpoondown", arrowsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/arrows_tab_rightleftharpoons.png"))),
		"\\rightleftharpoons", arrowsView);

	// Filling Delimiters tab
	/*
	(void) new QListWidgetItem(delimitersView, "\\lbrace",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_lbrace.png")));
	(void) new QListWidgetItem(delimitersView, "\\rbrace",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_rbrace.png")));
	(void) new QListWidgetItem(delimitersView, "\\langle",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_langle.png")));
	(void) new QListWidgetItem(delimitersView, "\\rangle",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_rangle.png")));
	(void) new QListWidgetItem(delimitersView, "\\hat",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_hat.png")));
	(void) new QListWidgetItem(delimitersView, "\\overbrace{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_overbrace.png")));
	(void) new QListWidgetItem(delimitersView, "\\overbrace{}^{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_overbrace_desc.png")));
	(void) new QListWidgetItem(delimitersView, "\\underbrace{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_underbrace.png")));
	(void) new QListWidgetItem(delimitersView, "\\underbrace{}_{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_underbrace_desc.png")));
	(void) new QListWidgetItem(delimitersView, "\\overline{}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_overline.png")));
	(void) new QListWidgetItem(delimitersView, "\\underline{}",
		QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_underline.png")));
	(void) new QListWidgetItem(delimitersView, "\\left(",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_left_bracket.png")));
	(void) new QListWidgetItem(delimitersView, "\\right)",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_right_bracket.png")));
	(void) new QListWidgetItem(delimitersView, "\\left[",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_left_square_bracket.png")));
	(void) new QListWidgetItem(delimitersView, "\\right]",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_right_square_bracket.png")));
	(void) new QListWidgetItem(delimitersView, "\\left{",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_left_lbrace.png")));
	(void) new QListWidgetItem(delimitersView, "\\right}",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_right_rbrace.png")));
	*/
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_lbrace.png"))),
		"\\lbrace", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_rbrace.png"))),
		"\\rbrace", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_langle.png"))),
		"\\langle", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_rangle.png"))),
		"\\rangle", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_hat.png"))),
		"\\hat", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_overbrace.png"))),
		"\\overbrace{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_overbrace_desc.png"))),
		"\\overbrace{}^{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_underbrace.png"))),
		"\\underbrace{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_underbrace_desc.png"))),
		"\\underbrace{}_{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_overline.png"))),
		"\\overline{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_underline.png"))),
		"\\underline{}", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_left_bracket.png"))),
		"\\left(", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_right_bracket.png"))),
		"\\right)", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_left_square_bracket.png"))),
		"\\left[", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_right_square_bracket.png"))),
		"\\right]", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_left_lbrace.png"))),
		"\\left{", delimitersView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/delimiters_tab_right_rbrace.png"))),
		"\\right}", delimitersView);

	// Filling Symbols tab
	/*
	(void) new QListWidgetItem(symbolsView, "\\infty",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_infty.png")));
	(void) new QListWidgetItem(symbolsView, "\\cdot",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_cdot.png")));
	(void) new QListWidgetItem(symbolsView, "\\cdots",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_cdots.png")));
	(void) new QListWidgetItem(symbolsView, "\\ldots",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_ldots.png")));
	(void) new QListWidgetItem(symbolsView, "\\ddots",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_ddots.png")));
	(void) new QListWidgetItem(symbolsView, "\\hbar",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_hbar.png")));
	(void) new QListWidgetItem(symbolsView, "\\nabla",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_nabla.png")));
	(void) new QListWidgetItem(symbolsView, "\\partial",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_partial.png")));
	(void) new QListWidgetItem(symbolsView, "\\aleph",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_aleph.png")));
	(void) new QListWidgetItem(symbolsView, "\\Re",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_Re.png")));
	(void) new QListWidgetItem(symbolsView, "\\Im",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_Im.png")));
	(void) new QListWidgetItem(symbolsView, "\\bot",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_bot.png")));
	(void) new QListWidgetItem(symbolsView, "\\emptyset",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_emptyset.png")));
	(void) new QListWidgetItem(symbolsView, "\\prime",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_prime.png")));
	(void) new QListWidgetItem(symbolsView, "\\surd",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_surd.png")));
	(void) new QListWidgetItem(symbolsView, "\\angle",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_angle.png")));
	(void) new QListWidgetItem(symbolsView, "\\forall",
			QPixmap(dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_forall.png")));
	*/
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_infty.png"))),
		"\\infty", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_cdot.png"))),
		"\\cdot", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_cdots.png"))),
		"\\cdots", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_ldots.png"))),
		"\\ldots", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_ddots.png"))),
		"\\ddots", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_hbar.png"))),
		"\\hbar", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_nabla.png"))),
		"\\nabla", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_partial.png"))),
		"\\partial", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_aleph.png"))),
		"\\aleph", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_Re.png"))),
		"\\Re", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_Im.png"))),
		"\\Im", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_bot.png"))),
		"\\bot", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_emptyset.png"))),
		"\\emptyset", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_prime.png"))),
		"\\prime", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_surd.png"))),
		"\\surd", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_angle.png"))),
		"\\angle", symbolsView);
	(void) new QListWidgetItem(QIcon(QPixmap(
		dataPath("kadu/modules/data/mime_tex/mime_tex_icons/symbols_tab_forall.png"))),
		"\\forall", symbolsView);

	
	undoButton = new QPushButton(QIconSet(QPixmap(dataPath("kadu/modules/data/mime_tex/editor_icons/undo.png"))),
			tr("&Undo"), this, "undo_button");
	redoButton = new QPushButton(QIconSet(QPixmap(dataPath("kadu/modules/data/mime_tex/editor_icons/redo.png"))),
			tr("&Redo"), this, "redo_button");
	QPushButton *okButton = new QPushButton(tr("&Ok"), this, "ok_button");
	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this, "cancel_button");
	
	formulaTextEdit = new QTextEdit(this, "formula_text_edit");
	formulaTextEdit->setMinimumHeight(formulaTextEditMinimunHeight);
	mainLayout->addWidget(formulaTextEdit);
	connect(&timer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
	
	QHBoxLayout *buttonsLayout = new QHBoxLayout(mainLayout, 3, "buttons_layout");
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
	connect(mimeTeX, SIGNAL(deleting()), this, SLOT(deleteLater()));

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
		QString formulaStr = formulaTextEdit->text();
		formulaStr.replace(QChar('\n'), QChar(' '));
		if(!config_file.readBoolEntry("MimeTeX", "mimetex_transparent_bg", false))
			arguments << "-o";
		arguments << "-s";
		arguments << QString::number(config_file.readNumEntry("MimeTeX", "mimetex_font_size",
			MimeTeX::MimeTeX::defaultFontSize()));
		arguments << "-e";
		arguments << tmpFileName;
		arguments << formulaStr;

		QStringList::const_iterator it;
		int i = 0;
		for(it = arguments.constBegin(); it != arguments.constEnd(); ++it)
			kdebugm(KDEBUG_INFO, "Arg[%d]=%s\n", i++, (*it).toLocal8Bit().constData());
		mimeTeXProcess.start(libPath("kadu/modules/bin/mime_tex/mimetex"),
			arguments);
		mimeTeXProcess.waitForStarted();
	}

	kdebugf2();
}

void MimeTeX::TeXFormulaDialog::insertComponentSlot(QListWidgetItem *item)
{
	kdebugf();
	if(item)
		formulaTextEdit->insert(item->text());
	formulaTextEdit->setFocus();
	kdebugf2();
}

void MimeTeX::TeXFormulaDialog::updateButtons()
{
	undoButton->setEnabled(formulaTextEdit->isUndoAvailable());
	redoButton->setEnabled(formulaTextEdit->isRedoAvailable());
}

void MimeTeX::TeXFormulaDialog::errorMessage(QProcess::ProcessError error)
{
	QMessageBox::critical(this, tr("TeX formula creator"), tr("Unable to run mimetex binary!"),
		QMessageBox::Ok, 0);
}

int MimeTeX::TeXFormulaDialog::tmpFileNumber = 0;
