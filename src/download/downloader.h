#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <src/ksz/ksz.h>
#include <src/ksz/kszwriter.h>
#include <src/danbooru/danbooruurl.h>
#include <src/danbooru/danbooruapi.h>
#include <src/download/downloadquery.h>
#include <QObject>
#include <QVector>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

///
/// The Downloader is responsible for parsing Danbooru pages and downloading images.
/// As it downloads it write files into a zip archive using the KszWriter.
///

class Downloader : public QObject
{
	Q_OBJECT

	public:
		explicit Downloader();
		~Downloader();
		bool add(DanbooruUrl*);
		bool add(DownloadQuery*);
		bool addReady(DownloadQuery* query);
		void remove(DownloadQuery*);
		bool start();
		void pause();
		void stop();
		bool start(DownloadQuery*);
		void pause(DownloadQuery*);
		void stop(DownloadQuery*);
		void clear();
		DownloadQuery* currentQuery() const;
		bool hasQuery(DownloadQuery*) const;
		bool hasQueryWithUrl(DanbooruUrl*) const;
		bool isRunning() const;
		bool hasIncompleteDownloads(const bool skipNotStarted) const;
		bool isQueued(DownloadQuery*) const;
		DownloadQuery* queryAt(int index) const;
		int queueLength() const;
		int indexOfQuery(DownloadQuery*) const;
		int placeInQueue(DownloadQuery*) const;
		int queryCount() const;

	private:
		QVector<DownloadQuery*> m_queries;
		QVector<DownloadQuery*> m_queue;
		DownloadQuery* m_currentQuery = nullptr;
		QNetworkAccessManager m_manager;
		KszWriter* m_kszWriter = nullptr;
		DanbooruApi* m_danbooruApi = nullptr;
		bool downloadQuery(DownloadQuery*);
		bool downloadFile(QString fileUrl);
		QString pageName(unsigned int index);
		bool downloadPost();
		void queue(DownloadQuery*);
		void dequeue(DownloadQuery*);
		void updateQueryFromApi(DownloadQuery*);
		void afterFileDownload();

	public slots:
		void onFileDownloadFinished(QNetworkReply*);
		void onFileDownloadProgress(qint64, qint64);

	signals:
		void downloadAdded(DownloadQuery*);
		void postDownloadProgress(unsigned int, qint64, qint64);
		void postDownloaded(DownloadQuery*);
		void statusMessage(QString);
};

#endif // DOWNLOADER_H
