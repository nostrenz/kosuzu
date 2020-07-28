#ifndef COLLECTIONITEM_H
#define COLLECTIONITEM_H

#include <src/ksz/ksz.h>
#include <QListWidgetItem>

class CollectionItem : public QListWidgetItem
{
	public:
		CollectionItem();
		Ksz* ksz() const;
		void setKsz(Ksz*);

	private:
		Ksz* m_ksz;
};

#endif // COLLECTIONITEM_H
