export module common;

export constexpr bool DEBUG =
#ifndef NDEBUG
    true;
#else
    false;
#endif

export class Resolution
{
    public:
        unsigned int width;
        unsigned int height;
};
