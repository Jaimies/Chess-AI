#include "transposition_table.h"

AtomicTranspositionPtr _TranspositionTable::get(int64_t hash) const {

}

void _TranspositionTable::insert(uint64_t hash, Transposition transposition) {
    AtomicTranspositionPtr ptr(new std::atomic<Transposition>(transposition));
    transpositions.insert({{hash, ptr}});
}
