# Tesseract OCR
unset(Tesseract_FOUND)

set(TESSERACT_VERSION 304)
set(LEPTONICA_VERSION 171)

#################
if(MSVC)
  if(CMAKE_CL_64)
    set(TESSERACT_ARCH x64)    
  else()
    set(TESSERACT_ARCH Win32)    
  endif()
  if(MSVC_VERSION EQUAL 1400)
    set(TESSERACT_RUNTIME vc8)
  elseif(MSVC_VERSION EQUAL 1500)
    set(TESSERACT_RUNTIME vc9)
  elseif(MSVC_VERSION EQUAL 1600)
    set(TESSERACT_RUNTIME vc10)
  elseif(MSVC_VERSION EQUAL 1700)
    set(TESSERACT_RUNTIME vc11)
  elseif(MSVC_VERSION EQUAL 1800)
    set(TESSERACT_RUNTIME vc12)
  endif()
endif()

#################

find_path(Tesseract_INCLUDE_DIR tesseract/baseapi.h
  HINTS
  /usr/include
  /usr/local/include)

find_path(Tesseract_LIB_DIR
  HINTS
  /usr/lib
  /usr/local/lib)

find_path(Leptonica_INCLUDE_DIR
  HINTS
  /usr/include
  /usr/local/include)
  
find_path(Leptonica_LIB_DIR
  HINTS
  /usr/lib
  /usr/local/lib)

if(Tesseract_LIB_DIR AND Tesseract_INCLUDE_DIR AND Leptonica_INCLUDE_DIR AND Leptonica_LIB_DIR)
    set(Tesseract_FOUND 1)
endif()

        
