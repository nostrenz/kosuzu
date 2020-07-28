#ifndef COLLECTION_H
#define COLLECTION_H

#include <src/ksz/ksz.h>
#include <QVector>
#include <QString>

class Collection
{
	public:
		Collection();
		~Collection();
		bool save(Ksz*, bool keepPage=false, QString previousSignature=NULL);
		Ksz* save(QString filePath, bool &isNew);
		void remove(Ksz*);
		bool has(QString) const;
		bool has(Ksz*) const;
		QVector<Ksz*> load();

	private:
		Ksz* read(QString iniPath);
		QString iniPath(QString iniName) const;
};

#endif // COLLECTION_H
