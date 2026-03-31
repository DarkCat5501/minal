#pragma once
#include <dlfcn.h>
#include <stdint.h>

enum {
  XMOD_ERR_LOADING = -3, // could not load the module
  XMOD_ERR_IMPORT = -2,  // could not import module function
  XMOD_ERR_INIT = -1,    // module not initialized correctly
  XMOD_ERR = 0,          // unknown error
  XMOD_OK,
};

typedef void *(*MemAllocFn)(void *, size_t);
typedef void (*MemFreeFn)(void *);
typedef struct { MemAllocFn alloc; MemFreeFn free; } XModAllocator;

typedef struct XMod XMod;
typedef int (*XModSetup)(XMod *module);  // callback to setup module
typedef int (*XModCleanupFn)(void);      // handler for module cleanup
typedef int (*XModUpdateFn)(void *data); // handle for update function
typedef int (*XModEventFn)(void *event, void *data); // handler of events
typedef int (*XModFn)(void *data, ...);              // generic module function

typedef struct {
  XModSetup setup;
  XModUpdateFn update;
  XModCleanupFn cleanup;
  XModEventFn events;
} XModCallbacks;

typedef struct {
  uint32_t load_time;
  int loaded : 1;
  int reload : 1;
} XModState;

struct XMod {
  const char *path;     // path to the dinamic library
  const char *setup_fn; // setup_function
  XModAllocator *allocator;
  // private members
  XModState _state;
  void *_lib; // pointer to the loaded library
  XModCallbacks _callbacks;
};

char *xmod_error();
int xmod_load(XMod *mod, XModAllocator *alloc);
int xmod_loadv(XMod modules[], size_t count, XModAllocator *alloc);
void xmod_check(XMod *mod);
void xmod_checkv(XMod modules[], size_t count);
int xmod_validate(const XMod *mod);
void xmod_cleanup(XMod *mod);
void xmod_cleanupv(XMod modules[], size_t count);
int xmod_cb_update(const XMod *mod, void *data);
void xmod_cb_updatev(const XMod modules[], size_t count, void *data);
int xmod_cb_events(const XMod *mod, void *event, void *data);
int xmod_cb_eventsv(const XMod modules[], size_t count, void *event, void *data);


#ifdef XMOD_IMPL

#include <errno.h>
#include <sys/stat.h>

char *xmod_error() { return dlerror(); }

void xmod_cleanup(XMod *mod) {
  if (mod->_callbacks.cleanup)
    mod->_callbacks.cleanup();
  if (mod->_lib)
    dlclose(mod->_lib);
  mod->_lib = NULL;
  mod->_state = (XModState){0};
  mod->_callbacks = (XModCallbacks){0};
}

void xmod_cleanupv(XMod modules[], size_t count) {
  for (size_t i = 0; i < count; i++) {
    xmod_cleanup(&modules[i]);
  }
}

int xmod_validate(const XMod *mod) {
  return mod->setup_fn != NULL && mod->path != NULL;
}

static int xmod_load_fn(const XMod *module, const char *fn_name,
                        void **fn_ptr) {
  if (module->_lib == NULL)
    return XMOD_ERR_INIT;

  dlerror();
  void *fn = dlsym(module->_lib, fn_name);
  if (fn == NULL)
    return XMOD_ERR_IMPORT;
  else if (fn_ptr != NULL)
    *fn_ptr = fn;
  return XMOD_OK;
}

static int xmod_needs_reload(XMod *mod) {
  struct stat statbuf = {0};
  if (stat(mod->path, &statbuf) < 0) {
    printf("Failed to stat: %s\n", strerror(errno));
    return -1;
  }
  uint32_t time = statbuf.st_mtime;
  int reload = time > (mod->_state.load_time+1);
  if (!mod->_state.loaded && reload) {
    printf("fist\n");
    mod->_state.load_time = time;
  } else if (reload) {
    printf("reload\n");
    mod->_state.reload = 1;
  }
  // printf("time: %d %zu, %zu\n", reload, time, mod->_state.load_time);
  return 0;
}

int xmod_load(XMod *mod, XModAllocator *alloc) {
  if (!xmod_validate(mod))
    return XMOD_ERR;
  if (!mod->allocator && alloc)
    mod->allocator = alloc;

  dlerror();
  mod->_lib = dlopen(mod->path, RTLD_NOW | RTLD_LOCAL);
  if (mod->_lib == NULL)
    return XMOD_ERR_LOADING;
  int err = xmod_load_fn(mod, mod->setup_fn, (void **)&mod->_callbacks.setup);
  if (err == XMOD_OK) {
    err = mod->_callbacks.setup(mod);
    mod->_state.loaded = xmod_needs_reload(mod) != -1;
  }

  printf("loaded module %s, {%s,%s}\n", mod->path,
         mod->_state.loaded ? "true" : "false",
         mod->_state.reload ? "true" : "false");

  return err;
}

int xmod_loadv(XMod modules[], size_t count, XModAllocator *alloc) {
  printf("loadind %zu modules\n", count);
  for (size_t i = 0; i < count; i++) {
    XMod *mod = &modules[i];
    int err = xmod_load(mod, alloc);
    if (err != XMOD_OK)
      return err;
  }
  return XMOD_OK;
}

void xmod_check(XMod *mod) {
  if (xmod_needs_reload(mod) != -1) {
    if (mod->_state.reload) {
      xmod_cleanup(mod);
      int err = xmod_load(mod, NULL);
      if (err == XMOD_OK) {
        printf("module reloaded:%s (%p)\n", mod->path, mod->_callbacks.setup);
      } else {
        fprintf(stderr, "Relload reloading module %d %s: %s\n", err, mod->path,
                xmod_error());
      }
    } else if (!mod->_state.loaded) {
      xmod_load(mod, NULL);
    }
  } else {
    fprintf(stderr, "Failed to stat %s\n", strerror(errno));
  }
}

void xmod_checkv(XMod modules[], size_t count) {
  for (size_t i = 0; i < count; i++) {
    xmod_check(&modules[i]);
  }
}

int xmod_cb_update(const XMod *mod, void *data) {
  if (mod->_callbacks.update)
    return mod->_callbacks.update(data);
  return 0;
}
void xmod_cb_updatev(const XMod modules[], size_t count, void *data) {
  for (size_t i = 0; i < count; i++) {
    xmod_cb_update(&modules[i], data);
  }
}

int xmod_cb_events(const XMod *mod, void *event, void *data) {
  if (!mod->_callbacks.events)
    return !mod->_callbacks.events(event, data);
  return 0;
}
int xmod_cb_eventsv(const XMod modules[], size_t count, void *event,
                    void *data) {

  for (size_t i = 0; i < count; i++) {
    int res = xmod_cb_events(&modules[i], event, data);
    if (res != 0)
      return res;
  }
  return 0;
}

#endif // XMOD_IMPL
