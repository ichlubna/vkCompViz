export module common;

export constexpr bool DEBUG =
#ifndef NDEBUG
    true;
#else
    false;
#endif
