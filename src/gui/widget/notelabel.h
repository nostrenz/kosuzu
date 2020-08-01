#ifndef NOTELABEL_H
#define NOTELABEL_H

#include <src/gui/widget/tooltiplabel.h>
#include <src/data/note.h>
#include <src/notetag.h>
#include <QLabel>

///
/// This is a GUI widget used to display a Note in the Viewer.
///

class NoteLabel : public QLabel
{
	Q_OBJECT

	public:
		enum BackgroundStyle {
			Danbooru = 1,
			Embeded  = 2
		};
		Q_ENUM(BackgroundStyle)
		NoteLabel(Note*, QString fontFamily);
		~NoteLabel();
		Note* note();
		unsigned int offsetX() const;
		unsigned int offsetY() const;
		QString body(int fontSize, float scaleRatio=1) const;
		void renderVertically();
		void setBackgroundStyle(BackgroundStyle, int fontSize, float scaleRatio=1);
		void showTooltipLabel();
		void hideTooltipLabel();

	private:
		Note* m_note;
		QVector<NoteTag*> m_tags;
		QString m_backgroundColor;
		BackgroundStyle m_backgroundStyle = BackgroundStyle::Danbooru;

	private slots:
		void enterEvent(QEvent*);
		void leaveEvent(QEvent*);

	signals:
		void showTooltip(NoteLabel*);
		void hideTooltip();
};

#endif // NOTELABEL_H
