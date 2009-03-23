#ifndef QTESTCORELIST_H
#define QTESTCORELIST_H

template <class T>
class QTestCoreList
{
    public:
        QTestCoreList();
        virtual ~QTestCoreList();

        void addToList(T **list);
        T *nextElement();
        T *previousElement();
        int count(T *list);
        int count();

    private:
        T *next;
        T *prev;
};

template <class T>
QTestCoreList<T>::QTestCoreList()
:next(0)
,prev(0)
{
}

template <class T>
QTestCoreList<T>::~QTestCoreList()
{
    if (prev) {
        prev->next = 0;
    }
    delete prev;

    if (next) {
        next->prev = 0;
    }
    delete next;
}

template <class T>
void QTestCoreList<T>::addToList(T **list)
{
    if (next)
        next->addToList(list);
    else {
        next = *list;
        if (next)
            next->prev = static_cast<T*>(this);
    }

    *list = static_cast<T*>(this);
}

template <class T>
T *QTestCoreList<T>::nextElement()
{
    return next;
}

template <class T>
T *QTestCoreList<T>::previousElement()
{
    return prev;
}

template <class T>
int QTestCoreList<T>::count()
{
    int numOfElements = 0;
    T *it = next;

    while(it){
        ++numOfElements;
        it = it->nextElement();
    }

    return numOfElements;
}

#endif
