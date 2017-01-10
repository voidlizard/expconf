#ifndef __cpp_sodomy_h
#define __cpp_sodomy_h

#define VA_LENGTH(...) VA_LENGTH_(,##__VA_ARGS__,10,9,8,7,6,5,4,3,2,1,0)
#define VA_LENGTH_(_00, _0,_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N

#define ITERATE(what, ...) ITERATE__(what, VA_LENGTH(__VA_ARGS__), __VA_ARGS__)
#define ITERATE__(f, n, ...) ITERATE_(f, n, __VA_ARGS__)
#define ITERATE_(f, n, ...) _ITERATE_##n(f, __VA_ARGS__)

#define _ITERATE_0(what, x, ...)
#define _ITERATE_1(what, x, ...)  what(1,x)
#define _ITERATE_2(what, x, ...)  what(2,x)ISEP _ITERATE_1(what, __VA_ARGS__)
#define _ITERATE_3(what, x, ...)  what(3,x)ISEP _ITERATE_2(what, __VA_ARGS__)
#define _ITERATE_4(what, x, ...)  what(4,x)ISEP _ITERATE_3(what, __VA_ARGS__)
#define _ITERATE_5(what, x, ...)  what(5,x)ISEP _ITERATE_4(what, __VA_ARGS__)
#define _ITERATE_6(what, x, ...)  what(6,x)ISEP _ITERATE_5(what, __VA_ARGS__)
#define _ITERATE_7(what, x, ...)  what(7,x)ISEP _ITERATE_6(what, __VA_ARGS__)
#define _ITERATE_8(what, x, ...)  what(8,x)ISEP _ITERATE_7(what, __VA_ARGS__)
#define _ITERATE_9(what, x, ...)  what(9,x)ISEP _ITERATE_8(what, __VA_ARGS__)
#define _ITERATE_10(what, x, ...) what(10,x)ISEP _ITERATE_9(what, __VA_ARGS__)

#define IFNOTNULL(what, ...) IFNOTNULL__(what, VA_LENGTH(__VA_ARGS__), __VA_ARGS__)
#define IFNOTNULL__(w,n,...) IFNOTNULL_(w,n)
#define IFNOTNULL_(w,n) _IFNOTNULL##n(w)
#define _IFNOTNULL0(w) w()
#define _IFNOTNULL1(w) w(ISEP)
#define _IFNOTNULL2(w) w(ISEP)
#define _IFNOTNULL3(w) w(ISEP)
#define _IFNOTNULL4(w) w(ISEP)
#define _IFNOTNULL5(w) w(ISEP)
#define _IFNOTNULL6(w) w(ISEP)
#define _IFNOTNULL7(w) w(ISEP)
#define _IFNOTNULL8(w) w(ISEP)
#define _IFNOTNULL9(w) w(ISEP)
#define _IFNOTNULL10(w) w(ISEP)

#define PUTSEP(...) PUTSEP_(__VA_ARGS__)
#define PUTSEP_(...) __VA_ARGS__

#endif
