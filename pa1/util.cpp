#include <iostream>
#include <cstring>
#include <assert.h>
#include "util.h"
#include "pngwriter.h"

using namespace std;

//range: [0-1]
void MdbToHSB(int v, int max, Color& hsb)
{
  hsb.r = v / ((double) max);
  hsb.g = 0.6;
  hsb.b = 1.0;
}

void WriteImage(int** image, int maxValue, int dimX, int dimY, char *filename)
{
  char* ftype = strcasestr(filename, ".png" );
  if(ftype == NULL || ftype != filename + strlen(filename) - 4)
      strcat(filename, ".png");
  cout << "writing image to " << filename << endl;
  
  Color hsb;
  pngwriter png(dimX, dimY, 0, filename);
  for(int x=0; x <dimX; ++x)
    for(int y=0; y<dimY; ++y) {
      MdbToHSB(image[y][x], maxValue, hsb);
      png.plotHSV(x, y, hsb.r, hsb.g, hsb.b);
    }
  png.close();
}

// range for h: [-180 : 180]
void HSBToRGB(const Color &hsb, ColorB &rgb)
{
  float h = hsb.r / 60.f;
  float s = hsb.g;
  float b = hsb.b;
  float maxRGB = b;
  if( s == 0.f) {
    rgb.r = rgb.g = rgb.b = 0.0f;
  } else {
    float delta = s * maxRGB;
    if(h > 3.f) {
      rgb.b = maxRGB * 255;
      if (h > 4.f) {
	rgb.g = (maxRGB - delta) * 255;
	rgb.r = ((h - 4) * delta + rgb.g) * 255;	
      }
      else {
	rgb.r = (maxRGB - delta)*255;
	rgb.g = (rgb.r - (h - 4.f) * delta) * 255;
      }
    }
    else {
      if( h > 1.f) {
	rgb.g = maxRGB * 255;
	if ( h > 2.f) {
	  rgb.r = (maxRGB - delta) * 255;
	  rgb.b = ((h-2.f) * delta + rgb.r) * 255;
	}
	else {
	  rgb.b = maxRGB - delta;
	  rgb.r = (rgb.b - (h -2.f) * delta) * 255;
	}
      }
      if ( h > -1.f) {
	rgb.r = maxRGB * 255;
	if( h > 0.f ) {
	  rgb.b = (maxRGB - delta) * 255;
	  rgb.g = ((h * delta) + rgb.b) * 255;
	}
	else {
	  rgb.g = (maxRGB - delta) * 255;
	  rgb.b = (rgb.g - h * delta) * 255;
	}
      }
    }
  }
}

void ConvertMdbToRGB(int **image, int max, int dimX, int dimY, ColorB** rgb)
{
  for(int y=0; y < dimY; ++y)
    for(int x=0; x < dimX; ++x) {
      Color hsb;
      MdbToHSB(image[y][x], max, hsb);
      hsb.r = (hsb.r * 360) - 180;
      HSBToRGB(hsb, rgb[y][x]);
      // greyscale
      //rgb[y][x].r = (unsigned char)((image[y][x] / double(max)) * 255);
      //rgb[y][x].g = (unsigned char)((image[y][x] / double(max)) * 255);
      //rgb[y][x].b = (unsigned char)((image[y][x] / double(max)) * 255);
    }
}
// Allocate a 2D array
int **alloc2D(int m,int n){

   int **E;
   int nx=n+1, ny=m+1;
   E = (int**)malloc(sizeof(int*)*ny + sizeof(int)*nx*ny);
   assert(E);
   int j;
   for(j=0;j<ny;j++) E[j] = (int*)(E+ny) + j*nx;
   return(E);
}

