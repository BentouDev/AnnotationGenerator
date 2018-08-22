#ifdef ANNOTATION_GENERATOR
struct __Y_CLASS__
{
    __Y_CLASS__(const char* args)
    { }
};

#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define MAKE_UNIQUE(x) CONCATENATE(x, __COUNTER__)

#define Meta(...) __attribute__((annotate(#__VA_ARGS__)))
#define $YClass(...) __Y_CLASS__ MAKE_UNIQUE(__y_class__)( #__VA_ARGS__ );
#else
#define Meta(...)
#define $YClass(...)
#endif
