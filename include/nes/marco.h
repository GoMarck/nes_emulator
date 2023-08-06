#pragma once

#define RETURN_FALSE_IF(_statement) \
  do {                              \
    if ((_statement)) {             \
      return false;                 \
    }                               \
  } while (false)

#define RETURN_FALSE_REPENT_IF(_statement, _msg) \
  do {                                           \
    if ((_statement)) {                          \
      NES_LOG(ERROR) << (_msg);                  \
      return false;                              \
    }                                            \
  } while (false)
