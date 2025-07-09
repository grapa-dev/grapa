#include <stdio.h>
#include "utf8proc.h"

int main() {
    const char* input = "café";
    utf8proc_uint8_t* nfd = NULL;
    utf8proc_ssize_t nfd_len = utf8proc_map(
        (const utf8proc_uint8_t*)input,
        0,
        &nfd,
        (utf8proc_option_t)(UTF8PROC_STABLE | UTF8PROC_DECOMPOSE)
    );
    printf("nfd_len = %lld, nfd = %p\n", (long long)nfd_len, (void*)nfd);
    if (nfd && nfd_len > 0) {
        printf("NFD bytes: ");
        for (utf8proc_ssize_t i = 0; i < nfd_len; ++i) {
            printf("%02X ", nfd[i]);
        }
        printf("\nNFD as string: %s\n", nfd);
        free(nfd);
    } else {
        printf("NFD normalization failed.\n");
    }
    return 0;
} 