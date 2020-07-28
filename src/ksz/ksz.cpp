#include <src/ksz/ksz.h>
#include <src/utils.h>

///
/// Constructor
///

Ksz::Ksz()
{
}

///
/// Public method
///

QString Ksz::path() const
{
	return m_path;
}

QString Ksz::title()
{
	if (!m_title.isEmpty()) {
		return m_title;
	}

	if (m_path.isNull()) {
		return "<Unnamed>";
	}

	// Use filename as title
	m_title = utils::basename(m_path);

	return m_title;
}

unsigned int Ksz::poolId() const
{
	return m_poolId;
}

unsigned int Ksz::postId() const
{
	return m_postId;
}

unsigned int Ksz::pages() const
{
	return m_pages;
}

unsigned int Ksz::page() const
{
	return m_page;
}

void Ksz::setPath(QString path)
{
	m_path = path;
}

void Ksz::setTitle(QString title)
{
	m_title = title;
}

void Ksz::setPoolId(const unsigned int poolId)
{
	m_poolId = poolId;
}

void Ksz::setPostId(const unsigned int postId)
{
	m_postId = postId;
}

void Ksz::setPages(unsigned int pages)
{
	m_pages = pages;
}

void Ksz::setPage(unsigned int page)
{
	m_page = page;
}
