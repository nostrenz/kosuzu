#include <src/ksz/kszsignature.h>
#include <QCryptographicHash>
#include <QFile>

#define ZIP_START 22
#define FIRST_BYTES 3

KszSignature::KszSignature()
{
}

///
/// Public method
///

/**
 * Produce a file signature by reading some bytes from the file.
 */
QString KszSignature::read(QString filePath, unsigned int length)
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
	if (fileSize <= ZIP_START) {
		fclose(file);

		return result;
	}

	if (fileSize < ZIP_START + FIRST_BYTES + length) {
		length = fileSize - ZIP_START - FIRST_BYTES;
	}

	// Read the first bytes
	fseek(file, ZIP_START, SEEK_SET);
	KszSignature::appendHex(file, FIRST_BYTES, result);

	// Read some bytes at end of file
	fseek(file, -length, SEEK_SET);
	KszSignature::appendHex(file, length, result);

	fclose(file);

	return result;
}

///
/// Private method
///

void KszSignature::appendHex(FILE* file, unsigned int length, QString &str)
{
	for (unsigned int i = 0; i < length; i++) {
		int c = fgetc(file);

		if (c != -1) {
			str += QString().asprintf("%X", c);
		}
	}
}
