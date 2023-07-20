#include "SimpleSsrc.h"


uint32_t SsrcGenerator::ssrcBase = 1234567890;

uint32_t SsrcGenerator::GenerateSsrc()
{
   return ssrcBase++;
}

