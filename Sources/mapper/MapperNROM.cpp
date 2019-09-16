// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "MapperNROM.hpp"
#include "utils/Literals.hpp"

MapperNROM::MapperNROM (const ProgramROM& iNes)
: _ramSize{iNes.ramSize()}
, _hasTrainer (iNes.hasTrainer())
, _mirroring (iNes.mirroring ())
{	
	assert (iNes.idMapper() == 0u);
	iNes.prgTransfer(_prgBits);
	iNes.chrTransfer(_chrBits);
	iNes.trainerTransfer(_trainer);
}

word MapperNROM::ppuMirror (word addr) const
{
	return basicMirror (_mirroring, addr);
}
