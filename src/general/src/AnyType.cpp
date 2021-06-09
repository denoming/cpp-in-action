#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

class Any;

template<class T>
T
castAny(Any&);
template<class T>
T
castAny(const Any&);
template<class T>
T*
castAny(Any*);
template<class T>
const T*
castAny(const Any*);

class Any {
public:
    Any() = default;

    Any(Any&& other)
        : _ptr(std::move(other._ptr))
    {
    }

    template<typename T>
    Any(const T& value)
        : _ptr{new Concrete<std::decay_t<T>>{value}}
    {
    }

    [[nodiscard]] const std::type_info&
    type() const
    {
        return empty() ? typeid(void) : _ptr->type();
    }

    void
    clear()
    {
        _ptr.reset();
    }

    [[nodiscard]] bool
    empty() const
    {
        return (_ptr == nullptr);
    }

    template<typename T>
    T&
    value()
    {
        return *pointer<T>();
    }

    template<typename T>
    const T&
    value() const
    {
        return *pointer<T>();
    }

    template<typename T>
    T*
    pointer()
    {
        if (empty()) {
            throw std::runtime_error{""};
        }
        return &static_cast<Concrete<T>*>(_ptr.get())->value;
    }

    template<typename T>
    const T*
    pointer() const
    {
        if (empty()) {
            throw std::runtime_error{""};
        }
        return &static_cast<const Concrete<T>*>(_ptr.get())->value;
    }

private:
    template<class T>
    friend T
    castAny(Any&);
    template<class T>
    friend T
    castAny(const Any&);
    template<class T>
    friend T
    castAny(Any*);
    template<class T>
    friend T
    castAny(const Any*);

    struct Placeholder {
        virtual ~Placeholder() = default;

        virtual const std::type_info&
        type() const = 0;

        virtual std::unique_ptr<Placeholder>
        clone() = 0;
    };

    template<typename T>
    struct Concrete : public Placeholder {
        Concrete(T&& aValue)
            : value{std::move(aValue)}
        {
        }

        Concrete(const T& aValue)
            : value{aValue}
        {
        }

        const std::type_info&
        type() const override
        {
            return typeid(T);
        }

        std::unique_ptr<Placeholder>
        clone() override
        {
            return std::make_unique<Concrete>(value);
        }

        T value;
    };

private:
    std::unique_ptr<Placeholder> _ptr;
};

template<class T>
T
castAny(Any& object)
{
    const auto n1 = typeid(T).name();
    const auto n2 = object.type().name();

    if (object.type() != typeid(T)) {
        throw std::bad_cast{};
    }
    return object.template value<T>();
}

template<class T>
T
castAny(const Any& object)
{
    if (object.type() != typeid(T)) {
        throw std::bad_cast{};
    }
    return object.template value<T>();
}

TEST(AnyType, Create)
{
    Any value{5};
    EXPECT_EQ(castAny<int>(value), 5);
}
