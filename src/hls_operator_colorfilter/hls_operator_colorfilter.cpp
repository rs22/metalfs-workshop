#include "hls_common/mtl_stream.h"

#define HEADER_BYTES 138
#define HEADER_ELEMENTS (HEADER_BYTES / 8) + 1
#define HEADER_OFFSET (8 - (HEADER_BYTES % 8))

#define RED_FACTOR 54
#define GREEN_FACTOR 183
#define BLUE_FACTOR 18

snapu8_t grayscale(snapu8_t red, snapu8_t green, snapu8_t blue) {
  snapu16_t red_contrib   = ((snapu16_t)red)   * RED_FACTOR;
  snapu16_t green_contrib = ((snapu16_t)green) * GREEN_FACTOR;
  snapu16_t blue_contrib  = ((snapu16_t)blue)  * BLUE_FACTOR;
  return (uint8_t)(red_contrib   >> 8) +
         (uint8_t)(green_contrib >> 8) +
         (uint8_t)(blue_contrib  >> 8);
}

snapu32_t transform_pixel(snapu32_t pixel) {
  // Select the color channels by using the VHDL-inspired bit selection syntax
  snapu8_t alpha = pixel(31, 24);
  snapu8_t red   = pixel(23, 16);
  snapu8_t green = pixel(15,  8);
  snapu8_t blue  = pixel( 7,  0);

  snapu8_t gray = grayscale(red, green, blue);

  // Task 2: Return a grayscale pixel if red is not the dominant color
  if (red < green || red < blue) {
    return (alpha, gray, gray, gray);
  } else {
    return (alpha, red, green, blue);
  }
}

void process_stream(mtl_stream &in, mtl_stream &out) {
  mtl_stream_element input, output;
  snapu32_t i = 0;

  do {
    #pragma HLS PIPELINE
    input = in.read();
    output = input;

    // Task 1: Leave the bitmap header unmodified during processing.
    // The first HEADER_ELEMENTS words (as defined above) should be directly written
    // to the out stream.

    if (i >= HEADER_ELEMENTS) {

      // Each 8-byte input word contains data for two 4-byte pixels
      snapu32_t first_pixel_in = input.data(63, 32);
      snapu32_t second_pixel_in = input.data(31, 0);
      snapu32_t first_pixel_out = transform_pixel(first_pixel_in);
      snapu32_t second_pixel_out = transform_pixel(second_pixel_in);

      output.data = (first_pixel_out, second_pixel_out);
    }

    ++i;

    out.write(output);
  } while (!output.last);
}

void hls_operator_colorfilter(mtl_stream &in, mtl_stream &out) {

    #pragma HLS INTERFACE axis port=in name=axis_input
    #pragma HLS INTERFACE axis port=out name=axis_output
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    #pragma HLS DATAFLOW
    mtl_stream padded_stream;
    mtl_stream processed_stream;

    insert_padding<HEADER_OFFSET>(in, padded_stream);
    process_stream(padded_stream, processed_stream);
    remove_padding<HEADER_OFFSET>(processed_stream, out);
}

#include "testbench.cpp"
