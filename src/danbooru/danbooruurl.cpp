#include <src/danbooru/danbooruurl.h>
#include <src/utils.h>

///
/// Constructor / Destructor
///

DanbooruUrl::DanbooruUrl(QString url)
{
	// Invalid URL
	if (url.isNull() || url.isEmpty()) {
		return;
	}

	// Provided string is a number
	int id = this->numeric(url);

	if (id > 0) {
		m_poolId = id;
		m_postId = id;
		m_isAmbiguous = true;

		return;
	}

	if (url.contains("/pools/")) {
		m_poolId = this->numeric(utils::regex(url, "\\/pools\\/([0-9]+)", 1));

		return;
	}

	if (url.contains("pool_id=")) {
		m_poolId = this->numeric(utils::regex(url, "pool_id=([0-9]+)", 1));
	}

	if (url.contains("pool%3A")) {
		m_poolId = this->numeric(utils::regex(url, "pool%3A([0-9]+)", 1));
	}

	if (url.contains("/posts/")) {
		m_postId = this->numeric(utils::regex(url, "\\/posts\\/([0-9]+)", 1));
	}
}

DanbooruUrl::DanbooruUrl(const unsigned int poolId, const unsigned int postId)
{
	m_poolId = poolId,
	m_postId = postId;
}

///
/// Public method
///

bool DanbooruUrl::isPoolOnly() const
{
	return m_poolId > 0 && m_postId == 0;
}

bool DanbooruUrl::isPostOnly() const
{
	return m_poolId == 0 && m_postId > 0;
}

bool DanbooruUrl::isPostWithPool() const
{
	return m_poolId > 0 && m_postId > 0;
}

bool DanbooruUrl::isValid() const
{
	return m_poolId > 0 || m_postId > 0;
}

QString DanbooruUrl::url() const
{
	if (this->isPostWithPool()) {
		return this->postWithPoolUrl();
	}

	if (this->isPostOnly()) {
		return this->postUrl();
	}

	if (this->isPoolOnly()) {
		return this->poolUrl();
	}

	return QString();
}

unsigned int DanbooruUrl::poolId() const
{
	return m_poolId;
}

unsigned int DanbooruUrl::postId() const
{
	return m_postId;
}

void DanbooruUrl::keepPoolId()
{
	m_postId = 0;
}

void DanbooruUrl::keepPostId()
{
	m_poolId = 0;
}

QString DanbooruUrl::poolUrl() const
{
	return "https://danbooru.donmai.us/pools/" + QString::number(m_poolId);
}

QString DanbooruUrl::postUrl() const
{
	return "https://danbooru.donmai.us/posts/" + QString::number(m_postId);
}

QString DanbooruUrl::postWithPoolUrl() const
{
	return "https://danbooru.donmai.us/posts/" + QString::number(m_postId) + "?pool_id=" + QString::number(m_poolId);
}

void DanbooruUrl::setPostId(const unsigned int id)
{
	m_postId = id;
}

/**
 * Returns true if it is not clear if the user intent is to download the whole pool or just this post.
 */
bool DanbooruUrl::isAmbiguous() const
{
	if (m_isAmbiguous) {
		return m_isAmbiguous;
	}

	return this->isPostWithPool();
}

///
/// Private method
///

/**
 * Convert a string to a number.
 *
 * @param str:
 */
unsigned int DanbooruUrl::numeric(QString str) const
{
	bool valid;

	int decimal = str.toUInt(&valid, 10);

	return valid ? decimal : 0;
}
