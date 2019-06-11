#ifndef SINGLETON_H
#define SINGLETON_H

#include <QSharedPointer>

template <typename T> class Singleton
{
public:
    static T* getInstance()
    {
        if(m_instance.data() == nullptr)
            m_instance.reset(new T, deleter);
        return m_instance.data();
    }

    static void clear()
    {
        m_instance.clear();
    }

private:
    Q_DISABLE_COPY(Singleton)

protected:
    Singleton() {}
    virtual ~Singleton() {}
    static QSharedPointer<T> m_instance;
    static void deleter(T *obj)
    {
        if( obj )
        {
            obj->disconnect();
            obj->deleteLater();
            obj = NULL;
        }
    }
};

template <typename T> QSharedPointer<T>  Singleton<T>::m_instance;

#endif // SINGLETON_H
