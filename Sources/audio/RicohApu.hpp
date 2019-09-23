// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "core/Memory.hpp"
#include "core/CoreConfig.hpp"
#include "utils/Types.hpp"
#include "utils/ClockDivider.hpp"
#include "input/InputPort.hpp"
#include "video/RicohPPU.hpp"
#include "audio/Sequencer.hpp"
#include "audio/AudioBuffer.hpp"
#include "audio/AudioChannel.hpp"
#include "audio/Mixer.hpp"
#include "audio/PulseChannel.hpp"
#include "audio/TriangleChannel.hpp"
#include "audio/NoiseChannel.hpp"
#include "audio/DMCChannel.hpp"

#include <vector>
#include <tuple>

struct RicohAPU
	: public CoreConfig
{
	RicohAPU ()
	{}

	template <BusOperation _Operation, typename _Host, typename _Data>
	auto tick (_Host& host, word addr, _Data&& data)
	{
		_trich.step<0b0001> (host);
		if (_clock_osc.tick ())
		{
			_sq0ch.step<0b0001> (host);
			_sq1ch.step<0b0001> (host);
			_noich.step<0b0001> (host);
			_dmcch.step<0b0001> (host);
		}

		if (_clock_mix.tick ())
			_mix.step (host, _sq0ch, _sq1ch, _trich, _noich, _dmcch);
		if (_clock_seq.tick ())
			_seq.step (host, _sq0ch, _sq1ch, _trich, _noich, _dmcch);

		host.irq (_seq.irq () || _dmcch.irq());

		if (addr >= 0x4000u && addr < 0x4200u)
		{
			addr -= 0x4000u;
			addr &= 0x1fu;

			if constexpr (_Operation == kPeek)
				data = latch;
			if constexpr (_Operation == kPoke)
				latch = byte (data);

			_sq0ch.tick<_Operation> (host, addr, latch);
			_sq1ch.tick<_Operation> (host, addr, latch);
			_trich.tick<_Operation> (host, addr, latch);
			_noich.tick<_Operation> (host, addr, latch);
			_dmcch.tick<_Operation> (host, addr, latch);

			switch (addr)
			{
			case 0x15:
				if constexpr (_Operation == kPoke)
				{
					auto [sq0e, sq1e, trie, noie, dmce, _]
						= bits::unpack_as_tuple<1, 1, 1, 1, 1, 3> (latch);
					_sq0ch.enable (sq0e);
					_sq1ch.enable (sq1e);
					_trich.enable (trie);
					_noich.enable (noie);
					_dmcch.enable (dmce);
				}
				if constexpr (_Operation == kPeek)
				{
					byte sq0e = !!_sq0ch.status();
					byte sq1e = !!_sq1ch.status();
					byte trie = !!_trich.status();
					byte noie = !!_noich.status();
					byte dmce = !!_dmcch.status();
					data = bits::pack<1,1,1,1,1,1,1,1>(sq0e, sq1e, trie, noie, dmce, 0, _seq.irq_rr(), _dmcch.irq());
				}
				break;
			case 0x16:
				if constexpr (_Operation == kPoke)
					_input.write (latch);
				if constexpr (_Operation == kPeek)
					_input.read<0> (data);
				break;
			case 0x17:
				if constexpr (_Operation == kPoke)
				{	
					auto [_, i, m] = bits::unpack_as_tuple<6,1,1>(latch);
					_seq.mode5(m);
					_seq.irqdi(i);
				}
				if constexpr (_Operation == kPeek)
					_input.read<1> (data);
				break;
			}
		}
		return kSuccess;
	}

	template <ResetType _Type>
	void reset ()
	{}

	template <typename _Sink>
	void grabFrame (_Sink&& sink)
	{
		_mix.grab (sink);
	}

	template <typename... _Args>
	auto input (_Args&& ... args)
	{
		return _input.set (std::forward<_Args> (args)...);
	}

private:

	byte latch{0u};
	ClockDivider<ctCPUTicksPerSecond, ctSamplingRate> _clock_mix;
	ClockDivider<ctCPUTicksPerSecond, ctSEQTicksPerSecond> _clock_seq;
	ClockDivider<ctCPUTicksPerSecond, ctCPUTicksPerSecond / 2> _clock_osc;
	InputPort	_input;
	Sequencer _seq;
	PulseChannel<0> _sq0ch;
	PulseChannel<1> _sq1ch;

	NoiseChannel _noich;
	DMCChannel _dmcch;
	TriangleChannel _trich;

	Mixer _mix;
};
