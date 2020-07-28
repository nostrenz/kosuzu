#include <src/data/post.h>

Post::Post()
{
}

Post::~Post()
{
	qDeleteAll(m_notes);
	m_notes.clear();
}

unsigned int Post::id() const
{
	return m_id;
}

QPixmap Post::pixmap() const
{
	return m_pixmap;
}

QVector<Note*> Post::notes() const
{
	return m_notes;
}

void Post::setId(const unsigned int id)
{
	m_id = id;
}

void Post::setPixmap(QPixmap pixmap)
{
	m_pixmap = pixmap;
}

void Post::addNote(Note* note)
{
	m_notes.append(note);
}
