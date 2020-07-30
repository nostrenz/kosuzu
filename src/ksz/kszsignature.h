#ifndef KSZSIGNATURE_H
#define KSZSIGNATURE_H

#include <QString>

///
/// This class is used to create a signature of a KSZ file.
/// It's pretty simple, hopefully the chance of two files with the same signature should be low enough.
///

class KszSignature
{
	public:
		KszSignature();
		static QString read(QString filePath, unsigned int length=16);

	private:
		static void appendHex(FILE* file, unsigned int length, QString &str);
};

#endif // KSZSIGNATURE_H
