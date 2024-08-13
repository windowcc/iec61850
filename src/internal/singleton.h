#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <mutex>

template <typename T>
class Singleton
{
public:
/**
     * @brief: 获取单例实例
     * @param {} NULL
     * @return: 单例实例对象
     * @remark: This function has no i remarks.
     */
    static T *Instance()
    {
        if (!instance)
        {
            std::lock_guard<std::mutex> lck(mtx);
            if (!instance)
            {
                try
                {
                    instance = new T();
                }
                catch (const std::exception &ex)
                {
                    Destroy();
                    throw std::exception(ex);
                }
            }
        }
        return instance;
    }

    static void Destroy()
    {
        if (instance)
        {
            delete instance;
            instance = nullptr;
        }
    }

protected:
    Singleton(void)                         = default;
    virtual ~Singleton(void)                = default;

private:
    Singleton(const Singleton&)             = delete;
    Singleton(Singleton&)                   = delete;
    Singleton& operator=(Singleton&)        = delete;
    Singleton& operator=(const Singleton&)  = delete;
private:
    static T* instance;                     // 单例实例对象
    static std::mutex mtx;                  // 锁
};

template <typename T>
T* Singleton<T>::instance = nullptr;

template <typename T>
std::mutex Singleton<T>::mtx;

#define DECL_SINGLETON(T) friend class Singleton<T>;
#define INSTANCE_SINGLETON(T) (T::Instance())
#define DESTORY_SINGLETON(T) (T::Destroy())

#endif // !__SINGLETON_H__
