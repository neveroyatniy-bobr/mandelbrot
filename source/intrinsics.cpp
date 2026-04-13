#include "intrinsics.hpp"

#include <SFML/Graphics.h>
#include <immintrin.h>

#include "prop.hpp"
#include "screen_state.hpp"

int IntrinsicsUpdatePixels(ScreenState screen_state, sfColor* pixels) {
    float inv_zoom = 1 / screen_state.zoom;

    __m256 rng8 = _mm256_set_ps(7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f);
    __m256 height8 = _mm256_set1_ps((float)HEIGHT);
    __m256 inv_zoom8 = _mm256_set1_ps(inv_zoom);

    for (int p_y = 0; p_y < HEIGHT; p_y++) {
        for (int p_x = 0; p_x < WIDTH / 8; p_x++) {
            __m256 p_x8 = _mm256_add_ps(_mm256_set1_ps((float)p_x*8), rng8);

            p_x8 = _mm256_div_ps(p_x8, height8);

            p_x8 = _mm256_add_ps(p_x8, _mm256_set1_ps(-((float)WIDTH / (float)HEIGHT) / 2.0f));

            p_x8 = _mm256_mul_ps(p_x8, inv_zoom8);

            __m256 cx8 = _mm256_add_ps(_mm256_set1_ps(screen_state.x_pos), p_x8);

            __m256 p_y8 = _mm256_set1_ps((float)p_y);

            p_y8 = _mm256_div_ps(p_y8, height8);

            p_y8 = _mm256_add_ps(p_y8, _mm256_set1_ps(-0.5f));

            p_y8 = _mm256_mul_ps(p_y8, inv_zoom8);

            p_x8 = _mm256_mul_ps(p_x8, _mm256_set1_ps(-1.0f));

            __m256 cy8 = _mm256_add_ps(_mm256_set1_ps(-screen_state.y_pos), p_y8);

            __m256i n8 = _mm256_setzero_si256();

            __m256 zx8 = _mm256_setzero_ps();
            __m256 zy8 = _mm256_setzero_ps();
            for (int i = 0; i < MAX_ITERATIONS; i++) {
                //(zx + izy)^2 = zx^2 - zy^2 + 2*zx*zy*

                __m256 zx8_2 = _mm256_mul_ps(zx8, zx8);
                __m256 zy8_2 = _mm256_mul_ps(zy8, zy8);

                __m256 new_zx8 = _mm256_add_ps(_mm256_sub_ps(zx8_2, zy8_2), cx8);

                __m256 zxzy8 = _mm256_mul_ps(zx8, zy8);
                __m256 new_zy8 = _mm256_add_ps(_mm256_add_ps(zxzy8, zxzy8), cy8);

                zx8 = new_zx8;
                zy8 = new_zy8;

                __m256 r8_2 = _mm256_add_ps(zx8_2, zy8_2);
                __m256 n4_8 = _mm256_set1_ps(4.0f);
                __m256i cmp8 = _mm256_castps_si256(_mm256_cmp_ps(n4_8, r8_2, _CMP_GT_OQ)); 

                n8 = _mm256_sub_epi32(n8, cmp8);

                if (_mm256_testz_si256(cmp8, cmp8)) {
                    break;
                }
            }
            
            __m256i r_div_255 = _mm256_set1_epi32(255);

            __m256i r8 = _mm256_and_si256(_mm256_mullo_epi32(n8, _mm256_set1_epi32(7)),  r_div_255);
            __m256i g8 = _mm256_and_si256(_mm256_mullo_epi32(n8, _mm256_set1_epi32(11)), r_div_255);
            __m256i b8 = _mm256_and_si256(_mm256_mullo_epi32(n8, _mm256_set1_epi32(17)), r_div_255);
            __m256i a8 = _mm256_set1_epi32(255);

            g8 = _mm256_slli_epi32(g8, 8);
            b8 = _mm256_slli_epi32(b8, 16);
            a8 = _mm256_slli_epi32(a8, 24);

            __m256i rgba8 = _mm256_or_si256(_mm256_or_si256(r8, g8), _mm256_or_si256(b8, a8));

            sfColor* start_pixel = pixels + p_y * WIDTH + p_x * 8;
            _mm256_storeu_si256((__m256i*)start_pixel, rgba8);
        }
    }

    return 0;
}