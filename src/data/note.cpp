#include <src/data/note.h>

Note::Note(QString body, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	m_body = body;
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
}

/*
Public method
*/

QString Note::body() const
{
	return m_body;
}

unsigned int Note::x() const
{
	return m_x;
}

unsigned int Note::y() const
{
	return m_y;
}

unsigned int Note::width() const
{
	return m_width;
}

unsigned int Note::height() const
{
	return m_height;
}

void Note::setBody(QString body)
{
	m_body = body;
}

void Note::setX(unsigned int x)
{
	m_x = x;
}

void Note::setY(unsigned int y)
{
	m_y = y;
}
void Note::setWidth(unsigned int width)
{
	m_width = width;
}

void Note::setHeight(unsigned int height)
{
	m_height = height;
}
