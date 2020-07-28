#ifndef UTILS_H
#define UTILS_H

#include <src/ksz/ksz.h>
#include <src/data/post.h>
#include <QString>
#include <QDir>
#include <QIcon>

///
/// Holds various utility functions.
///

class utils
{
	public:
		static QString regex(QString text, QString pattern, int index);
		static QString fileContent(QString path);
		static QString basename(QString filePath);
		static QString dirname(QString filePath);
		static QDir downloadDir(bool create=false);
		static void openDownloadDir();
		static void openUrl(Ksz* ksz);
		static void openUrl(const unsigned int poolId, const unsigned int postId);
		static void fileSystemSafeFileName(QString &fileName);
		static QIcon icon(QString);
		static void openContainingFolder(QString filePath);
};

#endif // UTILS_H
