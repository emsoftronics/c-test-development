 #include <stdlib.h>
 #include <unistd.h>
 #include <stdio.h>
 #include <fcntl.h>
 #include <linux/fb.h>
 #include <sys/mman.h>
 #include <sys/ioctl.h>
 #include "rubberball.h"
#define PITCH 10

     struct fb_var_screeninfo vinfo;
     struct fb_fix_screeninfo finfo;
     long int screensize = 0;
     char *fbp = 0;


void get_point(int *xof, int *yof)
{
    static int cwall = 0;
    static int px = 670;
    static int py = 100;
    static int cx = 670 + PITCH;
    static int cy = 100 + PITCH;

    if ((cx >= vinfo.xres - width/2) && (cy >= vinfo.yres - height/2)) {
        *xof = vinfo.xres - width/2  - PITCH;
        *yof = vinfo.yres - height/2 - PITCH;
        cwall = 1;
    }
    else if ((cx >= vinfo.xres - width/2) && (cy <= height/2)) {
        *xof = vinfo.xres - width/2  - PITCH;
        *yof = height/2 + PITCH;
        cwall = 1;
    }
    if ((cx <= width/2) && (cy >= vinfo.yres - height/2)) {
        *xof = width/2  + PITCH;
        *yof = vinfo.yres - height/2 - PITCH;
        cwall = 3;
    }
    if ((cx <= width/2) && (cy <= height/2)) {
        *xof = width/2  + PITCH;
        *yof = height/2 + PITCH;
        cwall = 3;
    }
    else if (cx >= vinfo.xres - width/2) {
        *xof = px;
        *yof = 2*cy - py;
        cwall = 1;
    }
    else if(cy >= vinfo.yres - height/2) {
        *yof = py;
        *xof = 2*cx - px;
        cwall = 2;
    }
    else if(cx <= width/2) {
        *xof = px;
        *yof = 2*cy - py;
        cwall = 3;
    }
    else if(cy <= height/2) {
        *yof = py;
        *xof = 2*cx - px;
        cwall = 0;
    } else {
        switch(cwall){
            case 0:
                *yof = cy + PITCH;
                *xof = ((px - cx)*(*yof)/(py - cy)) - ((px - cx)*(cy)/(py - cy)) + cx;
                break;
            case 1:
                *xof = cx - PITCH;
                *yof = ((py - cy)*(*xof)/(px - cx)) - ((py - cy)*(cx)/(px - cx)) + cy;
                break;
            case 2:
                *yof = cy - PITCH;
                *xof = ((px - cx)*(*yof)/(py - cy)) - ((px - cx)*(cy)/(py - cy)) + cx;
                break;
            case 3:
                *xof = cx + PITCH;
                *yof = ((py - cy)*(*xof)/(px - cx)) - ((py - cy)*(cx)/(px - cx)) + cy;
                break;
        }

   }
   if (*xof > vinfo.xres - width/2) *xof = vinfo.xres - width/2;
   if (*yof > vinfo.yres - height/2) *yof = vinfo.yres - height/2;
   px = cx;
   py = cy;
   cx = *xof;
   cy = *yof;
}

void draw_picture(char *pic, int left, int top)
{
     int x = 0, y = 0;
     long int location = 0;
     // Figure out where in memory to put the pixel
     char pixel[3];
     for (y = top; y < height + top; y++) {
         for (x = left; x < width + left; x++) {

             location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                        (y+vinfo.yoffset) * finfo.line_length;
              HEADER_PIXEL(pic, pixel);
             if (vinfo.bits_per_pixel == 32) {
                 *(fbp + location) = pixel[2];        // Some blue
                 *(fbp + location + 1) = pixel[1];//25+(x-100)/2;     // A little green
                 *(fbp + location + 2) = pixel[0];//20-(y-100)/5;    // A lot of red
                 *(fbp + location + 3) = 0;      // No transparency
             } else  { //assume 16bpp
                 int b = pixel[2];//10;
                 int g = pixel[1];//(x-100)/6;     // A little green
                 int r = pixel[0];//31-(y-100)/16;    // A lot of red
                 unsigned short int t = r<<11 | g << 5 | b;
                 *((unsigned short int*)(fbp + location)) = t;
             }

         }
     }
}

