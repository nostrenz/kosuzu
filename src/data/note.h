#ifndef NOTE_H
#define NOTE_H

#include <QString>

///
/// This class represents a Danbooru note.
/// Notes are boxes placed on top of an image to provide translations.
/// This is used to store the text, position and size informations of a note.
/// For the actual GUI note, look for NoteLabel.
///

class Note
{
	public:
		Note(QString body, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		QString body() const;
		unsigned int x() const;
		unsigned int y() const;
		unsigned int width() const;
		unsigned int height() const;
		void setX(unsigned int x);
		void setY(unsigned int y);
		void setWidth(unsigned int width);
		void setHeight(unsigned int height);

	private:
		QString m_body;
		unsigned int m_x = 0;
		unsigned int m_y = 0;
		unsigned int m_width = 0;
		unsigned int m_height = 0;
};

#endif // NOTE_H
