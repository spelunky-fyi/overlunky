#include <tuple>
#include <type_traits>

template <typename fun>
struct MemberFun_t;

namespace
{
template <typename R, typename C, typename... Args>
struct MemberFunBase
{
    using Pointer = R (C::*)(Args...);
    using BaseLessType = R(Args...);
    using BaseType = C;
    using ReturnType = R;
    // can't transfer the parameters by itself
    using Parameters_Tuple = std::tuple<Args...>;
    static constexpr std::size_t ArgCount = sizeof...(Args);
    template <std::size_t N>
    using ArgType = std::tuple_element_t<N, Parameters_Tuple>;
};
} // namespace

template <typename R, typename C, typename... Args>
struct MemberFun_t<R (C::*)(Args...)> : MemberFunBase<R, C, Args...>
{
};
template <typename R, typename C, typename... Args>
struct MemberFun_t<R (C::*)(Args...) const> : MemberFunBase<R, C, Args...>
{
};
template <typename R, typename C, typename... Args>
struct MemberFun_t<R (C::*)(Args...) volatile> : MemberFunBase<R, C, Args...>
{
};
template <typename R, typename C, typename... Args>
struct MemberFun_t<R (C::*)(Args...) const volatile> : MemberFunBase<R, C, Args...>
{
};
template <typename R, typename C, typename... Args>
struct MemberFun_t<R (C::*)(Args...) noexcept> : MemberFunBase<R, C, Args...>
{
};
template <typename R, typename C, typename... Args>
struct MemberFun_t<R (C::*)(Args...) const noexcept> : MemberFunBase<R, C, Args...>
{
};

template <typename R, typename C>
struct MemberFun_t<R (C::*)()> : MemberFunBase<R, C>
{
};
template <typename R, typename C>
struct MemberFun_t<R (C::*)() const> : MemberFunBase<R, C>
{
};
template <typename R, typename C>
struct MemberFun_t<R (C::*)() volatile> : MemberFunBase<R, C>
{
};
template <typename R, typename C>
struct MemberFun_t<R (C::*)() const volatile> : MemberFunBase<R, C>
{
};
template <typename R, typename C>
struct MemberFun_t<R (C::*)() noexcept> : MemberFunBase<R, C>
{
};
template <typename R, typename C>
struct MemberFun_t<R (C::*)() const noexcept> : MemberFunBase<R, C>
{
};

template <auto fun>
struct MemberFun
{
  private:
    using FunT = decltype(fun);
    static_assert(std::is_member_function_pointer_v<FunT>);

  public:
    using Pointer = MemberFun_t<FunT>::Pointer;
    using BaseLessType = MemberFun_t<FunT>::BaseLessType;
    using BaseType = MemberFun_t<FunT>::BaseType;
    using ReturnType = MemberFun_t<FunT>::ReturnType;
    // can't transfer the parameters by itself
    using Parameters_Tuple = MemberFun_t<FunT>::Parameters_Tuple;
    static constexpr std::size_t ArgCount = MemberFun_t<FunT>::ArgCount;
    template <std::size_t N>
    using ArgType = std::tuple_element_t<N, Parameters_Tuple>;
};
