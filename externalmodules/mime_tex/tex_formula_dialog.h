#ifndef TEX_FORMULA_DIALOG_H
#define TEX_FORMULA_DIALOG_H

#include <QDialog>
#include <QString>
#include <QTimer>
#include <QProcess>

class ChatWidget;
class QTextEdit;
class QListWidgetItem;

namespace MimeTeX
{
	class FormulaView;

	/*!
	 * This is TeX formula editor class. An object of this class is created
	 * each time, user presses "TeX formula" action button.
	 * \brief TeX formula dialog class
	 */
	class TeXFormulaDialog : public QDialog
	{
		Q_OBJECT
		public:
			/*!
			 * \brief Default constructor
			 * \param *parent parent widget (chat window)
			 * \param f dialog's flags. The WDestructiveClose and WType_TopLevel
			 * are always automaticly set.
			 */
			TeXFormulaDialog(ChatWidget *parent = 0, Qt::WindowFlags f = 0);

			//!	Default destructor
			~TeXFormulaDialog();

			//! Returns temporary gif file name.
			/*!
			 * \returns file's name
			 */
			QString getTmpFileName() const { return tmpFileName; }

			private slots:
				//! This slot is called each time, the "TeX formula" action button is pressed
				void okClickedSlot();

			//! Called periodicly to update formula's widget. It runs mimetex process
			void timeoutSlot();

			//! Called each time, component icon is clicked
			void insertComponentSlot(QListWidgetItem *);

		private:
			FormulaView *formulaView;
			QString formula;
			ChatWidget *chat;
			QTextEdit *formulaTextEdit;
			QTimer timer;
			QProcess mimeTeXProcess;
			QString tmpFileName;
			static int tmpFileNumber;
			QPushButton *undoButton;
			QPushButton *redoButton;

		private slots:
			//! Updates Redo and Undo buttons
			void updateButtons();
			void errorMessage(QProcess::ProcessError error);
	};
}


#endif
