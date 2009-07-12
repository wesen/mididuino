#include "List.hh"

class SeqEvent {
 public:
  uint32_t when;
  uint8_t type; // mask for freerunning, recurring
  uint16_t data;
};

ListPool<SeqEvent, 256> seqPool;

int main() {
  
  return 1;
}
