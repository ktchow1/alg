#pragma once
#include<mutex>
#include<atomic>


namespace alg
{
    template<typename T>
    class singleton0 
    {
    private:
        singleton0() = default;
       ~singleton0() = default;
        singleton0(const singleton0<T>&) = delete;
        singleton0<T>& operator=(const singleton0<T>&) = delete;

    public:
        static T& get_instance()
        {
            static singleton0 x;
            return x.m_impl;
        }

    private:
        T m_impl;
    };
}


namespace alg
{
    template<typename T>
    class singleton1
    {
    private:
        singleton1() = default;
       ~singleton1() = default;
        singleton1(const singleton1<T>&) = delete;
        singleton1<T>& operator=(const singleton1<T>&) = delete;

    public:
        static T& get_instance()
        {
            std::call_once(flag, []() { px = new singleton1<T>; });        
            return px->m_impl;
        }

        static singleton1<T>* px;
        static std::once_flag flag;

    private:
        T m_impl;
    };

    template<typename T>
    singleton1<T>* singleton1<T>::px = nullptr;

    template<typename T>
    std::once_flag singleton1<T>::flag;
}


namespace alg
{
    template<typename T>
    class singleton2
    {
    private:
        singleton2() = default;
       ~singleton2() = default;
        singleton2(const singleton2<T>&) = delete;
        singleton2<T>& operator=(const singleton2<T>&) = delete;

    public:
        static T& get_instance()
        {
            std::lock_guard<std::mutex> lock(m);
            if (!px) { px = new singleton2<T>; };
            return px->m_impl;
        }

        static singleton2<T>* px;
        static std::mutex m;

    private:
        T m_impl;
    };

    template<typename T>
    singleton2<T>* singleton2<T>::px = nullptr;

    template<typename T>
    std::mutex singleton2<T>::m;
}


namespace alg
{
    template<typename T>
    class singleton3
    {
    private:
        singleton3() = default;
        ~singleton3() = default;
        singleton3(const singleton3<T>&) = delete;
        singleton3<T>& operator=(const singleton3<T>&) = delete;

    public:
        static T& get_instance()
        {
            singleton3<T>* px = apx.load();
            if (!px) // double checked loop pattern
            {
                std::lock_guard<std::mutex> lock(m);
                if (!px) 
                {
                    px = new singleton3<T>; 
                    apx.store(px);
                };
            }
            return px->m_impl;
        }

        static std::atomic<singleton3<T>*> apx;
        static std::mutex m;

    private:
        T m_impl;
    };

    template<typename T>
    std::atomic<singleton3<T>*> singleton3<T>::apx = nullptr;

    template<typename T>
    std::mutex singleton3<T>::m;
}
