# Install script for directory: C:/work/c++/testedProject/build/_deps/mbedtls-src/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/ConvexHull")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/MinGW/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/aes.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/aria.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/asn1.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/asn1write.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/base64.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/bignum.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/block_cipher.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/build_info.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/camellia.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ccm.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/chacha20.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/chachapoly.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/check_config.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/cipher.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/cmac.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/compat-2.x.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/config_adjust_legacy_crypto.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/config_adjust_legacy_from_psa.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/config_adjust_psa_from_legacy.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/config_adjust_psa_superset_legacy.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/config_adjust_ssl.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/config_adjust_x509.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/config_psa.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/constant_time.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ctr_drbg.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/debug.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/des.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/dhm.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ecdh.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ecdsa.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ecjpake.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ecp.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/entropy.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/error.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/gcm.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/hkdf.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/hmac_drbg.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/lms.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/mbedtls_config.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/md.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/md5.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/memory_buffer_alloc.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/net_sockets.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/nist_kw.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/oid.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/pem.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/pk.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/pkcs12.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/pkcs5.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/pkcs7.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/platform.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/platform_time.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/platform_util.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/poly1305.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/private_access.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/psa_util.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ripemd160.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/rsa.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/sha1.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/sha256.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/sha3.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/sha512.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ssl.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ssl_cache.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ssl_ciphersuites.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ssl_cookie.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/ssl_ticket.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/threading.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/threading_alt.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/timing.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/version.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/x509.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/x509_crl.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/x509_crt.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/mbedtls/x509_csr.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/psa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/build_info.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_adjust_auto_enabled.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_adjust_config_dependencies.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_adjust_config_key_pair_types.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_adjust_config_synonyms.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_builtin_composites.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_builtin_key_derivation.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_builtin_primitives.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_compat.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_config.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_driver_common.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_driver_contexts_composites.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_driver_contexts_key_derivation.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_driver_contexts_primitives.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_extra.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_legacy.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_platform.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_se_driver.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_sizes.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_struct.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_types.h"
    "C:/work/c++/testedProject/build/_deps/mbedtls-src/include/psa/crypto_values.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/work/c++/testedProject/build/_deps/mbedtls-build/include/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
