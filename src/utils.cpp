#include <src/main.h>
#include <src/utils.h>
#include <src/danbooru/danbooruurl.h>
#include <src/ksz/kszreader.h>
#include <QRegExp>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>

///
/// Constant
///

const QString DOWNLOAD_DIR = "downloads";

QString utils::regex(QString text, QString pattern, int index)
{
	QRegExp rx(pattern);
	rx.indexIn(text);

	QStringList list = rx.capturedTexts();

	if (list.length() < index+1) {
		return NULL;
	}

	return list[index];
}

QString utils::fileContent(QString path)
{
	QFile f(path);

	if (!f.open(QFile::ReadOnly | QFile::Text)) {
		 return "";
	}

	QTextStream in(&f);

	return in.readAll();
}

QString utils::basename(QString filePath)
{
	int lastSlashIndex = filePath.lastIndexOf('/');
	int lastDotIndex = filePath.lastIndexOf('.');

	if (lastSlashIndex < 1) {
		lastSlashIndex = filePath.lastIndexOf('\\');
	}

	return filePath.mid(lastSlashIndex+1, lastDotIndex);
}

QString utils::dirname(QString filePath)
{
	int lastSlashIndex = filePath.lastIndexOf('/');

	if (lastSlashIndex < 1) {
		lastSlashIndex = filePath.lastIndexOf('\\');
	}

	return filePath.mid(0, lastSlashIndex);
}

QDir utils::downloadDir(bool create)
{
	QDir directory;
	QString dirPath = APP_DIR + '/' + DOWNLOAD_DIR;

	if (create && !directory.exists(dirPath)) {
		directory.mkdir(dirPath);
	}

	directory.cd(dirPath);

	return directory;
}

void utils::openDownloadDir()
{
	QString storePath = utils::downloadDir().absolutePath();

	QDesktopServices::openUrl(storePath);
}

void utils::openUrl(Ksz* ksz)
{
	if (ksz->poolId() == 0 && ksz->postId() == 0) {
		bool success = KszReader::read(ksz);

		if (!success) {
			return;
		}
	}

	DanbooruUrl url(ksz->poolId(), ksz->postId());

	utils::openUrl(ksz->poolId(), ksz->postId());
}

void utils::openUrl(const unsigned int poolId, const unsigned int postId)
{
	DanbooruUrl url(poolId, postId);

	if (url.isValid()) {
		QDesktopServices::openUrl(url.url());
	}
}

void utils::fileSystemSafeFileName(QString &fileName)
{
	fileName.replace('/', "");
	fileName.replace('\\', "");
	fileName.replace('*', "");
	fileName.replace('?', "");
	fileName.replace('<', "");
	fileName.replace('>', "");
	fileName.replace('|', "");
	fileName.replace(':', "");
	fileName.replace('"', "");
}

QIcon utils::icon(QString name)
{
	return QIcon(APP_DIR + "/assets/icons/" + name + ".png");
}

void utils::openContainingFolder(QString filePath)
{
	QDesktopServices::openUrl(utils::dirname(filePath));
}
