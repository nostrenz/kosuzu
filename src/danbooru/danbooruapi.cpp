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
	QUrl url = QUrl(this->route(route));

	// Print the OpenSSL version Qt was built with. For Qt 5.14.2, this should display "OpenSSL 1.1.1d".
	//qDebug() << QSslSocket::sslLibraryBuildVersionString();
	//
	// Print the OpenSSL version the program currently have access to.
	// If it prints nothing or a version number too different from the one printed above, we'll need
	// to download the binaries for the right version (1.1.1) and place them next to the executable.
	// OpenSSL binaries for Windows can be obtained from here: https://wiki.overbyte.eu/wiki/index.php/ICS_Download.
	//qDebug() << QSslSocket::sslLibraryVersionString();
	//
	// If everything's good, this should display "true".
	//qDebug() << QSslSocket::supportsSsl();

	QNetworkRequest request(url);
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
