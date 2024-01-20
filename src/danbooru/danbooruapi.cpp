#include "danbooruapi.h"

// Usage Agent header used for requests made against Danbooru.
// Not including that header can trigger the Cloudflare check, preventing to access the requested resource.
extern const QString USER_AGENT = "Kosuzu/1.0";

const QString BASE_URL = "https://danbooru.donmai.us";
const QString TEST_URL = "https://testbooru.donmai.us";

DanbooruApi::DanbooruApi(bool test)
{
	m_test = test;
}

///
/// Public method
///

JsonObjectResponse DanbooruApi::getPoolJson(int poolId) const
{
	FetchResponse response = this->fetch("/pools/" + QString::number(poolId) + ".json");
	QJsonDocument document = QJsonDocument::fromJson(response.content.toUtf8());

	JsonObjectResponse jsonObjectResponse;
	jsonObjectResponse.jsonObject = document.object();
	jsonObjectResponse.networkError = response.networkError;

	return jsonObjectResponse;
}

JsonObjectResponse DanbooruApi::getPostJson(int postId) const
{
	FetchResponse response = this->fetch("/posts/" + QString::number(postId) + ".json");
	QJsonDocument document = QJsonDocument::fromJson(response.content.toUtf8());

	JsonObjectResponse jsonObjectResponse;
	jsonObjectResponse.jsonObject = document.object();
	jsonObjectResponse.networkError = response.networkError;

	return jsonObjectResponse;
}

JsonArrayResponse DanbooruApi::getNotesJson(int postId) const
{
	FetchResponse response = this->fetch("/notes.json?limit=1000&group_by=note&search[post_id]=" + QString::number(postId));
	QJsonDocument document = QJsonDocument::fromJson(response.content.toUtf8());

	JsonArrayResponse jsonArrayResponse;
	jsonArrayResponse.jsonArray = document.array();
	jsonArrayResponse.networkError = response.networkError;

	return jsonArrayResponse;
}

///
/// Private method
///

FetchResponse DanbooruApi::fetch(QString route) const
{
	QNetworkRequest request(QUrl(this->route(route)));
	request.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);

	QNetworkAccessManager manager;
	QNetworkReply *response = manager.get(request);
	QEventLoop event;

	connect(response, SIGNAL(finished()), &event, SLOT(quit()));
	event.exec();

	FetchResponse fetchResponse;
	fetchResponse.content = response->readAll();
	fetchResponse.networkError = response->error();

	return fetchResponse;
}

QString DanbooruApi::route(QString route) const
{
	return (m_test ? TEST_URL : BASE_URL) + route;
}
