
#ifdef SINGLETONPOINTER_H
#undef SINGLETONPOINTER_H
#endif

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//#include "SingletonPointer_sf.h"
#else
#include "SingletonPointer_p.h"
#endif
