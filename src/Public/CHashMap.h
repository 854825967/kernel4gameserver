#ifndef CHASHMAP_H
#define CHASHMAP_H

#if defined WIN32 || defined WIN64
#include <hash_map>
using namespace std;
using namespace stdext;
#define CHashMap hash_map
#elif defined linux
#include <tr1/unordered_map>
using namespace std;
using namespace std::tr1;
#define CHashMap unordered_map
#elif defined CC_TARGET_OS_IPHONE | defined __ANDROID__
#include <map>
#define CHashMap map
using namespace std;
#endif //defined WIN32 || defined WIN64

#endif //CHASHMAP_H
