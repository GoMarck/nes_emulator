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

// Same as above but for stream API calls like fread() and fwrite().
#define STREAM_RETRY_ON_EINTR(nread, stream, expr)                  \
  do {                                                              \
    static_assert(std::is_unsigned<decltype(nread)>::value == true, \
                  #nread " must be an unsigned integer");           \
    (nread) = (expr);                                               \
  } while ((nread) == 0 && ferror(stream) == EINTR)