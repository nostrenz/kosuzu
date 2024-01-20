#ifndef DOWNLOADQUERY_H
#define DOWNLOADQUERY_H

#include <src/danbooru/danbooruurl.h>
#include <src/data/note.h>
#include <src/ksz/ksz.h>
#include <QVector>
#include <QNetworkReply>

///
/// This class represents a Danbooru download.
/// If is used by the Downloader.
///

class DownloadQuery
{
	public:
		DownloadQuery(DanbooruUrl* url);
		DownloadQuery(Ksz* ksz, unsigned int postId);
		~DownloadQuery();
		DanbooruUrl* url() const;
		QString title() const;
		QString kszPath() const;
		unsigned int downloadedPosts() const;
		unsigned int postCount() const;
		QString kszSignature() const;
		void setTitle(QString title);
		void setPostCount(unsigned int postCount);
		void setDownloadedPosts(unsigned int downloadedPosts);
		void setKszPath(QString filePath);
		bool isValid();
		bool isNotStarted() const;
		bool isRunning();
		bool isPaused() const;
		bool isCompleted() const;
		void setAsRunning();
		void setAsPaused();
		void postDownloaded();
		void addPostId(int postId);
		int postIdOfPage(int page) const;
		bool hasPostIds() const;
		void reset();
		QNetworkReply::NetworkError m_networkError = QNetworkReply::NetworkError::NoError;

	private:
		DanbooruUrl* m_url;
		QString m_title;
		QString m_kszPath;
		bool m_running = false;
		unsigned int m_downloadedPosts = 0;
		unsigned int m_postCount = 0;
		QVector<int> m_postIds;
		QString m_kszSignature;
};

#endif // DOWNLOADQUERY_H
