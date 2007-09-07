#ifndef DEFS_H
#define DEFS_H

#ifndef SIMPLE_MAN2HTML
#define SIMPLE_MAN2HTML
#endif

#include <string>
#include <map>
#include <list>
#include <vector>
#include <stack>
#include <ctime>
#include <cctype>
#include <cstdio>
#include <string.h>

#define kDebug(x) DummyOutput()
#define kWarning(x) DummyOutput()
#define BYTEARRAY(x) x

#define qstrncpy strncpy
#define qstrncmp strncmp
#define qstrdup strdup
#define qstrcmp strcmp
#define qstrlen strlen
#define qstrcpy strcpy

class DummyOutput
{
};

template<typename T>
const DummyOutput & operator << (const DummyOutput &o, const T &)
{
    return o;
}

template <typename C>
void qDeleteAll(const C &cont)
{
    for (typename C::const_iterator i = cont.begin(); i != cont.end(); ++i)
    {
        delete *i;
    }
}

class QByteArray : public std::string
{
    public:
        typedef std::string::iterator iterator;
        typedef std::string::const_iterator const_iterator;
        typedef std::string::size_type size_type;

        QByteArray()
        {
        }

        QByteArray(const char *str)
        : std::string(str)
        {
        }

        QByteArray(const char *buffer, std::string::size_type size)
        : std::string(buffer, size)
        {
        }

        QByteArray(const_iterator b, const_iterator e)
        : std::string(b, e)
        {
        }

        QByteArray(const QByteArray &str, size_type stridx, size_type strlen)
        : std::string(str, stridx, strlen)
        {
        }

        QByteArray trimmed() const
        {
            const_iterator b = begin();

            while (b != end() && std::isspace(*b))
            {
                ++b;
            }

            if (b == end())
            {
                return QByteArray();
            }

            const_iterator e = end() - 1;

            while (e != b && std::isspace(*e))
            {
                --e;
            }

            return QByteArray(b, e + 1);
        }

        using std::string::append;

        QByteArray &append(char x)
        {
            push_back(x);
            return *this;
        }

        bool isEmpty() const
        {
            return empty();
        }

        int indexOf(char x, int start_pos = 0) const
        {
            return find(x, start_pos);
        }

        QByteArray mid(size_type pos, size_type len = npos) const
        {
            return QByteArray(*this, pos, len);
        }

        QByteArray toLower() const
        {
            QByteArray ret(*this);

            for (iterator i = ret.begin(); i != ret.end(); ++i)
            {
                *i = std::tolower(*i);
            }

            return ret;
        }

        QByteArray left(size_type len) const
        {
            return QByteArray(0, len);
        }

        unsigned int toUInt(bool *ok = 0, int base = 10) const
        {
            char *endptr;
            unsigned int ret = std::strtoul(c_str(), &endptr, base);

            if (*endptr != '\0')
            {
                if (ok)
                {
                    *ok = false;
                }

                return 0;
            }

            if (ok)
            {
                *ok = true;
            }

            return static_cast<unsigned int>(ret);
        }

        QByteArray &setNum(unsigned int n, int base = 10)
        {
            char buffer[100];
            std::sprintf(buffer, "%ud", n);
            assign(buffer);
            return *this;
        }

        static QByteArray number(unsigned int n, int base = 10)
        {
            char buffer[100];
            std::sprintf(buffer, "%ud", n);
            return QByteArray(buffer);
        }

        QByteArray &prepend(const QByteArray &str)
        {
            assign(str + *this);
            return *this;
        }

        /*operator const char * () const
        {
            return c_str();
        }*/

        bool endsWith(char x) const
        {
            return *rbegin() == x;
        }
};

template <typename T>
class QStack : public std::stack<T>
{
    public:
        typedef std::stack<T> base_stack;

        T pop()
        {
            T ret = base_stack::top();
            base_stack::pop();
            return ret;
        }

        void clear()
        {
            while (!base_stack::empty())
            {
                base_stack::pop();
            }
        }
};

template <typename T>
class QList : public std::vector<T>
{
    public:
        typedef std::vector<T> base_vector;

        bool isEmpty() const
        {
            return base_vector::empty();
        }

        int count() const
        {
            return base_vector::size();
        }

        void append(const T &value)
        {
            base_vector::push_back(value);
        }

        void pop_front()
        {
            base_vector::erase(base_vector::begin());
        }
};

template <class T>
class QListIterator
{
    private:
        typename QList<T>::const_iterator m_iter;
        const QList<T> *m_pList;

    public:
        QListIterator(const QList<T> &list)
        : m_iter(list.begin()), m_pList(&list)
        {
            //*this = list.begin();
        }

        bool hasNext() const
        {
            return m_iter != m_pList->end();
        }

        const T &next()
        {
            return *++m_iter;
        }
};

template <typename Key, typename T>
class QMap : public std::map<Key, T>
{
    public:
        typedef std::map<Key, T> base_map;
        typedef typename base_map::iterator iterator;
        typedef typename base_map::const_iterator const_iterator;

        using base_map::insert;

        iterator insert(const Key &k, const T &t, bool overwrite = true)
        {
            std::pair<iterator, bool> i = insert(std::make_pair(k, t));

            if (i.second == false && overwrite)
            {
                find(k)->second = t;
            }

            return i.first;
        }

        int remove(const Key &k)
        {
            return erase(k);
        }
};

class QDate
{
    private:
        int m_year;
        int m_month;
        int m_day;
        int m_dayOfWeek;

    public:
        QDate(int y, int m, int d)
        : m_year(y), m_month(m), m_day(d)
        {
            std::tm init_time;
            init_time.tm_year = y;
            init_time.tm_mon = m - 1;
            init_time.tm_mday  = d;
            std::mktime(&init_time);
            m_dayOfWeek = init_time.tm_wday;

            if (m_dayOfWeek == 0)
            {
                m_dayOfWeek = 7;
            }
        }

        int year() const
        {
            return m_year;
        }

        int month() const
        {
            return m_month;
        }

        int day() const
        {
            return m_day;
        }

        int dayOfWeek() const
        {
            return m_dayOfWeek;
        }

        static QDate currentDate()
        {
            std::time_t tmp = std::time(0);
            std::tm *current_time = std::localtime(&tmp);

            return QDate(current_time->tm_year, current_time->tm_mon + 1, current_time->tm_mday);
        }
};

#endif
