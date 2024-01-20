#ifndef DANBOORUAPI_H
#define DANBOORUAPI_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QEventLoop>

QT_BEGIN_NAMESPACE
namespace Danbooru { class DanbooruApi; }
QT_END_NAMESPACE

struct FetchResponse
{
	QString content;
	QNetworkReply::NetworkError networkError;
};

struct JsonObjectResponse
{
	QJsonObject jsonObject;
	QNetworkReply::NetworkError networkError;
};

struct JsonArrayResponse
{
	QJsonArray jsonArray;
	QNetworkReply::NetworkError networkError;
};

class DanbooruApi : public QObject
{
	Q_OBJECT

	public:
		DanbooruApi(bool test=false);
		JsonObjectResponse getPoolJson(int poolId) const;
		JsonObjectResponse getPostJson(int postId) const;
		JsonArrayResponse getNotesJson(int postId) const;

	private:
		bool m_test = false;
		FetchResponse fetch(QString url) const;
		QString route(QString route) const;
};

#endif // DANBOORUAPI_H
