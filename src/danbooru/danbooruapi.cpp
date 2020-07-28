#include "danbooruapi.h"

const QString BASE_URL = "https://danbooru.donmai.us";
const QString TEST_URL = "https://testbooru.donmai.us";

DanbooruApi::DanbooruApi(bool test)
{
	m_test = test;
}

///
/// Public method
///

QJsonObject DanbooruApi::getPoolJson(int poolId)
{
	QString json = this->fetch("/pools/" + QString::number(poolId) + ".json");
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());

	return document.object();
}

QJsonObject DanbooruApi::getPostJson(int postId)
{
	QString json = this->fetch("/posts/" + QString::number(postId) + ".json");
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());

	return document.object();
}

QJsonArray DanbooruApi::getNotesJson(int postId)
{
	QString json = this->fetch("/notes.json?limit=1000&group_by=note&search[post_id]=" + QString::number(postId));
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());

	return document.array();
}

///
/// Private method
///

QString DanbooruApi::fetch(QString route)
{
	QNetworkAccessManager manager;
	QNetworkReply *response = manager.get(QNetworkRequest(QUrl(this->route(route))));
	QEventLoop event;

	connect(response, SIGNAL(finished()), &event, SLOT(quit()));
	event.exec();

	return response->readAll();
}

QString DanbooruApi::route(QString route)
{
	return (m_test ? TEST_URL : BASE_URL) + route;
}
