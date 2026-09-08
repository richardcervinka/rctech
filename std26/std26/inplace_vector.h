#include <array>
#include <new>

namespace std26
{
    template<class T, std::size_t N>
    class inplace_vector
    {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = std::array<T, N>::iterator;
        using const_iterator = std::array<T, N>::const_iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr inplace_vector() noexcept = default;

        constexpr explicit inplace_vector(size_type count) : m_size{count}
        {
            if (count > N)
            {
                throw std::bad_alloc();
            }
            for (size_type i = 0; i < m_size; i++)
            {
                m_data[i] = {};
            }
        }

        constexpr reference at(size_type pos)
        {
            return m_data.at(pos);
        }

        constexpr const_reference at(size_type pos) const
        {
            return m_data.at(pos);
        }

        constexpr reference operator[](size_type pos)
        {
            return m_data[pos];
        }

        constexpr const_reference operator[](size_type pos) const
        {
            return m_data[pos];
        }

        constexpr reference front()
        {
            return m_data.front();
        }

        constexpr const_reference front() const
        {
            return m_data.front();
        }

        constexpr reference back()
        {
            return m_data[m_size];
        }

        constexpr const_reference back() const
        {
            return m_data[m_size];
        }

        constexpr T* data() noexcept
        {
            return m_data.data();
        }

        constexpr const T* data() const noexcept
        {
            return m_data.data();
        }

        constexpr bool empty() const noexcept
        {
            return m_size == 0;
        }

        constexpr size_type size() const
        {
            return m_size;
        }

        static constexpr size_type capacity() noexcept
        {
            return N;
        }

        static constexpr size_type max_size() noexcept
        {
            return N;
        }

        constexpr reference push_back(const T& value)
        {

            if (size() == capacity())
            {
                throw std::bad_alloc();
            }
            m_data[m_size] = value;
            m_size += 1;
            return back();
        }

        constexpr reference push_back(T&& value)
        {
            if (size() == capacity())
            {
                throw std::bad_alloc();
            }
            m_data[m_size] = std::move(value);
            m_size += 1;
            return back();
        }

        
        constexpr void pop_back()
        {
            m_size -= 1;
        }

        constexpr void clear() noexcept
        {
            m_size = 0;
        }

        constexpr iterator begin() noexcept
        {
            return m_data.begin();
        }

        constexpr const_iterator begin() const noexcept
        {
            return m_data.begin();
        }
        
        constexpr const_iterator cbegin() const noexcept
        {
            return m_data.cbegin();
        }

        constexpr iterator end() noexcept
        {
            return m_data.begin() + m_size;
        }

        constexpr const_iterator end() const noexcept
        {
            return m_data.begin() + m_size;
        }

        constexpr const_iterator cend() const noexcept
        {
            return m_data.cbegin() + m_size;
        }

    private:
        std::array<T, N> m_data;
        size_type m_size {0};
    };
};