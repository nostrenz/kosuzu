#include <src/download/downloader.h>
#include <src/download/downloadquery.h>
#include <src/ksz/kszwriter.h>
#include <src/collection.h>
#include <src/utils.h>
#include <QFile>

///
/// This class is responsible for parsing Danbooru pages
/// and downloader their content.
///

// From src/danbooru/danbooruapi.cpp
extern const QString USER_AGENT;

Downloader::Downloader() : QObject()
{
	m_danbooruApi = new DanbooruApi();

	QObject::connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFileDownloadFinished(QNetworkReply*)));
}

Downloader::~Downloader()
{
	if (m_danbooruApi != nullptr) {
		delete m_danbooruApi;
	}

	if (m_kszWriter != nullptr) {
		delete m_kszWriter;
	}

	qDeleteAll(m_queries);
}

///
/// Public method
///

/**
 * Add a new URL to the download list.
 */
bool Downloader::add(DanbooruUrl* url)
{
	DownloadQuery* query = new DownloadQuery(url);

	return this->add(query);
}

/**
 * Add an existing query to be resumed.
 */
bool Downloader::add(DownloadQuery* query)
{
	// Single post download
	if (query->url()->isPostOnly()) {
		query->setTitle("Post " + QString::number(query->url()->postId()));
		query->setPostCount(1);
		query->addPostId(query->url()->postId());

		return this->addReady(query);
	}

	// Pool download
	this->updateQueryFromApi(query);

	if (!query->isValid()) {
		return false;
	}

	return this->addReady(query);
}

/**
 * Add a query without checking with the API.
 */
bool Downloader::addReady(DownloadQuery* query)
{
	m_queries.append(query);

	emit downloadAdded(query);

	return true;
}

void Downloader::remove(DownloadQuery* query)
{
	m_queries.removeAll(query);

	delete query;
}

/**
 * Get basic infos about the pool the add it to the download queue.
 */
void Downloader::queue(DownloadQuery* query)
{
	m_queue.append(query);
}

/**
 * Start downloading queued queries, starting with the first non-completed and non-paused one.
 */
bool Downloader::start()
{
	if (m_queries.empty()) {
		return false;
	}

	// Look for paused downloads in the queue
	foreach (DownloadQuery* query, m_queue) {
		if (!query->isRunning() && !query->isCompleted() && query->isPaused()) {
			m_currentQuery = query;
			m_currentQuery->setAsRunning();

			return this->downloadQuery(m_currentQuery);
		}
	}

	// Add downloads to the queue
	foreach (DownloadQuery* query, m_queries) {
		if (!query->isRunning() && !query->isCompleted() && !this->isQueued(query)) {
			this->queue(query);
		}
	}

	// Nothing was added to the queue
	if (m_queue.isEmpty()) {
		return false;
	}

	// There's running downloads, just add to the queue
	if (this->isRunning()) {
		return true;
	}

	// Nothing is already being downloaded right now, we can start the first query right away
	m_currentQuery = m_queue.first();
	m_currentQuery->setAsRunning();

	return this->downloadQuery(m_currentQuery);
}

/**
 * Pause the query currently being downloaded.
 */
void Downloader::pause()
{
	if (m_queue.empty()) {
		return;
	}

	foreach (DownloadQuery* query, m_queue) {
		if (query->isRunning() && !query->isCompleted() && !query->isPaused()) {
			query->setAsPaused();
		}
	}
}

void Downloader::stop()
{
	if (m_queue.empty()) {
		return;
	}

	foreach (DownloadQuery* query, m_queue) {
		if (!query->isPaused()) {
			this->stop(query);
		}
	}
}

/**
 * Clear the download queue.
 */
void Downloader::clear()
{
	while (!m_queries.isEmpty()) {
		this->remove(m_queries[0]);
	}

	m_queue.clear();
	m_queries.clear();
}

/**
 * Start downloading the given query.
 */
bool Downloader::start(DownloadQuery* query)
{
	// Not already in the queue
	if (m_queue.indexOf(query) < 0) {
		this->queue(query);
	}

	// There's running download, just add to que queue
	if (this->isRunning()) {
		return true;
	}

	// Nothing is already being downloaded right now, we can start this query right away
	query->setAsRunning();

	m_currentQuery = query;

	return this->downloadQuery(query);
}

/**
 * Pause the given query.
 */
