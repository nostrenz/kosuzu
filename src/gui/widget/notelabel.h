#ifndef NOTELABEL_H
#define NOTELABEL_H

#include <src/gui/widget/tooltiplabel.h>
#include <src/data/note.h>
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
		QString body(float scaleRatio=1, bool isEmbeded=false) const;
		float fontScale() const;
		void renderVertically();
		void setBackgroundStyle(BackgroundStyle, float scaleRatio=1);
		void showTooltipLabel();
		void hideTooltipLabel();

	private:
		Note* m_note;
		BackgroundStyle m_backgroundStyle = BackgroundStyle::Danbooru;
		QString m_backgroundColor;
		float m_fontScale = 1;
		void parseBody();

	private slots:
		void enterEvent(QEvent*);
		void leaveEvent(QEvent*);

	signals:
		void showTooltip(NoteLabel*);
		void hideTooltip();
};

#endif // NOTELABEL_H
