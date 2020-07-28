#ifndef KSZ_H
#define KSZ_H

#include <QString>

///
/// This class holds basic infos about a KSZ file.
///

class Ksz
{
	public:
		Ksz();
		QString path() const;
		QString title();
		unsigned int poolId() const;
		unsigned int postId() const;
		unsigned int pages() const;
		unsigned int page() const;
		void setPath(QString);
		void setTitle(QString);
		void setPoolId(const unsigned int);
		void setPostId(const unsigned int);
		void setPages(unsigned int);
		void setPage(unsigned int);

	private:
		QString m_path = "";
		QString m_title = "";
		unsigned int m_poolId = 0;
		unsigned int m_postId = 0;
		unsigned int m_pages = 0;
		unsigned int m_page = 1;
};

#endif // KSZ_H
