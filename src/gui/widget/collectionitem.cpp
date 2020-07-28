#include <src/gui/widget/collectionitem.h>

CollectionItem::CollectionItem()
{
}

///
/// Public method
///

Ksz* CollectionItem::ksz() const
{
	return m_ksz;
}

void CollectionItem::setKsz(Ksz* ksz)
{
	m_ksz = ksz;
}
