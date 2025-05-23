# Copyright (c) 2020 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(MBED_ENABLE_OS_INTERNAL_TESTS)
    if(MBED_BUILD_GREENTEA_TESTS)
        add_subdirectory(tests/TESTS)
    else()
        add_subdirectory(tests/UNITTESTS)
    endif()
endif()

add_library(mbed-mbedtls STATIC EXCLUDE_FROM_ALL)

target_include_directories(mbed-mbedtls
    PUBLIC
        .
        ./include
        ./include/mbedtls
        ./platform
        ./platform/inc
)

target_sources(mbed-mbedtls
    PRIVATE
        platform/src/hash_wrappers.c
        platform/src/mbed_trng.cpp
        platform/src/platform_alt.cpp
        platform/src/shared_rng.cpp
        platform/src/timing_mbed.cpp

        source/aes.c
        source/aesni.c
        source/arc4.c
        source/aria.c
        source/asn1parse.c
        source/asn1write.c
        source/base64.c
        source/bignum.c
        source/blowfish.c
        source/camellia.c
        source/ccm.c
        source/certs.c
        source/chacha20.c
        source/chachapoly.c
        source/cipher.c
        source/cipher_wrap.c
        source/cmac.c
        source/ctr_drbg.c
        source/debug.c
        source/des.c
        source/dhm.c
        source/ecdh.c
        source/ecdsa.c
        source/ecjpake.c
        source/ecp.c
        source/ecp_curves.c
        source/entropy.c
        source/entropy_poll.c
        source/error.c
        source/gcm.c
        source/havege.c
        source/hkdf.c
        source/hmac_drbg.c
        source/md.c
        source/md2.c
        source/md4.c
        source/md5.c
        source/memory_buffer_alloc.c
        source/net_sockets.c
        source/nist_kw.c
        source/oid.c
        source/padlock.c
        source/pem.c
        source/pk.c
        source/pk_wrap.c
        source/pkcs11.c
        source/pkcs12.c
        source/pkcs5.c
        source/pkparse.c
        source/pkwrite.c
        source/platform.c
        source/platform_util.c
        source/poly1305.c
        source/ripemd160.c
        source/rsa.c
        source/rsa_internal.c
        source/sha1.c
        source/sha256.c
        source/sha512.c
        source/ssl_cache.c
        source/ssl_ciphersuites.c
        source/ssl_cli.c
        source/ssl_cookie.c
        source/ssl_msg.c
        source/ssl_srv.c
        source/ssl_ticket.c
        source/ssl_tls.c
        source/ssl_tls13_keys.c
        source/threading.c
        source/timing.c
        source/version.c
        source/version_features.c
        source/x509.c
        source/x509_create.c
        source/x509_crl.c
        source/x509_crt.c
        source/x509_csr.c
        source/x509write_crt.c
        source/x509write_csr.c
        source/xtea.c
)

target_compile_definitions(mbed-mbedtls
    PUBLIC
        MBED_CONF_MBEDTLS_PRESENT=1
)

target_link_libraries(mbed-mbedtls PUBLIC mbed-core-flags)

# Link Mbed's PSA implementation on enabled
if(("FEATURE_EXPERIMENTAL_API=1" IN_LIST MBED_TARGET_DEFINITIONS) AND
    ("FEATURE_PSA=1" IN_LIST MBED_TARGET_DEFINITIONS))
    target_link_libraries(mbed-mbedtls PUBLIC mbed-psa)
endif()

# Workaround for https://github.com/ARMmbed/mbedtls/issues/1077
# which affects cores without __thumb2__ set by the compiler
# due to the lack of full Thumb-2 support
set(partial-thumb2-cores
    Cortex-M0
    Cortex-M0+
    Cortex-M1
    Cortex-M23
    Cortex-M23-NS
)
if(MBED_CPU_CORE IN_LIST partial-thumb2-cores)
    target_compile_definitions(mbed-mbedtls
        PUBLIC
            MULADDC_CANNOT_USE_R7
    )
endif()
