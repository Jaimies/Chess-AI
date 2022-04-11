#pragma once

#include <tbb/concurrent_hash_map.h>
#include <vector>
#include "transposition.h"

typedef tbb::concurrent_hash_map<uint64_t, AtomicTranspositionPtr> TranspositionTable;

extern bool analysisStopped;
