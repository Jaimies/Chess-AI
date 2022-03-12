#pragma once

#include <tbb/concurrent_hash_map.h>
#include <vector>

typedef tbb::concurrent_hash_map<uint64_t, int64_t> TranspositionTable;

extern std::vector<uint64_t> depthHashes;
