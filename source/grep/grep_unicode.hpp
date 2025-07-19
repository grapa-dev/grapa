#include "grapa_grep_unicode.hpp"

#ifdef USE_PCRE
#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2/pcre2.h"
#endif 