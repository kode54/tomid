#ifndef NALL_UTF8_HPP
#define NALL_UTF8_HPP

//UTF-8 <> UTF-16 conversion
//used only for Win32; Linux, etc use UTF-8 internally

#if defined(_WIN32)

#undef UNICODE
#undef _WIN32_WINNT
#undef  NOMINMAX
#define UNICODE
#define _WIN32_WINNT 0x0501
#define NOMINMAX
#include <winsock2.h>
#include <windows.h>
#undef interface

namespace nall {
  //UTF-8 to UTF-16
  class utf16_t {
  public:
    operator wchar_t*() {
      return buffer;
    }

    operator const wchar_t*() const {
      return buffer;
    }

    utf16_t(const char *s = "") {
      if(!s) s = "";
      unsigned length = MultiByteToWideChar(CP_UTF8, 0, s, -1, 0, 0);
      buffer = new wchar_t[length + 1]();
      MultiByteToWideChar(CP_UTF8, 0, s, -1, buffer, length);
    }

    ~utf16_t() {
      delete[] buffer;
    }

  private:
    wchar_t *buffer;
  };

  //UTF-16 to UTF-8
  class utf8_t {
  public:
    operator char*() {
      return buffer;
    }

    operator const char*() const {
      return buffer;
    }

    utf8_t(const wchar_t *s = L"") {
      if(!s) s = L"";
      unsigned length = WideCharToMultiByte(CP_UTF8, 0, s, -1, 0, 0, (const char*)0, (BOOL*)0);
      buffer = new char[length + 1]();
      WideCharToMultiByte(CP_UTF8, 0, s, -1, buffer, length, (const char*)0, (BOOL*)0);
    }

    ~utf8_t() {
      delete[] buffer;
    }

  private:
    char *buffer;
  };

  struct utf8_args {
    int argc() const { return _argc; }
    char const* const* argv() const { return _argv; }

    utf8_args(int unused, char ** unused2) {
      (void)unused;
      (void)unused2;
      typedef int (__cdecl *__wgetmainargs_)(int*, wchar_t***, wchar_t***, int, int*);
      __wgetmainargs_ __wgetmainargs;
      HMODULE module;
      wchar_t **wargv;
      wchar_t **wenv;
      int i;
      module = LoadLibraryA("msvcrt.dll");
      __wgetmainargs = (__wgetmainargs_)GetProcAddress(module, "__wgetmainargs");
      __wgetmainargs(&_argc, &wargv, &wenv, 1, &i);
      _argv = new char*[_argc];
      for(unsigned i = 0; i < _argc; i++) {
        nall::utf8_t arg(wargv[i]);
        _argv[i] = new char[strlen(arg) + 1];
        strcpy(_argv[i], arg);
      }
      FreeLibrary(module);
    }

    ~utf8_args() {
      for (unsigned i = 0; i < _argc; i++) {
        delete [] _argv[i];
      }
      delete [] _argv;
    }

  private:
    int _argc;
    char ** _argv;
  };

  static inline FILE* fopen_utf8(char const* path, char const* mode) {
    return _wfopen( utf16_t( path ), utf16_t( mode ) );
  }
}

#else  //if defined(_WIN32)

namespace nall {
  struct utf8_args {
    int argc() const { return _argc; }
    char const* const* argv() { return _argv; }

    utf8_args(int p_argc, char const* const* p_argv)
    : _argc( p_argc ), _argv( p_argv ) { }

  private:
    int _argc;
    char const* const* _argv;
  };

  static inline FILE* fopen_utf8(char const* path, char const* mode) {
    return fopen( path, mode );
  }
}

#endif

#endif
