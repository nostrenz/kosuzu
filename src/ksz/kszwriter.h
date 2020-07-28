#ifndef KSZWRITER_H
#define KSZWRITER_H

#include <lib/QuaZIP/quazip/quazip.h>
#include <lib/QuaZIP/quazip/quazipfile.h>
#include <src/data/note.h>
#include <QImage>

///
/// The KszWriter is used to write KSZ files.
///

class KszWriter
{
	public:
		KszWriter(QString filePath);
		~KszWriter();
		void addMetaXml(const QString title, const unsigned int poolId=0, const unsigned int postId=0);
		void addFile(QString fileName, QByteArray data);
		void addNotes(int postId, QVector<Note> notes, int pageNumber);
		bool isOpen() const;
		QString close(bool rename=false);
		QString filePath() const;

	private:
		QString m_filePath;
		QuaZip* m_quaZip;
		QString findAvailableFileName(QString newFileName, unsigned int count=1) const;
};

#endif // KSZWRITER_H
