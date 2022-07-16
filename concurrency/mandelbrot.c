#include "thread.h"
#include <math.h>

int NT;
#define W 12800
#define H 12800
#define IMG_FILE "./mandelbrot.ppm"

static inline int belongs(int x, int y, int t) {
  return x / (W / NT) == t;
}

int x[W][H];
int volatile done = 0;

void display(FILE *fp, int step) { 
  static int rnd = 1;
  int w = W / step, h = H / step;
  // STFW: Portable Pixel Map
  fprintf(fp, "P6\n %d %d 255\n", w, h);
  for (int j = 0; j < H; j += step) {
    for (int i = 0; i < W; i += step) {
      int n = x[i][j];
      int r = 255 * pow((n - 80) / 800.0, 3);
      int g = 255 * pow((n - 80) / 800.0, 0.7);
      int b = 255 * pow((n - 80) / 800.0, 0.5);
      fputc(r, fp); fputc(g, fp); fputc(b, fp);
    }
  }
}

void Tworker(int tid) {
  for (int i = 0; i < W; i++)
    for (int j = 0; j < H; j++)
      if (belongs(i, j, tid - 1)) {
        double a = 0, b = 0, c, d;
        while ((c = a * a) + (d = b * b) < 4 && x[i][j]++ < 880) {
          b = 2 * a * b + j * 1024.0 / H * 8e-9 - 0.645411;
          a = c - d + i * 1024.0 / W * 8e-9 + 0.356888;
        }
      }
  done++;
}

void Tdisplay() {
  float ms = 0;
  while (1) {
    FILE *fp = popen("viu -", "w"); assert(fp);
    display(fp, W / 256);
    pclose(fp);
    if (done == NT) break;
    usleep(1000000 / 5);
    ms += 1000.0 / 5;
  }
  printf("Approximate render time: %.1lfs\n", ms / 1000);

  FILE *fp = fopen(IMG_FILE, "w"); assert(fp);
  display(fp, 2);
  fclose(fp);
}

int main(int argc, char *argv[]) {
  assert(argc == 2);
  NT = atoi(argv[1]);
  for (int i = 0; i < NT; i++) {
    create(Tworker);
  }
  create(Tdisplay);
  join();
  return 0;
}
