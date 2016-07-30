#include "PNGRead.h"
#include "ColorType.h"


void PNGRead::readPngDataCallback(png_structp png_ptr, png_byte* raw_data, png_size_t read_length) {
    PNGRead::ReadDataHandle* handle = (PNGRead::ReadDataHandle*)png_get_io_ptr(png_ptr);
    const png_byte* png_src = handle->data.data + handle->offset;

    memcpy(raw_data, png_src, read_length);
    handle->offset += read_length;
}

PNGRead::PngInfo PNGRead::readAndUpdateInfo(const png_structp png_ptr, const png_infop info_ptr) {
    png_uint_32 width, height;
    int bit_depth, color_type;

    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

    // Convert transparency to full alpha
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    // Convert grayscale, if needed.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    // Convert paletted images, if needed.
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    // Add alpha channel, if there is none (rationale: GL_RGBA is faster than GL_RGB on many GPUs)
    if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_RGB)
        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

    // Ensure 8-bit packing
    if (bit_depth < 8)
        png_set_packing(png_ptr);
    else if (bit_depth == 16)
        png_set_scale_16(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    // Read the new color type after updates have been made.
    color_type = png_get_color_type(png_ptr, info_ptr);

    return (PngInfo) {width, height, color_type};
}

PNGRead::DataHandle PNGRead::readEntirePngImage(const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height) {
    const png_size_t row_size = png_get_rowbytes(png_ptr, info_ptr);
    const int data_length = (int)row_size * height;
    assert(row_size > 0);

    png_byte* raw_image = (png_byte*)malloc(data_length);
    assert(raw_image != NULL);

    png_byte* row_ptrs[height];

    png_uint_32 i;
    for (i = 0; i < height; i++) {
        row_ptrs[i] = raw_image + i * row_size;
    }

    png_read_image(png_ptr, &row_ptrs[0]);

    return (DataHandle) {raw_image, (png_size_t)data_length};
}

int PNGRead::getColorFormat(const int png_color_format) {
    assert(png_color_format == PNG_COLOR_TYPE_GRAY
           || png_color_format == PNG_COLOR_TYPE_RGB_ALPHA
           || png_color_format == PNG_COLOR_TYPE_GRAY_ALPHA);

    switch (png_color_format) {
        case PNG_COLOR_TYPE_GRAY:
            return S_COLOR_GRAY;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            return S_COLOR_RGB_ALPHA;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            return S_COLOR_GRAY_ALPHA;
    }

    return 0;
}

RawImage PNGRead::getRawImage(const void* png_data, const int png_data_size) {
    assert(png_data != NULL && png_data_size > 8);
    assert(png_sig_cmp((png_bytep)png_data, 0, 8) == 0);

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(png_ptr != NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    assert(info_ptr != NULL);

    PNGRead::ReadDataHandle png_data_handle = (PNGRead::ReadDataHandle) {{(png_byte*)png_data, (png_size_t)png_data_size}, 0};
    png_set_read_fn(png_ptr, &png_data_handle, readPngDataCallback);

    if (setjmp(png_jmpbuf(png_ptr))) {
        //  CRASH("Error reading PNG file!");
    }

    const PNGRead::PngInfo png_info = readAndUpdateInfo(png_ptr, info_ptr);
    const PNGRead::DataHandle raw_image = readEntirePngImage(png_ptr, info_ptr, png_info.height);

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    return RawImage((int)png_info.width, (int)png_info.height, (int)raw_image.size, getColorFormat(png_info.color_type), (void*)raw_image.data);

}
