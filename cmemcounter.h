#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
#include <string.h>
#include <stdint.h>

size_t malloced_memory_usage = 0;

size_t myalloc_cookie = 123456; // we can't easily prevent some free calls from coming to us from outside, mark them

void* malloc(size_t sz) {
    void *(*libc_malloc)(size_t) = dlsym(RTLD_NEXT, "malloc");
    void * answerplus =  libc_malloc(sz + sizeof(size_t) + sizeof(myalloc_cookie) );
    if(answerplus == NULL) return answerplus;// nothing can be done
    malloced_memory_usage += sz;
    memcpy(answerplus ,&myalloc_cookie,sizeof(myalloc_cookie));
    memcpy((char *) answerplus + sizeof(myalloc_cookie),&sz,sizeof(sz));
    return ((char *) answerplus) + sizeof(size_t) + sizeof(myalloc_cookie);
}


// can fail to produce an aligned result if alignment does not divide 2 * size_t
int posix_memalign(void **memptr, size_t alignment, size_t size) {
    int(*libc_posix_memalign)(void **, size_t, size_t) = dlsym(RTLD_NEXT, "posix_memalign");
    size_t offset = (sizeof(size_t) + sizeof(myalloc_cookie) );
    void * answerplus;
    int ret = libc_posix_memalign(&answerplus,alignment, size + offset);
    if(ret) return ret;// nothing can be done
    malloced_memory_usage += size;
    memcpy(answerplus ,&myalloc_cookie,sizeof(myalloc_cookie));
    memcpy((char *) answerplus + sizeof(myalloc_cookie),&size,sizeof(size));
    * memptr = (char *) answerplus + offset;
    return ret;
}

void * calloc(size_t count, size_t size) {
    size_t sz = count * size;
    void *(*libc_malloc)(size_t) = dlsym(RTLD_NEXT, "malloc");
    size_t volume = sz + sizeof(size_t) + sizeof(myalloc_cookie);
    void * answerplus =  libc_malloc(volume);
    memset(answerplus,0,volume);
    if(answerplus == NULL) return answerplus;// nothing can be done
    malloced_memory_usage += sz;
    memcpy(answerplus ,&myalloc_cookie,sizeof(myalloc_cookie));
    memcpy((char *) answerplus + sizeof(myalloc_cookie),&sz,sizeof(sz));
    return ((char *) answerplus) + sizeof(size_t) + sizeof(myalloc_cookie);
}

void free(void *p) {
    if(p == NULL) return; // nothing to do
    void (*libc_free)(void*) = dlsym(RTLD_NEXT, "free");
    void * truep = ((char *) p) - sizeof(size_t) - sizeof(myalloc_cookie);
    size_t cookie;
    // the cookie approach is kind of a hack, don't use in production code!
    memcpy(&cookie ,truep,sizeof(myalloc_cookie)); // in some case, this might read data outside of bounds
    if(cookie != myalloc_cookie) {
      libc_free(p);
      return;
    }
    size_t sz;
    memcpy(&sz,(char *) truep + sizeof(myalloc_cookie),sizeof(sz));
    malloced_memory_usage -= sz;
    libc_free(truep);
}


void* realloc(void *p, size_t sz) {
  if(p == NULL) return malloc(sz);
  // implement
  void *(*libc_realloc)(void *m,size_t) = dlsym(RTLD_NEXT, "realloc");
  void * truep = ((char *) p) - sizeof(size_t) - sizeof(myalloc_cookie);
  size_t cookie;
  // the cookie approach is kind of a hack, don't use in production code!
  memcpy(&cookie ,truep,sizeof(myalloc_cookie)); // in some case, this might read data outside of bounds
  if(cookie != myalloc_cookie) {
      return libc_realloc(p,sz);
  }
  size_t oldsz;
  memcpy(&oldsz,(char *) truep + sizeof(myalloc_cookie),sizeof(sz));
  malloced_memory_usage -= oldsz;
  void * newp = libc_realloc(truep,sz +  sizeof(size_t) + sizeof(myalloc_cookie));
  if(newp == NULL) return newp;// nothing can be done?
  malloced_memory_usage += sz;
  memcpy((char *) newp + sizeof(myalloc_cookie),&sz,sizeof(sz));
  return newp + sizeof(size_t) + sizeof(myalloc_cookie);
}

