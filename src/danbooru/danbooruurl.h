#ifndef DANBOORUURL_H
#define DANBOORUURL_H

#include <QString>

///
/// This class represents a Danbooru URL.
///

QT_BEGIN_NAMESPACE
namespace Danbooru { class DanbooruUrl; }
QT_END_NAMESPACE

class DanbooruUrl
{
	public:
		DanbooruUrl(QString url);
		DanbooruUrl(const unsigned int poolId, const unsigned int postId);
		bool isPoolOnly() const;
		bool isPostOnly() const;
		bool isPostWithPool() const;
		bool isValid() const;
		unsigned int poolId() const;
		unsigned int postId() const;
		void keepPoolId();
		void keepPostId();
		QString url() const;
		QString poolUrl() const;
		QString postUrl() const;
		QString postWithPoolUrl() const;
		void setPostId(const unsigned int id);
		bool isAmbiguous() const;

	private:
		unsigned int m_poolId = 0;
		unsigned int m_postId = 0;
		bool m_isAmbiguous = false;
		unsigned int numeric(QString str) const;
};

#endif // DANBOORUURL_H
