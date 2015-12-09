#if defined (WIN32)// && defined (BUILD_SHARED_LIBS)
#if defined (_MSC_VER)
#pragma warning(disable: 4251)
#endif
#if defined(SurvecLib_EXPORTS)
     #define  SURVECLIB_EXPORT __declspec(dllexport)
   #else
     #define  SURVECLIB_EXPORT __declspec(dllimport)
   #endif
#else
  #define SURVECLIB_EXPORT
#endif