void draw_var_picture(char *pic, int left, int top)
{
     int x = 0, y = 0;
     long int location = 0;
     static int i = 0;
     static int j = 1;
     static int k = 2;
     static int count = 0;
     // Figure out where in memory to put the pixel

     if (count > 15) {
         count = 0;
         i++; j++; k++;
         i %= 3; j %= 3; k %= 3;
     }

     char pixel[3];
     for (y = top; y < height + top; y++) {
         for (x = left; x < width + left; x++) {

             location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                        (y+vinfo.yoffset) * finfo.line_length;
              HEADER_PIXEL(pic, pixel);
             if (vinfo.bits_per_pixel == 32) {
                 *(fbp + location) = pixel[i];        // Some blue
                 *(fbp + location + 1) = pixel[j];//25+(x-100)/2;     // A little green
                 *(fbp + location + 2) = pixel[k];//20-(y-100)/5;    // A lot of red
                 *(fbp + location + 3) = 0;      // No transparency
             } else  { //assume 16bpp
                 int b = pixel[i];//10;
                 int g = pixel[j];//(x-100)/6;     // A little green
                 int r = pixel[k];//31-(y-100)/16;    // A lot of red
                 unsigned short int t = r<<11 | g << 5 | b;
                 *((unsigned short int*)(fbp + location)) = t;
             }

         }
     }
     count++;
}

void clear_picture(int left, int top)
{
     int x = 0, y = 0;
     long int location = 0;
     for (y = top; y < height + top; y++) {
         for (x = left; x < width + left; x++) {

             location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                        (y+vinfo.yoffset) * finfo.line_length;
             if (vinfo.bits_per_pixel == 32) {
                 *(fbp + location) = 0;        // Some blue
                 *(fbp + location + 1) = 0;//25+(x-100)/2;     // A little green
                 *(fbp + location + 2) = 0;//20-(y-100)/5;    // A lot of red
                 *(fbp + location + 3) = 0;      // No transparency
             } else  { //assume 16bpp
                 int b = 0;//10;
                 int g = 0;//(x-100)/6;     // A little green
                 int r = 0;//31-(y-100)/16;    // A lot of red
                 unsigned short int t = r<<11 | g << 5 | b;
                 *((unsigned short int*)(fbp + location)) = t;
             }

         }
     }
}

 int main()
 {
     int fbfd = 0;
     int x = 0, y = 0;

     // Open the file for reading and writing
     fbfd = open("/dev/fb0", O_RDWR);
     if (fbfd == -1) {
         perror("Error: cannot open framebuffer device");
         exit(1);
     }
     printf("The framebuffer device was opened successfully.\n");

     // Get fixed screen information
     if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
         perror("Error reading fixed information");
         exit(2);
     }

     // Get variable screen information
     if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
         perror("Error reading variable information");
         exit(3);
     }

     vinfo.bits_per_pixel = 32;
     vinfo.blue.offset = 0;
     vinfo.blue.length = 8;
     vinfo.green.offset = 8;
     vinfo.green.length = 8;
     vinfo.red.offset = 16;
     vinfo.red.length = 8;
     vinfo.transp.offset = 24;
     vinfo.transp.length = 8;

     // Get variable screen information
     if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
         perror("Error reading variable information");
         exit(3);
     }


     printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
     printf("No. pages : %d\n",(finfo.smem_len/finfo.line_length)/vinfo.yres);

     // Figure out the size of the screen in bytes
     screensize = vinfo.yres_virtual * finfo.line_length ;

     // Map the device to memory
     fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                        fbfd, 0);
     if ((int)fbp == -1) {
         perror("Error: failed to map framebuffer device to memory");
         exit(4);
     }
     printf("The framebuffer device was mapped to memory successfully.\n");

     x = 100; y = 100;       // Where we are going to put the pixel
    while(1) {
        clear_picture(x, y);
        get_point(&x,&y);
        x -= width/2;
        y -= height/2;
 //       printf ("(%d,%d)", x, y);
//        memset(fbp, 0, screensize);
        draw_var_picture(header_data, x, y);
        usleep(100000);
    }
     munmap(fbp, screensize);
     close(fbfd);
     return 0;
 }