void Downloader::pause(DownloadQuery* query)
{
	query->setAsPaused();
}

void Downloader::stop(DownloadQuery* query)
{
	query->setAsPaused();

	this->dequeue(query);
}

DownloadQuery* Downloader::currentQuery() const
{
	return m_currentQuery;
}

bool Downloader::hasQuery(DownloadQuery* query) const
{
	foreach (DownloadQuery* q, m_queries) {
		if (query->url()->poolId() == q->url()->poolId()) {
			return true;
		}

		if (query->url()->postId() == q->url()->postId()) {
			return true;
		}
	}

	return false;
}

/**
 * Check if there's a query with the given pool ID already in the queue.
 */
bool Downloader::hasQueryWithUrl(DanbooruUrl* url) const
{
	foreach (DownloadQuery* query, m_queries) {
		if (url->poolId() > 0 && url->poolId() == query->url()->poolId()) {
			return true;
		}

		if (url->postId() > 0 && url->postId() == query->url()->postId()) {
			return true;
		}
	}

	return false;
}

/**
 * Check if a query is currently being downloaded.
 */
bool Downloader::isRunning() const
{
	if (m_queue.isEmpty()) {
		return false;
	}

	foreach (DownloadQuery* query, m_queue) {
		if (query->isCompleted()) {
			continue;
		}

		if (query->isPaused()) {
			continue;
		}

		if (query->isRunning()) {
			return true;
		}
	}

	return false;
}

/**
 * Check if there's non-completed downloads in the queue.
 */
bool Downloader::hasIncompleteDownloads(const bool skipNotStarted) const
{
	if (m_queries.isEmpty()) {
		return false;
	}

	foreach (DownloadQuery* query, m_queries) {
		if (skipNotStarted && query->isNotStarted()) {
			continue;
		}

		if (!query->isCompleted()) {
			return true;
		}
	}

	return false;
}

/**
 * Check if a query is already in the queue.
 */
bool Downloader::isQueued(DownloadQuery* query) const
{
	return m_queue.indexOf(query) >= 0;
}

/**
 * Get a query at the given index.
 */
DownloadQuery* Downloader::queryAt(int index) const
{
	return m_queries[index];
}

/**
 * How many queries are in the queue.
 */
int Downloader::queueLength() const
{
	return m_queue.length();
}

/**
 * Get the index of a given query in the list.
 */
int Downloader::indexOfQuery(DownloadQuery* query) const
{
	return m_queries.indexOf(query);
}

/**
 * Index of a query in the queue.
 */
int Downloader::placeInQueue(DownloadQuery* query) const
{
	return m_queue.indexOf(query);
}

int Downloader::queryCount() const
{
	return m_queries.count();
}

///
/// Private method
///

bool Downloader::downloadQuery(DownloadQuery* query)
{
	// Renew writer
	if (m_kszWriter != nullptr) {
		delete m_kszWriter;
		m_kszWriter = nullptr;
	}

	// Missing values in the query, update it
	if (!query->isValid()) {
		this->updateQueryFromApi(query);

		// Still not valid
		if (!query->isValid()) {
			return false;
		}
	}

	QString title = query->title();
	utils::fileSystemSafeFileName(title);

	if (query->kszPath().isNull()) {
		query->setKszPath(utils::downloadDir(true).absolutePath() + "/" + title + ".ksz");
	}

	m_kszWriter = new KszWriter(query->kszPath());

	if (!m_kszWriter->isOpen()) {
		emit statusMessage("Error: unable to open KSZ file for writing.");

		return false;
	}

	query->setKszPath(m_kszWriter->filePath());

	return this->downloadPost();
}

bool Downloader::downloadFile(QString fileUrl)
{
	QUrl url = QUrl::fromEncoded(fileUrl.toLocal8Bit());

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);

	auto reply = m_manager.get(request);

	QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onFileDownloadProgress(qint64,qint64)));

	return true;
}

QString Downloader::pageName(unsigned int index)
{
	QString page = QString::number(index + 1);

	return page/*.rightJustified(4, '0')*/;
}

