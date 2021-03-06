#pragma once

#include "Bitcommon.hpp"

template <std::size_t _Offset, std::size_t _Length, std::size_t _TotalBits = _Offset + _Length>
struct Bitfield 
{
	static constexpr const auto mask = __bits_hidden__::mask<_Offset, _Length>();
	using value_type = std::remove_const_t<decltype(mask)>;

	template <typename T>
	using ei = __bits_hidden__::ei<T>;
	
	constexpr Bitfield (value_type val)
	:	_value{ encode (val) }	
	{}

	constexpr Bitfield ()
	:	Bitfield(0u)
	{}

	constexpr operator value_type () const 
	{
		return extract (_value);
	}
	
	constexpr auto& operator = (value_type val)
	{
		emplace (_value, val);
		return *this;
	}

	constexpr auto& operator = (const Bitfield& val)
	{
		return *this = value_type(val);
	}


	template <typename _Color, ei<_Color> = 0> constexpr auto& operator += (_Color val) { return *this = _Color(*this) + val; }
	template <typename _Color, ei<_Color> = 0> constexpr auto& operator -= (_Color val) { return *this = _Color(*this) - val; }
	template <typename _Color, ei<_Color> = 0> constexpr auto& operator /= (_Color val) { return *this = _Color(*this) / val; }
	template <typename _Color, ei<_Color> = 0> constexpr auto& operator *= (_Color val) { return *this = _Color(*this) * val; }
	template <typename _Color, ei<_Color> = 0> constexpr auto& operator |= (_Color val) { return *this = _Color(*this) | val; }
	template <typename _Color, ei<_Color> = 0> constexpr auto& operator ^= (_Color val) { return *this = _Color(*this) ^ val; }
	template <typename _Color, ei<_Color> = 0> constexpr auto& operator &= (_Color val) { return *this = _Color(*this) & val; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator == (const Bitfield& a, _Color b) { return value_type(a) == b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator != (const Bitfield& a, _Color b) { return value_type(a) != b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator <= (const Bitfield& a, _Color b) { return value_type(a) <= b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator >= (const Bitfield& a, _Color b) { return value_type(a) >= b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  < (const Bitfield& a, _Color b) { return value_type(a)  < b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  > (const Bitfield& a, _Color b) { return value_type(a)  > b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  + (const Bitfield& a, _Color b) { return value_type(a)  + b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  - (const Bitfield& a, _Color b) { return value_type(a)  - b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  / (const Bitfield& a, _Color b) { return value_type(a)  / b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  * (const Bitfield& a, _Color b) { return value_type(a)  * b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  ^ (const Bitfield& a, _Color b) { return value_type(a)  ^ b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  & (const Bitfield& a, _Color b) { return value_type(a)  & b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  | (const Bitfield& a, _Color b) { return value_type(a)  | b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator >> (const Bitfield& a, _Color b) { return value_type(a) >> b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator << (const Bitfield& a, _Color b) { return value_type(a) << b; }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator == (_Color a, const Bitfield& b) { return a == value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator != (_Color a, const Bitfield& b) { return a != value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator <= (_Color a, const Bitfield& b) { return a <= value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator >= (_Color a, const Bitfield& b) { return a >= value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  < (_Color a, const Bitfield& b) { return a  < value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  > (_Color a, const Bitfield& b) { return a  > value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  + (_Color a, const Bitfield& b) { return a  + value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  - (_Color a, const Bitfield& b) { return a  - value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  * (_Color a, const Bitfield& b) { return a  * value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  / (_Color a, const Bitfield& b) { return a  / value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  ^ (_Color a, const Bitfield& b) { return a  ^ value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  & (_Color a, const Bitfield& b) { return a  & value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator  | (_Color a, const Bitfield& b) { return a  | value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator << (_Color a, const Bitfield& b) { return a << value_type(b); }
	template <typename _Color, ei<_Color> = 0> friend constexpr auto operator >> (_Color a, const Bitfield& b) { return a >> value_type(b); }
	constexpr auto& operator ++ () { return *this = value_type(*this) + 1u; }
	constexpr auto& operator -- () { return *this = value_type(*this) - 1u; }
	constexpr auto operator ++ (int) { auto p = *this; *this = value_type(*this) + 1u; return p; }
	constexpr auto operator -- (int) { auto p = *this; *this = value_type(*this) - 1u; return p; }

protected:
	static constexpr auto encode (value_type val) 
	{
		return value_type((val << _Offset) & mask);
	}
	static constexpr auto emplace (value_type& out, value_type val) 
	{
		out = (out & ~mask) | encode (val);
	}
	static constexpr auto extract (value_type val) 
	{
		return value_type((val & mask) >> _Offset);
	}

private:
	value_type _value;
};
