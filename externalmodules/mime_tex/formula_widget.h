#ifndef FORMULA_WIDGET_H
#define FORMULA_WIDGET_H

#include <qwidget.h>
#include <qstring.h>
#include <qpixmap.h>

class QPainter;

namespace MimeTeX
{
	/*!
	 * A widget representing the TeX formula's view of the GIF image
	 * Improtant public function is fillView() slot. When called, it draws
	 * the formula's GIF inside the view.
	 * \brief This class represents TeX formula's view in the TeX formula's dialog
	 */
	class FormulaWidget : public QWidget
	{
		Q_OBJECT
		public:
			/*!
			 * \brief Default constructor
			 * \param fileName Name for a GIF file
			 * \param parent Parent widget
			 */
			FormulaWidget(QString fileName, QWidget *parent = 0);

			//! Default destructor
			~FormulaWidget();

			public slots:
				//! Cleans the view
				void cleanView(QPainter &p);

			/*!
			 * This slot calles the cleanView() automaticly before
			 * filling the view, so there's no need to clean manually
			 * before calling fillView()
			 * \brief Fills the view with the proper GIF
			 */
			void fillView(QPainter &p);

		protected:
			/*!
			 * \brief Paints the view's contents
			 * \param paintEvent Unused here
			 */
			virtual void paintEvent (QPaintEvent *paintEvent);

		private:
			//! GIF's name
			QString tmpFileName;

			//! Temporary pixmap holding the formula image
			QPixmap formulaPixmap;

			//! An empty white pixmap used for cleaning the view
			QPixmap cleanPixmap;

			//! Left and top margin
			static const int margin;
	};
}

#endif
