#include "skein.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void output_hex(u08b_t *data) {
    for (int i = 0; i < 128; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

// Counts the number of ones in a 32bit value
int num_ones(int32_t i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

// Computes the difference in bits between two 1024bit values
int diff(u08b_t *a, u08b_t *b) {
    int32_t *a2 = (int32_t *) a;
    int32_t *b2 = (int32_t *) b;
    int d = 0;
    for (int i = 0; i < 32; i++) {
        d += num_ones(a2[i] ^ b2[i]);
    }
    return d;
}

// Increments input value (assumed to be from the alphabet [a,z])
void next(u08b_t *n) {
    for (int i = 0;; i++) {
        n[i]++;
        if (n[i] <= 'z') {
            break;
        } else {
            n[i] = 'a';
        }
    }
}

void calc(Skein1024_Ctxt_t *ctx, char *in, int num_bytes, u08b_t *out) {
    Skein1024_Init(ctx, 1024);
    Skein1024_Update(ctx, in, num_bytes);
    Skein1024_Final(ctx, out);
}

int main() {
    char *xkcd_target = "5b4da95f5fa08280fc9879df44f418c8f9f12ba424b7757de02bbdfbae0d4c4fdf9317c80cc5fe04c6429073466cf29706b8c25999ddd2f6540d4475cc977b87f4757be023f19b8f4035d7722886b78869826de916a79cf9c94cc79cd4347d24b567aa3e2390a573a373a48a5e676640c79cc70197e1c5e7f902fb53ca1858b6";
    u08b_t target[128];
    // Convert from hex to a 1024bit value
    for (int i = 0; i < 128; i++) {
        sscanf(&xkcd_target[2*i], "%2hhx", &target[i]);
    }

    // Initialize our starting value, with some random digits at the end, 
    // so that this program can be run in parallel
    int8_t rand_str[9];
    srand(time(NULL));
    snprintf(rand_str, 9, "%08x", rand());
    int8_t cur[33];
    for (int i = 0; i < 32; i++) {
        cur[i] = 'a';
    }
    memcpy(&cur[24], rand_str, 9);

    // Brute force the hash
    u08b_t out[128];
    Skein1024_Ctxt_t ctx;
    int best = 1024;
    for(;;) {
        next(cur);
        calc(&ctx, cur, 32, out);
        int d = diff(out, target);
        if (d < best) {
            best = d;
            printf("new best (%d): %s\n", best, cur);
            printf("hashes to: ");
            output_hex(out);
        }
    }

    return 0;
}
