#ifndef FORMULA_VIEW_H
#define FORMULA_VIEW_H

#include <QScrollArea>

namespace MimeTeX
{
	class FormulaWidget;

	/*!
	 * \brief This class provides the view of the formula widget image
	 *
	 * \see FormulaWidget
	 */
	class FormulaView : public QScrollArea
	{
		Q_OBJECT
		public:
			/*!
			 * \brief Constructor of the class
			 * \param tmpFileName FormulaView class is a visual
			 *        reprezentation of the gif image containing
			 *        the math formula. This parameter is required
			 *        and contains the name of gif file.
			 *        
			 * \param parent Parent widget. If specified, will be
			 *        set as a parent for the view.
			 */
			FormulaView(QString tmpFileName, QWidget *parent = 0);

			//! Default destructor
			virtual ~FormulaView();

			public slots:
				//! Invalidates the formula widget
				void fillView();

		protected:

			//! Instance of FormulaWidget, that represents the GIF image
			FormulaWidget *formulaWidget;
	};
}

#endif
