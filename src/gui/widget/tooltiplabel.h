#ifndef POPUPLABEL_H
#define POPUPLABEL_H

#include <QLabel>

///
/// The TooltipLabel is displayed in the Viewer when hovering a NoteLabel.
///

class TooltipLabel : public QLabel
{
	Q_OBJECT

	public:
		TooltipLabel();
		void setAppearance();
};

#endif // TOOLTIPLABEL_H
