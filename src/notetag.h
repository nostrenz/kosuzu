#ifndef NOTETAG_H
#define NOTETAG_H

#include <QString>

class NoteTag
{
	public:
		NoteTag(QString html, int startIndex, int endIndex, NoteTag* parent=nullptr);
		NoteTag* parent();
		QString style();
		QString backgroundColor() const;
		float fontScale() const;
		int styleStart() const;
		int styleEnd() const;
		QString modifyStyle(int fontSize, float scaleRatio);

	private:
		NoteTag* m_parent = nullptr;
		QString m_style;
		QString m_backgroundColor;
		QString m_fontSize;
		float m_fontScale = 1;
		int m_styleStart = 0;
		int m_styleEnd = 0;
		float fontScaleFromParent(NoteTag* parent, float fontScale=1);
};

#endif // NOTETAG_H
