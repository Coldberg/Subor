// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "core/Memory.hpp"
#include "core/CoreConfig.hpp"
#include "utils/Types.hpp"
#include "video/RicohPPU.hpp"

#include <vector>
#include <tuple>

struct RicohAPU
	: public CoreConfig
{
	using AudioBuffer = std::vector<std::pair<float, float>>;

	RicohAPU ()
	{}

	template <MemoryOperation _Operation, typename _Host, typename _Data>
	auto tick (_Host& host, word addr, _Data&& data)
	{		
		if (addr >= 0x4000u && addr < 0x4200u)
		{
			addr -= 0x4000u;
			addr &= 0x1fu;

			if constexpr (_Operation == kPeek)
				data = latch;
			if constexpr (_Operation == kPoke)
				latch = byte (data);

			switch (addr)
			{
			case 0x16:
				if constexpr (_Operation == kPoke)
					inputStrobe (data);
				if constexpr (_Operation == kPeek)
					inputRead<0> (data);
				break;
			case 0x17:
				if constexpr (_Operation == kPeek)
					inputRead<1> (data);
				break;
			}
		}
		return kSuccess;
	}

	template <ResetType _Type>
	void reset ()
	{
	}

	template <typename... Args>
	void input (Args&& ... values)
	{
		byte new_state [4u] = {values...};
		input_state [0] = new_state [0];
		input_state [1] = new_state [1];
		input_state [2] = new_state [2];
		input_state [3] = new_state [3];
	}

	template <typename _Sink>
	void grabFrame (_Sink&& sink)
	{
		sink (buffer);
		buffer.clear ();
	}

private:

	template<int _Port, typename _Value>
	void inputRead (_Value&& data)
	{
		data = (data & 0xe0) | (input_shift [_Port] & 1u);
		if (!(input_latch & 1u))
			input_shift [_Port] >>= 1u;
	}

	template<typename _Value>
	void inputStrobe (_Value&& data)
	{
		for (auto i = 0; i < std::size (input_shift); ++i)
		{
			input_latch = (byte)data;
			if (input_latch & 1u)
				input_shift [i] = input_state [i];
		}
	}

	float mix_audio (float sq0, float sq1, float tri, float noi, float dmc)
	{
		auto _sq0 = 15.0f  * sq0;
		auto _sq1 = 15.0f  * sq1;
		auto _tri = 15.0f  * tri;
		auto _noi = 15.0f  * noi;
		auto _dmc = 127.0f * dmc;

		float g0 = 0.0f;
		if (_tri || _noi || _dmc)
		{
			g0 = (_tri / 8227.0f) + (_noi / 12241.0f) + (_dmc / 22638.0f);
			g0 = 1.0f / g0 + 100.0f;
			g0 = 159.79f / g0;
		}

		float g1 = 0.0f;
		if (_sq0 || _sq1)
		{
			g1 = (8128.0f / (_sq0 + _sq1)) + 100.0f;
			g1 = 95.88f / g1;
		}

		return g0 + g1;		
	}


private:
	byte										latch{0u};
	byte										input_state [4u] = {0, 0, 0, 0};
	byte										input_shift [4u] = {0, 0, 0, 0};
	byte										input_latch{0};

	AudioBuffer							buffer;
};