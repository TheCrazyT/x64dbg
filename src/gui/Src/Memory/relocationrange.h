#ifndef RELOCATIONRANGE_H
#define RELOCATIONRANGE_H
#include <windows.h>
#include <QList>
#include <QObject>

class Range
{
public:
    Range(void* from, void* to);
    PVOID _from;
    PVOID _to;
    bool isWithin(void* addr);
};

typedef Range RelocationRange;


class  RelocationRanges: public QObject, public QList<RelocationRange*>
{
    Q_OBJECT

public:
    static void init();
    bool isWithin(void* addr);
    void append(RelocationRange* t);
    void prepend(RelocationRange* t);
    void insert(int i, RelocationRange* t);
    void replace(int i, RelocationRange* t);
    static RelocationRanges* instance()
    {
        if(RelocationRanges::_instance == NULL)
        {
            RelocationRanges::_instance = new RelocationRanges();
        }
        return RelocationRanges::_instance;
    }
public slots:
    void refresh();
private:
    long _lastupdate;
    PVOID _from;
    PVOID _to;
    RelocationRanges();
    static RelocationRanges* _instance;
};

#endif // RELOCATIONRANGE_H
