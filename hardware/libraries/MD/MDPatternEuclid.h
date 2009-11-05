#ifndef MD_PATTERN_EUCLID_H__
#define MD_PATTERN_EUCLID_H__

#include <MD.h>
#include "MDPitchEuclid.h"

class MDPatternEuclid : public MDPitchEuclid {
public:
  MDPattern pattern;

  MDPatternEuclid();

  void makeTrack(uint8_t trackNum);
};



#endif /* MD_PATTERN_EUCLID_H__ */
				 
