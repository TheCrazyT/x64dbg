#ifndef RELOCATIONRANGE_H
#define RELOCATIONRANGE_H
#include <QList>
#include <QObject>
#include <windows.h>

class Range
{
public:
    Range(void* from, void* to);
    PVOID _from;
    PVOID _to;
    bool isWithin(void* addr);
};

typedef Range RelocationRange;
typedef Range ModuleRange;

template <typename T>
class Ranges: public QList<T>, public QObject
{

public:
    Ranges();
    bool isWithin(void* addr)
    {
        if((this->_from == 0) && (this->_to == 0))
        {
            return false;
        }
        if((addr > _from) && (addr > _to))
        {
            for(Range r : *this)
            {
                if(r.isWithin(addr))
                {
                    return true;
                }
            }
        }
        return false;
    };

    void append(const T & t);
    void prepend(const T & t);
    void insert(int i, const T & t);
    void replace(int i, const T & t);

protected:
    void* _from;
    void* _to;
};
class  RelocationRanges: public Ranges<RelocationRange>
{
    Q_OBJECT

public:
    RelocationRanges();
    static RelocationRanges & instance()
    {
        return RelocationRanges::_instance;
    }
    static RelocationRanges _instance;
public slots:
    void refresh();
};

extern RelocationRanges EmptyRelocationRanges;
#endif // RELOCATIONRANGE_H
