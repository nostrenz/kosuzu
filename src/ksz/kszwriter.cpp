#include <src/ksz/kszwriter.h>
#include <QXmlStreamWriter>

const QString TEMP_EXT = ".tmp";

KszWriter::KszWriter(QString filePath)
{
	bool fileExists = QFile::exists(filePath);

	// New file
	if (!fileExists) {
		filePath += TEMP_EXT;
	}

	m_quaZip = new QuaZip(filePath);
	m_quaZip->open(fileExists ? QuaZip::mdAdd : QuaZip::mdCreate);
}

KszWriter::~KszWriter()
{
	delete m_quaZip;
}

///
/// Public method
///

void KszWriter::addMetaXml(const QString title, const unsigned int poolId, const unsigned int postId)
{
	// Create content for pool.xml
	QString xmlContent;
	QXmlStreamWriter xml(&xmlContent);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	xml.writeStartElement("meta");

	if (poolId > 0) {
		xml.writeAttribute("pool", QString::number(poolId));
	}

	if (postId > 0) {
		xml.writeAttribute("post", QString::number(postId));
	}

	xml.writeTextElement("title", title);

	xml.writeEndElement(); // /meta
	xml.writeEndDocument();

	this->addFile("ksz.xml", xmlContent.toUtf8());
}

void KszWriter::addFile(QString fileName, QByteArray data)
{
	QuaZipFile imageZipFile(m_quaZip);
	imageZipFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName));
	imageZipFile.write(data);
	imageZipFile.close();
}

void KszWriter::addNotes(int postId, QVector<Note> notes, int pageNumber)
{
	if (notes.size() < 1) {
		return;
	}

	// Write XML
	QString postXmlContent;
	QXmlStreamWriter xml(&postXmlContent);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();

	xml.writeStartElement("notes");
	xml.writeAttribute("post", QString::number(postId));

	foreach (Note note, notes) {
		xml.writeStartElement("note");
		xml.writeAttribute("x", QString::number(note.x()));
		xml.writeAttribute("y", QString::number(note.y()));
		xml.writeAttribute("width", QString::number(note.width()));
		xml.writeAttribute("height", QString::number(note.height()));
		xml.writeCharacters(note.body());
		xml.writeEndElement(); // /note
	}

	xml.writeEndElement(); // /notes

	this->addFile(QString::number(pageNumber) + ".xml", postXmlContent.toUtf8());
}

QString KszWriter::close(bool rename)
{
	QString zipName = m_quaZip->getZipName();

	if (!m_quaZip->isOpen()) {
		return zipName;
	}

	m_quaZip->close();

	// We don't want to rename the file
	if (!rename) {
		return zipName;
	}

	if (!zipName.endsWith(TEMP_EXT)) {
		return zipName;
	}

	// Rename .ksz.tmp to .ksz
	QString newName = this->findAvailableFileName(QString(zipName).remove(TEMP_EXT));
	QFile file(zipName);

	file.rename(newName);

	if (file.isOpen()) {
		file.close();
	}

	return newName;
}

bool KszWriter::isOpen() const
{
	return m_quaZip->isOpen();
}

QString KszWriter::filePath() const
{
	return m_quaZip->getZipName();
}

///
/// Private method
///

QString KszWriter::findAvailableFileName(QString newFileName, unsigned int count) const
{
	// This name is available, nothing more to do
	if (!QFile::exists(newFileName)) {
		return newFileName;
	}

	int lastDotIndex = newFileName.lastIndexOf('.');
	QString fileExtension = newFileName.right(newFileName.length() - lastDotIndex);
	QString newFileNameWithoutExtension = newFileName.left(lastDotIndex);

	// Append (1) at the end of the file
	newFileNameWithoutExtension += " (" + QString::number(count) + ")" + fileExtension;

	if (!QFile::exists(newFileNameWithoutExtension)) {
		return newFileNameWithoutExtension;
	}

	return this->findAvailableFileName(newFileName, count+1);
}
