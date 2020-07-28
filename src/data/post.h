#ifndef POST_H
#define POST_H

#include <QString>
#include <QVector>
#include <QPixmap>
#include "note.h"

class Post
{
	public:
		Post();
		~Post();
		unsigned int id() const;
		QVector<Note*> notes() const;
		QPixmap pixmap() const;
		void setId(unsigned int id);
		void setPixmap(QPixmap pixmap);
		void addNote(Note* note);

	private:
		unsigned int m_id = 0;
		QPixmap m_pixmap;
		QVector<Note*> m_notes;
};

#endif // POST_H
