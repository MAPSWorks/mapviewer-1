#ifndef MUTEXLOCKER_H
#define MUTEXLOCKER_H

#include <QMutex>

class MutexLocker
{
public:
    MutexLocker(QMutex* mutex):
        m_mutex(mutex)
    {
        mutex->lock();
    }

    ~MutexLocker() {
        m_mutex->unlock();
    }
private:
    QMutex* m_mutex;
};

#endif // MUTEXLOCKER_H
