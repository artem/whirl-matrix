#pragma once

#include <cstdint>

// For debugging

uintptr_t GlobalAllocsCheckSum();

void ActivateAllocsTracker();
void PrintAllocsTrackerReport();
