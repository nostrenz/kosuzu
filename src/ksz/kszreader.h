#ifndef KSZREADER_H
#define KSZREADER_H

#include "lib/QuaZIP/quazip/quazip.h"
#include "lib/QuaZIP/quazip/quazipfile.h"
#include <src/ksz/ksz.h>
#include <src/data/post.h>
#include <src/data/note.h>
#include <QImage>

///
/// The KszReader is used to read KSZ files.
///

class KszReader
{
	public:
		KszReader(QString filePath);
		~KszReader();
		static Ksz* read(QString filePath);
		static bool read(Ksz* ksz);
		Post* postAtPage(unsigned int page);
		void close();

	private:
		QString m_filePath;
		QuaZip* m_quaZip;
		QString getFileNameForPage(int page);
		QImage getImageByFileName(QString fileName);
		Post* readXmlByFilename(QString fileName);
};

#endif // KSZREADER_H
