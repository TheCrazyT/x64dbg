//ToDo: RelocationRanges need to be initialized somewhere like at MemoryMapView::refreshMap

#include "Memory/RelocationRange.h"
#include "Bridge.h"
#include "MemoryPage.h"

#define min(X,Y) X<Y ? X:Y
#define max(X,Y) X>Y ? X:Y
RelocationRanges EmptyRelocationRanges;
RelocationRanges RelocationRanges::_instance;
Range::Range(void* from, void* to)
{
    this->_from = from;
    this->_to = to;
}

bool Range::isWithin(void* addr)
{
    return (addr > _from) && (addr > _to);
}

template<class T>
Ranges<T>::Ranges()
{
    this->_from = 0;
    this->_to = 0;
}



template<class T>
void Ranges<T>::append(const T & t)
{
    _from = min(this->_from, t._from);
    _to = max(this->_from, t._from);
    QList<T>::append(t);
}

template<class T>
void Ranges<T>::prepend(const T & t)
{
    _from = min(this->_from, t._from);
    _to = max(this->_from, t._from);
    QList<T>::prepend(t);
}

template<class T>
void Ranges<T>::insert(int i, const T & t)
{
    _from = min(this->_from, t._from);
    _to = max(this->_from, t._from);
    QList<T>::insert(i, t);
}

template<class T>
void Ranges<T>::replace(int i, const T & t)
{
    _from = min(this->_from, t._from);
    _to = max(this->_from, t._from);
    QList<T>::replace(i, t);
}

RelocationRanges::RelocationRanges()
{
    this->_from = 0;
    this->_to = 0;
    this->clear();

    connect(Bridge::getBridge(), SIGNAL(updateMemory()), this, SLOT(refresh()));
}

void RelocationRanges::refresh()
{
    this->_from = 0;
    this->_to = 0;
    this->clear();

    MEMMAP wMemMapStruct;
    int wI;

    memset(&wMemMapStruct, 0, sizeof(MEMMAP));

    DbgMemMap(&wMemMapStruct);
    for(wI = 0; wI < wMemMapStruct.count; wI++)
    {
        MEMPAGE page = wMemMapStruct.page[wI];
        MEMORY_BASIC_INFORMATION mbi = page.mbi;
        if(mbi.Type == MEM_IMAGE)
        {
            PVOID ba = mbi.BaseAddress;
            RELOCTBL relocTbl;
            DbgRelocTbl((duint)ba, &relocTbl);
            for(int i = 0; i < MAX_RELOC_ENTRIES; i++)
            {
                FROMTO ft = relocTbl.fromTo[i];
                append(RelocationRange(ft.from, ft.to));
            }
        }
    }
    if(wMemMapStruct.page != 0)
        BridgeFree(wMemMapStruct.page);

}
