#include "stdio.h"
#include <src/collection.h>
#include <src/serializer.h>
#include <src/ksz/kszreader.h>
#include <src/ksz/kszsignature.h>
#include <src/main.h>
#include <src/utils.h>
#include <QDir>
#include <QFile>

///
/// The collection stores recently opened or downloaded KSZ files.
///

const QString COLLECTION_DIR = "collection";

Collection::Collection()
{
	QDir directory;
	QString dirPath = APP_DIR + '/' + COLLECTION_DIR;

	if (!directory.exists(dirPath)) {
		directory.mkdir(dirPath);
	}
}

Collection::~Collection()
{
}

///
/// Public method
///

bool Collection::save(Ksz* ksz, bool keepPage, QString previousSignature)
{
	QString kszSignature = KszSignature::bytes(ksz->path());

	if (kszSignature.isNull()) {
		return false;
	}

	QString iniPath = this->iniPath(previousSignature.isNull() ? kszSignature : previousSignature);
	bool fileExists = QFile::exists(iniPath);

	// There's already a file for this ksz, we'll keep the page number
	if (fileExists) {
		if (keepPage) {
			Ksz* existingKsz = this->read(iniPath);

			if (existingKsz != nullptr &&  existingKsz->page() <= ksz->pages()) {
				ksz->setPage(existingKsz->page());
			}
		}

		QFile::remove(iniPath);
	}

	QFile file(this->iniPath(kszSignature));

	if (!file.open(QIODevice::WriteOnly)) {
		return false;
	}

	QTextStream stream(&file);
	stream.seek(0);

	stream << "path=" << ksz->path() << "\n";
	stream << "title=" << ksz->title().trimmed() << "\n";
	stream << "pages=" << QString::number(ksz->pages()) << "\n";
	stream << "page=" << QString::number(ksz->page()) << "\n";

	file.close();

	return !fileExists;
}

Ksz* Collection::save(QString filePath, bool &isNew)
{
	Ksz* ksz = KszReader::read(filePath);

	if (ksz == nullptr) {
		return ksz;
	}

	isNew = this->save(ksz, true);

	return ksz;
}

void Collection::remove(Ksz* ksz)
{
	QString signature = KszSignature::bytes(ksz->path());

	if (signature.isNull()) {
		return;
	}

	QFile file(this->iniPath(signature));

	if (file.exists()) {
		file.remove();
	}
}

bool Collection::has(QString filePath) const
{
	QString signature = KszSignature::bytes(filePath);

	if (signature.isNull()) {
		return false;
	}

	return QFile::exists(this->iniPath(signature));
}

bool Collection::has(Ksz* ksz) const
{
	return this->has(ksz->path());
}

QVector<Ksz*> Collection::load()
{
	QDir dir(APP_DIR + '/' + COLLECTION_DIR);
	QStringList items = dir.entryList(QStringList() << "*.ini", QDir::Files);
	QVector<Ksz*> kszs;

	foreach (QString item, items) {
		Ksz* ksz = this->read(APP_DIR + '/' + COLLECTION_DIR + '/' + item);

		if (ksz == nullptr) {
			continue;
		}

		if (!QFile::exists(ksz->path())) {
			delete ksz;
			continue;
		}

		kszs.append(ksz);
	}

	return kszs;
}

///
/// Private method
///

Ksz* Collection::read(QString iniPath)
{
	QFile file(iniPath);

	if (!file.open(QIODevice::ReadOnly)) {
		return nullptr;
	}

	QTextStream stream(&file);
	QString line;
	Ksz* ksz = new Ksz();

	do {
		line = stream.readLine();
		int separatorIndex = line.indexOf('=');

		if (separatorIndex < 1) {
			continue;
		}

		QString key = line.mid(0, separatorIndex);
		QString value = line.mid(separatorIndex + 1).trimmed();

		if (key == "path") {
			ksz->setPath(value);
		} else if (key == "title") {
			ksz->setTitle(value);
		} else if (key == "pages") {
			ksz->setPages(value.toUInt());
		} else if (key == "page") {
			ksz->setPage(value.toUInt());
		}
	} while (!line.isNull());

	file.close();

	return ksz;
}

QString Collection::iniPath(QString signature) const
{
	return APP_DIR + '/' + COLLECTION_DIR + '/' + signature + ".ini";
}
