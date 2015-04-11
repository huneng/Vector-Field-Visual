#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>


#define WIDTH 257
#define HEIGHT 257

#define EPSILON 1e-10

#define SHOW_IMAGE(x) {cv::imshow(#x, x); cv::waitKey();}


void generate_noise(uchar *data, int width, int height)
{
    int x, y;

    srand(time(NULL));

    for(y = 0; y < height; y++)
        for(x = 0; x < width; x++)
            data[y * width + x] = rand()%256;
}


//center radiate vector field
double generate_center_radiate_field(float *dx, float *dy, int width, int height)
{
    double cx = width / 2;
    double cy = height / 2;

    int x, y;

    for(y = 0; y < height; y++)
    {
        for(x = 0; x < width; x++)
        {
            double deltaX = cx - x;
            double deltaY = cy - y;
            double dist = sqrt( deltaX * deltaX + deltaY * deltaY ) + EPSILON;

            int idx = y * width + x;
            dx[idx] = deltaX / dist;
            dy[idx] = deltaY / dist;
        }
    }

}


//circle vector field
double generate_circle_field(float *dx, float *dy, int width, int height)
{
    double cx = width / 2;
    double cy = height / 2;

    int x, y;


    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < height; x++)
        {
            double deltaX = cx - x;
            double deltaY = cy - y;
            double dist = sqrt( deltaX * deltaX + deltaY * deltaY ) + EPSILON;

            int idx = y * width + x;

            dx[idx] = deltaY / dist;
            dy[idx] = - deltaX / dist;
        }
    }
}


int trace_pixel(uchar* noise, float *dx, float *dy, double x, double y, int width, int height, int step)
{
    int ix, iy;
    int noise_color = 0;
    double next_x, next_y;

    while(step >= 0)
    {
        ix = int(x + 1 + width ) % width;
        iy = int(y + 1 + height) % height;


        noise_color += noise[iy * width + ix];

        ix = (int(x + width)) % width;
        iy = (int(y + height)) % height;

        assert(ix >= 0 && iy >= 0);

        next_x = x + dx[iy * width + ix];
        next_y = y + dy[iy * width + ix];

        x = next_x;
        y = next_y;

        step --;
    }

    return noise_color;
}


//line integral converlution
void generate_lic(uchar* noise, uchar* output, float *dx, float *dy, int width, int height)
{
    int y, x;

    for (x = 0; x < width; x ++)
    {
        for (y = 0; y < height; y++)
        {
            int sum_val = trace_pixel(noise, dx, dy, x, y, width, height, 10);

            output[y*width+x] = sum_val/10;
        }
    }
}


int main(int argc, char **argv)
{
    uchar noise[WIDTH * HEIGHT];
    uchar output[WIDTH * HEIGHT];

    float dx[WIDTH * HEIGHT];
    float dy[WIDTH * HEIGHT];

    cv::Mat noiseImg(HEIGHT, WIDTH, CV_8UC1, noise);
    cv::Mat outImg(HEIGHT, WIDTH, CV_8UC1, output);

    generate_noise(noise, WIDTH, HEIGHT);

    //SHOW_IMAGE(noiseImg);

    //generate_center_radiate_field(dx, dy, WIDTH, HEIGHT);
    generate_circle_field(dx, dy, WIDTH, HEIGHT);

    generate_lic(noise, output, dx, dy, WIDTH, HEIGHT);

    SHOW_IMAGE(outImg);

    return 0;
}
