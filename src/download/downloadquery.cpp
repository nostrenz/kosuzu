#include <src/download/downloadquery.h>
#include <src/ksz/kszsignature.h>

DownloadQuery::DownloadQuery(DanbooruUrl* url)
{
	m_url = url;
}

DownloadQuery::DownloadQuery(Ksz* ksz, unsigned int postId)
{
	m_url = new DanbooruUrl(ksz->poolId(), postId);
	m_kszSignature = KszSignature::read(ksz->path());

	this->setKszPath(ksz->path());
	this->setDownloadedPosts(ksz->pages());

	if (ksz->pages() == 1) {
		this->setTitle("Post " + QString::number(postId));
	} else {
		this->setTitle(ksz->title());
	}
}

DownloadQuery::~DownloadQuery()
{
	delete m_url;
}

///
/// Public method
///

DanbooruUrl* DownloadQuery::url() const
{
	return m_url;
}

QString DownloadQuery::title() const
{
	return m_title;
}

QString DownloadQuery::kszPath() const
{
	return m_kszPath;
}

unsigned int DownloadQuery::downloadedPosts() const
{
	return m_downloadedPosts;
}

unsigned int DownloadQuery::postCount() const
{
	return m_postCount;
}

QString DownloadQuery::kszSignature() const
{
	return m_kszSignature;
}

void DownloadQuery::setTitle(QString title)
{
	m_title = title;
}

void DownloadQuery::setPostCount(unsigned int postCount)
{
	m_postCount = postCount;
}

void DownloadQuery::setDownloadedPosts(unsigned int downloadedPosts)
{
	m_downloadedPosts = downloadedPosts;
}

void DownloadQuery::setKszPath(QString filePath)
{
	m_kszPath = filePath;
}

bool DownloadQuery::isValid()
{
	return (m_postCount != 0 && this->hasPostIds() && !m_title.isNull());
}

bool DownloadQuery::isNotStarted() const
{
	return (m_downloadedPosts == 0 && m_running == false);
}

bool DownloadQuery::isRunning()
{
	if (this->isCompleted()) {
		m_running = false;
	}

	return m_running;
}

bool DownloadQuery::isPaused() const
{
	return !m_running && m_downloadedPosts > 0;
}

bool DownloadQuery::isCompleted() const
{
	return m_downloadedPosts == m_postCount;
}

void DownloadQuery::setAsRunning()
{
	m_running = true;
}

void DownloadQuery::setAsPaused()
{
	m_running = false;
}

void DownloadQuery::postDownloaded()
{
	m_downloadedPosts++;
}

void DownloadQuery::addPostId(int postId)
{
	m_postIds.append(postId);
}

int DownloadQuery::postIdOfPage(int page) const
{
	return m_postIds[page];
}

bool DownloadQuery::hasPostIds() const
{
	return m_postIds.length() > 0;
}

void DownloadQuery::reset()
{
	m_kszPath = QString();
	m_downloadedPosts = 0;
}
