#include <src/serializer.h>
#include <QJsonArray>
#include <QJsonDocument>

///
/// This class is used to convert some objets to text format (usually JSON),
/// in order to be stored to and restored from disk.
///

Serializer::Serializer()
{
}

///
/// Public method
///

///
/// Serialize
///

QString Serializer::serialize(QVector<DownloadQuery*> queries)
{
	QJsonArray jsonArray;

	foreach (DownloadQuery* query, queries) {
		QJsonObject jsonObject = this->downloadQueryToJsonObject(query);

		jsonArray.append(jsonObject);
	}

	QJsonDocument doc(jsonArray);

	return doc.toJson();
}

///
/// Unserialize
///

QVector<DownloadQuery*> Serializer::downloadQueries(QByteArray data)
{
	QVector<DownloadQuery*> queries;

	QJsonDocument document = QJsonDocument::fromJson(data);
	QJsonArray array = document.array();

	foreach (const QJsonValue &v, array) {
		QJsonObject jsonObject = v.toObject();

		QString title = jsonObject.value("title").toString();
		QString downloadedPosts = jsonObject.value("downloadedPosts").toString();
		QString nextPostId = jsonObject.value("nextPostId").toString();
		QString postCount = jsonObject.value("postCount").toString();
		QString kszPath = jsonObject.value("kszPath").toString();

		int downloadedPostsInt = downloadedPosts.toInt();
		int postCountInt = postCount.toInt();

		QJsonObject urlJsonObject = jsonObject.value("url").toObject();

		QString poolId = urlJsonObject.value("poolId").toString();
		QString postId = urlJsonObject.value("postId").toString();

		DanbooruUrl* danbooruUrl = new DanbooruUrl(poolId.toUInt(), postId.toUInt());
		DownloadQuery* query = new DownloadQuery(danbooruUrl);

		query->setTitle(jsonObject["title"].toString());
		query->setDownloadedPosts(downloadedPostsInt);
		query->setPostCount(postCountInt);

		queries.append(query);
	}

	return queries;
}

///
/// Private method
///

QJsonObject Serializer::danbooruUrlToJsonObject(DanbooruUrl* url)
{
	QJsonObject jsonObject;

	jsonObject["poolId"] = QString::number(url->poolId());
	jsonObject["postId"] = QString::number(url->postId());

	return jsonObject;
}

QJsonObject Serializer::downloadQueryToJsonObject(DownloadQuery* query)
{
	QJsonObject jsonObject;

	jsonObject["url"] = this->danbooruUrlToJsonObject(query->url());
	jsonObject["title"] = query->title();
	jsonObject["kszPath"] = query->kszPath();
	jsonObject["downloadedPosts"] = QString::number(query->downloadedPosts());
	jsonObject["postCount"] = QString::number(query->postCount());

	return jsonObject;
}
