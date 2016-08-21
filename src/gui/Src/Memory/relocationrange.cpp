//ToDo: RelocationRanges need to be initialized somewhere like at MemoryMapView::refreshMap

#include <QObject>
#include "Memory/RelocationRange.h"
#include "Bridge.h"
#include "MemoryPage.h"

#define min(X,Y) X<Y ? X:Y
#define max(X,Y) X>Y ? X:Y
RelocationRanges* RelocationRanges::_instance;
Range::Range(void* from, void* to)
{
    this->_from = from;
    this->_to = to;
}

bool Range::isWithin(void* addr)
{
    return (addr > _from) && (addr < _to);
}

bool RelocationRanges::isWithin(void* addr)
{
    if((this->_from == (void*) - 1) && (this->_to == 0))
    {
        return false;
    }
    if((addr > this->_from) && (addr < this->_to))
    {
        QList<RelocationRange*>::iterator b = this->begin();
        QList<RelocationRange*>::iterator e = this->end();
        QList<RelocationRange*>::iterator r = b;
        while(r != e)
        {
            if((*r)->isWithin(addr))
            {
                return true;
            }
            r++;
        }
    }
    return false;
}



void RelocationRanges::append(RelocationRange* t)
{
    this->_from = min(this->_from, t->_from);
    this->_to = max(this->_to, t->_to);
    QList<RelocationRange*>::append(t);
}


void RelocationRanges::prepend(RelocationRange* t)
{
    this->_from = min(this->_from, t->_from);
    this->_to = max(this->_to, t->_to);
    QList<RelocationRange*>::prepend(t);
}


void RelocationRanges::insert(int i, RelocationRange* t)
{
    this->_from = min(this->_from, t->_from);
    this->_to = max(this->_to, t->_to);
    QList<RelocationRange*>::insert(i, t);
}


void RelocationRanges::replace(int i, RelocationRange* t)
{
    this->_from = min(this->_from, t->_from);
    this->_to = max(this->_to, t->_to);
    QList<RelocationRange*>::replace(i, t);
}

RelocationRanges::RelocationRanges()
{
    this->_from = (void*) - 1;
    this->_to = 0;
    this->_lastupdate = 0;
    this->clear();
}
void RelocationRanges::init()
{
    Bridge* bridge = Bridge::getBridge();
    RelocationRanges* instance = RelocationRanges::instance();

    connect(bridge, SIGNAL(updateMemory()), instance, SLOT(refresh()));
}

void RelocationRanges::refresh()
{
    long t = GetCurrentTime();

    //TODO: find the right event instead of this sh*t.
    if(t - this->_lastupdate > 30 * 60 * 100)
    {
        this->_lastupdate = t;
    }
    else
    {
        return;
    }


    MEMMAP wMemMapStruct;
    int wI;

    memset(&wMemMapStruct, 0, sizeof(MEMMAP));

    DbgMemMap(&wMemMapStruct);
    if(wMemMapStruct.count > 0)
    {
        RelocationRanges tmp;
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
                    if((ft.from != 0) && (ft.to != 0))
                    {
                        tmp.append(new RelocationRange(ft.from, ft.to));
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        if(tmp.size() > 0)
        {
            this->_from = (void*) - 1;
            this->_to = 0;
            this->clear();
            for(RelocationRange* r : tmp)
            {
                RelocationRanges::append(r);
            }
        }
    }
    if(wMemMapStruct.page != 0)
        BridgeFree(wMemMapStruct.page);

}
