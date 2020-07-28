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
		static QString sha1(QString filePath);
		static QString bytes(QString filePath, unsigned int length=16);
};

#endif // KSZSIGNATURE_H
