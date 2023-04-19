#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <jpeglib.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdarg.h>

typedef struct {
    unsigned char *image;
    unsigned char *new_image;
    unsigned int start;
    unsigned int end;
} ARGUMENTS;

void sobel_filter(const unsigned char *image, unsigned char *new_image, struct jpeg_decompress_struct cinfo,
                  unsigned int start, unsigned int end);
void *thread_func(void *arg);

void error_msg(int condition, const char *fmt, ...);

unsigned char *image, *new_image;
struct jpeg_decompress_struct cinfo;

int main(int argc, char **argv) {
    error_msg(argc != 3, "Usage: program_name <file.jpg> <number_of_threads>.\n");

    struct timeval begin, end;
    struct jpeg_error_mgr jerr;
    int error_code;

    FILE *stream = fopen(argv[1], "rb");
    error_msg(!stream, "File %s can't be opened.\n", argv[1]);

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, stream);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    new_image =
        (unsigned char *)malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    image =
        (unsigned char *)malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);

    while (cinfo.output_scanline < cinfo.output_height) {
        unsigned char *row_ptr =
            &image[cinfo.output_width * cinfo.output_components * cinfo.output_scanline];
        jpeg_read_scanlines(&cinfo, &row_ptr, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(stream);

    int threadsNum = atoi(argv[2]);

    gettimeofday(&begin, NULL);

    pthread_t *threads = malloc(sizeof(pthread_t) * threadsNum);
    ARGUMENTS *args = malloc(sizeof(ARGUMENTS) * threadsNum);
    int chunk = cinfo.output_height / threadsNum;
    for (int i = 0; i < threadsNum; i++) {
        args[i].image = image;
        args[i].new_image = new_image;
        args[i].start = i * chunk;
        args[i].end = (i + 1) * chunk;
        if (i == threadsNum - 1) args[i].end = cinfo.output_height;

        error_code = pthread_create(&threads[i], NULL, thread_func, &args[i]);
        error_msg(error_code, "Thread %d creation fault.\n", i);
    }

    for (int i = 0; i < threadsNum; i++) {
        error_code = pthread_join(threads[i], NULL);
        error_msg(error_code, "Thread %d join fault.\n", i);
    }
    gettimeofday(&end, NULL); 
    
    FILE *outfile = fopen("result.jpg", "wb");
    error_msg(!outfile, "File result.jpg can't be opened.\n");

    struct jpeg_compress_struct cinfo_compress;
    struct jpeg_error_mgr jerr_compress;
    cinfo_compress.err = jpeg_std_error(&jerr_compress);
    jpeg_create_compress(&cinfo_compress);
    jpeg_stdio_dest(&cinfo_compress, outfile);

    cinfo_compress.image_width = cinfo.output_width;
    cinfo_compress.image_height = cinfo.output_height;
    cinfo_compress.input_components = cinfo.num_components;
    cinfo_compress.in_color_space = cinfo.out_color_space;

    jpeg_set_defaults(&cinfo_compress);
    jpeg_start_compress(&cinfo_compress, TRUE);

    while (cinfo_compress.next_scanline < cinfo_compress.image_height) {
        unsigned char *row_ptr =
            &new_image[cinfo_compress.image_width * cinfo_compress.input_components *
                       cinfo_compress.next_scanline];
        jpeg_write_scanlines(&cinfo_compress, &row_ptr, 1);
    }

    jpeg_finish_compress(&cinfo_compress);
    jpeg_destroy_compress(&cinfo_compress);

    fclose(outfile);
    free(image);
    free(new_image);
    free(threads);
    free(args);

    double time = ((double)(end.tv_sec - begin.tv_sec) + (double)(end.tv_usec - begin.tv_usec)) / 1000000.0;
    printf("Time spent: %f seconds\n", time);

    exit(EXIT_SUCCESS);
}

void *thread_func(void *arg) {
    ARGUMENTS *args = (ARGUMENTS *)arg;
    sobel_filter(args->image, args->new_image, cinfo, args->start, args->end);
    pthread_exit(NULL);
}

void sobel_filter(const unsigned char *image, unsigned char *new_image, struct jpeg_decompress_struct cinfo,
                  unsigned int start, unsigned int end) {
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    
    if (start == 0) start++;
    if (end == cinfo.output_height) end--;
    for (unsigned int y = start; y < end; y++) {
        for (unsigned int x = 1; x < cinfo.output_width - 1; x++) {
            int sum_x = 0;
            int sum_y = 0;

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int pixel_index = ((y + i) * cinfo.output_width + (x + j)) * cinfo.num_components;
                    sum_x += (image[pixel_index] * Gx[i + 1][j + 1]);
                    sum_y += (image[pixel_index] * Gy[i + 1][j + 1]);
                }
            }

            int gradient = (int)(sqrt(sum_x * sum_x + sum_y * sum_y));

            int new_pixel_index = (y * cinfo.output_width + x) * cinfo.num_components;
            new_image[new_pixel_index] = gradient;
            new_image[new_pixel_index + 1] = gradient;
            new_image[new_pixel_index + 2] = gradient;
        }
    }
}

void error_msg(int condition, const char *fmt, ...) {
    //#include <stdarg.h>
    if (condition) {
        va_list argp;
        va_start(argp, fmt);
        vprintf(fmt, argp);
        va_end(argp);
        exit(EXIT_FAILURE);
    }
}