#include <src/ksz/kszsignature.h>
#include <QCryptographicHash>
#include <QFile>

#define MIN_KSZ_SIZE 22

KszSignature::KszSignature()
{

}

///
/// Public method
///

/**
 * Produce a file signature using an MD5 hash.
 * Slow for big files (~100MB+)
 */
QString KszSignature::sha1(QString filePath)
{
	QFile file(filePath);
	QCryptographicHash hash(QCryptographicHash::Sha1);

	if (!file.open(QIODevice::ReadOnly)) {
		return QString();
	}

	hash.addData(file.readAll());
	file.close();

	return hash.result().toHex();
}

/**
 * Produce a file signature by reading some bytes from the file.
 * Fast even for big files.
 */
QString KszSignature::bytes(QString filePath, unsigned int length)
{
	FILE* file = fopen(filePath.toUtf8().constData(), "r+b");
	QString result;

	if (file == NULL) {
		return result;
	}

	// Seek to end of file
	fseek(file, 0, SEEK_END);

	unsigned int fileSize = ftell(file);

	// Empty zip archive
	if (fileSize <= MIN_KSZ_SIZE*2) {
		fclose(file);

		return result;
	}

	length += MIN_KSZ_SIZE;

	if (fileSize < length) {
		length = fileSize;
	}

	// Seek to start of file
	fseek(file, MIN_KSZ_SIZE, SEEK_SET);

	for (unsigned int i = 0; i < length; i++) {
		int c = fgetc(file);

		if (c != -1) {
			result += QString().asprintf("%X", c);
		}
	}

	// Seek to end of file
	fseek(file, -length, SEEK_SET);

	for (unsigned int i = 0; i < length; i++) {
		int c = fgetc(file);

		if (c != -1) {
			result += QString().asprintf("%X", c);
		}
	}

	fclose(file);

	return result;
}
