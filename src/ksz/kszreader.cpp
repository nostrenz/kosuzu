#include <src/ksz/kszreader.h>
#include <src/utils.h>
#include <QXmlStreamReader>
#include <QTextStream>
#include <QCollator>
#include <QFile>

KszReader::KszReader(QString filePath)
{
	m_quaZip = new QuaZip(filePath);
	m_quaZip->open(QuaZip::mdUnzip);
}

KszReader::~KszReader()
{
	delete m_quaZip;
}

///
/// Public method
///

/**
 * Read a complete KSZ file.
 */
Ksz* KszReader::read(QString filePath)
{
	if (!QFile::exists(filePath)) {
		return nullptr;
	}

	Ksz* ksz = new Ksz();
	ksz->setPath(filePath);

	bool success = KszReader::read(ksz);

	if (!success) {
		delete ksz;

		return nullptr;
	}

	return ksz;
}

bool KszReader::read(Ksz* ksz)
{
	QuaZip quaZip(ksz->path());

	if (!quaZip.open(QuaZip::mdUnzip)) {
		return false;
	}

	// Count posts
	QStringList fileNames = quaZip.getFileNameList();

	if (fileNames.size() < 1) {
		quaZip.close();

		return false;
	}

	unsigned int pages = 0;

	foreach (QString fileName, fileNames) {
		if (fileName.endsWith(".jpg") || fileName.endsWith(".png")) {
			pages++;
		}
	}

	if (pages < 1) {
		quaZip.close();

		return false;
	}

	ksz->setPages(pages);

	// Missing ksz.xml
	if (!quaZip.setCurrentFile("ksz.xml")) {
		quaZip.close();

		return true;
	}

	QuaZipFile inFile(&quaZip);

	if (!inFile.open(QIODevice::ReadOnly)) {
		quaZip.close();

		return true;
	}

	QTextStream ts(&inFile);
	QString metaXmlContent = ts.readAll();

	inFile.close();
	quaZip.close();

	QXmlStreamReader xml(metaXmlContent);

	while(!xml.atEnd()) {
		if (xml.name() == "meta") {
			if (xml.attributes().hasAttribute("pool")) {
				ksz->setPoolId(xml.attributes().value("pool").toUInt());
			}

			if (xml.attributes().hasAttribute("post")) {
				ksz->setPostId(xml.attributes().value("post").toUInt());
			}
		} else if (xml.name() == "title") {
			xml.readNext();

			QString title = QString::fromUtf8(xml.text().toUtf8());

			if (!title.isNull()) {
				title = title.trimmed();

				if (!title.isEmpty()) {
					ksz->setTitle(title);
				}
			}
		}

		xml.readNext();
	}

	return true;
}

Post* KszReader::postAtPage(unsigned int page)
{
	QString fileName = this->getFileNameForPage(page);

	Post* post = this->readXmlByFilename(fileName);

	QImage image = this->getImageByFileName(fileName);
	post->setPixmap(QPixmap::fromImage(image));

	return post;
}

void KszReader::close()
{
	if (m_quaZip->isOpen()) {
		m_quaZip->close();
	}
}

///
/// Private method
///

QString KszReader::getFileNameForPage(int page)
{
	QStringList files = m_quaZip->getFileNameList();

	if (page > files.length()) {
		return QString();
	}

	QStringList images;

	foreach (QString file, files) {
		if (file.endsWith(".jpg") || file.endsWith(".png")) {
			images.append(file);
		}
	}

	if (page > images.length()) {
		return QString();
	}

	QCollator collator;
	collator.setNumericMode(true);

	std::sort(
		images.begin(),
		images.end(),
		[&collator](const QString &file1, const QString &file2)
		{
			return collator.compare(file1, file2) < 0;
		}
	);

	return images.at(page - 1);
}

Post* KszReader::readXmlByFilename(QString fileName)
{
	Post* post = new Post();

	int lastDotIndex = fileName.lastIndexOf('.');
	fileName = fileName.left(lastDotIndex);

	if (!m_quaZip->setCurrentFile(fileName + ".xml")) {
		return post;
	}

	QuaZipFile inFile(m_quaZip);

	if (!inFile.open(QIODevice::ReadOnly)) {
		return post;
	}

	QTextStream ts(&inFile);
	QString xmlContent = ts.readAll();;

	inFile.close();

	QXmlStreamReader xml(xmlContent);

	while(!xml.atEnd()) {
		if (xml.name() == "notes") {
			if (xml.attributes().hasAttribute("post")) {
				post->setId(xml.attributes().value("post").toUInt());
			}
		} else if (xml.name() == "note") {
			QString body;
			unsigned int x = xml.attributes().value("x").toUInt();
			unsigned int y = xml.attributes().value("y").toUInt();
			unsigned int width = xml.attributes().value("width").toUInt();
			unsigned int height = xml.attributes().value("height").toUInt();

			// Go to inner text
			xml.readNext();

			body = xml.text().toUtf8();

			// Go to </note>
			xml.readNext();

			if (xml.isEndElement() && xml.name() == "note") {
				post->addNote(new Note(body, x, y, width, height));
			}
		}

		xml.readNext();
	}

	return post;
}

QImage KszReader::getImageByFileName(QString fileName)
{
	QImage image;

	if (!m_quaZip->setCurrentFile(fileName)) {
		return image;
	}

	QuaZipFile inFile(m_quaZip);

	if (!inFile.open(QIODevice::ReadOnly)) {
		return image;
	}

	QByteArray bytes = inFile.readAll();

	if (bytes.size() == 0) {
		return image;
	}

	if (fileName.endsWith(".png")) {
		image.loadFromData(bytes, "PNG");
	} else {
		image.loadFromData(bytes, "JPG");
	}

	inFile.close();

	return image;
}