bool Downloader::downloadPost()
{
	int postId = m_currentQuery->postIdOfPage(m_currentQuery->downloadedPosts());
	QJsonObject postJsonObject = m_danbooruApi->getPostJson(postId);

	// File cannot be downloaded, skip it
	// Happens when a gold account is needed
	// TODO: implement login
	if (!postJsonObject.contains("file_url")) {
		this->afterFileDownload();

		return false;
	}

	// Post data
	QString fileUrl = postJsonObject.value("file_url").toString();
	//QString lastNotedAt = postJsonObject.value("last_noted_at").toString();
	//QString fileSize = postJsonObject.value("file_size").toString();
	//QString updatedAt = postJsonObject.value("updated_at").toString();

	QJsonArray notesJsonArray = m_danbooruApi->getNotesJson(postId);
	QVector<Note> notes;

	foreach (const QJsonValue &v, notesJsonArray) {
		QJsonObject noteJsonObject = v.toObject();

		// Ignore inactive notes
		if (!noteJsonObject.value("is_active").toBool()) {
			continue;
		}

		//QString noteId = noteJsonObject.value("id").toString();
		//QString updatedAt = noteJsonObject.value("updated_at").toString();

		notes.append(Note(
			noteJsonObject.value("body").toString(),
			noteJsonObject.value("x").toInt(),
			noteJsonObject.value("y").toInt(),
			noteJsonObject.value("width").toInt(),
			noteJsonObject.value("height").toInt()
		));
	}

	m_kszWriter->addPageXml(postId, notes, m_currentQuery->downloadedPosts()+1);

	// Download image
	return this->downloadFile(fileUrl);
}

void Downloader::dequeue(DownloadQuery* query)
{
	m_queue.removeAll(query);
}

/**
 * Call the API to update a query.
 */
void Downloader::updateQueryFromApi(DownloadQuery* query)
{
	QJsonObject jsonObject = m_danbooruApi->getPoolJson(query->url()->poolId());

	if (jsonObject.isEmpty()) {
		return;
	}

	int id = jsonObject.value("id").toInt();
	QString name = jsonObject.value("name").toString();
	QString updated = jsonObject.value("updated_at").toString();
	int postCount = jsonObject.value("post_count").toInt();
	QJsonArray postIds = jsonObject.value("post_ids").toArray();

	foreach (const QJsonValue &v, postIds) {
		query->addPostId(v.toInt());
	}

	query->setTitle(name.replace("_", " "));
	query->setPostCount(postCount);
}

void Downloader::afterFileDownload()
{
	m_currentQuery->postDownloaded();

	emit postDownloaded(m_currentQuery);

	// All posts downloaded
	if (m_currentQuery->isCompleted()) {
		// Only add ksz.xml if it's a new file
		if (m_currentQuery->kszSignature().isNull()) {
			m_kszWriter->addMetaXml(m_currentQuery->title(), m_currentQuery->url()->poolId());
		}

		QString kszPath = m_kszWriter->close(true);

		m_currentQuery->setKszPath(kszPath);

		Ksz ksz;
		ksz.setTitle(m_currentQuery->title());
		ksz.setPoolId(m_currentQuery->url()->poolId());
		ksz.setPostId(m_currentQuery->url()->postId());
		ksz.setPath(m_currentQuery->kszPath());
		ksz.setPages(m_currentQuery->downloadedPosts());

		// Add KSZ to collection
		Collection collection;
		collection.save(&ksz, !m_currentQuery->kszSignature().isNull(), m_currentQuery->kszSignature());

		this->dequeue(m_currentQuery);

		// Start downloading the next query if any
		if (!m_queue.isEmpty()) {
			this->start();
		}

		return;
	}

	// Query paused, we won't download the next post right now
	if (m_currentQuery->isPaused()) {
		// Close file but don't rename as it's not completed yet
		m_kszWriter->close();

		delete m_kszWriter;
		m_kszWriter = nullptr;

		return;
	}

	// Download next post
	this->downloadPost();
}

///
/// Public Slot
///

/**
 * Called once a file has been downloaded.
 */
void Downloader::onFileDownloadFinished(QNetworkReply *data)
{
	if (data->size() > 0) {
		QString url = data->request().url().toString();
		int lastDotIndex = url.lastIndexOf('.');
		QString fileExtension = url.right(url.length() - lastDotIndex);

		m_kszWriter->addFile(QString::number(m_currentQuery->downloadedPosts()+1) + fileExtension, data->readAll());
	}

	this->afterFileDownload();
}

void Downloader::onFileDownloadProgress(qint64 recieved, qint64 total)
{
	emit postDownloadProgress(m_currentQuery->downloadedPosts()+1, recieved, total);
}
