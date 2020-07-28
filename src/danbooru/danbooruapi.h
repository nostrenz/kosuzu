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

class DanbooruApi : public QObject
{
	Q_OBJECT

	public:
		DanbooruApi(bool test=false);
		QJsonObject getPoolJson(int poolId);
		QJsonObject getPostJson(int postId);
		QJsonArray getNotesJson(int postId);

	private:
		bool m_test = false;
		QString fetch(QString url);
		QString route(QString route);
};

#endif // DANBOORUAPI_H
