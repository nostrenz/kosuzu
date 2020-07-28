#ifndef DOWNLOADSERIALIZER_H
#define DOWNLOADSERIALIZER_H

#include <src/download/downloadquery.h>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QJsonObject>

class Serializer
{
	public:
		Serializer();

		// Serialize
		QString serialize(QVector<DownloadQuery*>);

		// Unserialize
		QVector<DownloadQuery*> downloadQueries(QByteArray);

	private:
		QJsonObject danbooruUrlToJsonObject(DanbooruUrl*);
		QJsonObject downloadQueryToJsonObject(DownloadQuery*);
};

#endif // DOWNLOADSERIALIZER_H
