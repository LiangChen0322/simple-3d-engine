
#ifndef SCREEN_H
#define SCREEN_H

#include <gtkmm.h>
#include "Camera.h"
#include "Mesh.h"

namespace g3
{
/**
 * Implements the graphics pipeline.
 */
class World: public Gtk::DrawingArea {

  public:
  World(unsigned int w, unsigned int h);
  virtual ~World(){};

  bool on_idle();

  virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
  virtual bool on_scroll_event(GdkEventScroll* event);

  private:

  void clear();
  void render();

  void renderWireframe(const Mat4& viewProjMat);
  void renderAxesAndGrid(const Mat4& viewProjMat);

  int mapXToWin(float x);
  int mapYToWin(float y);

  void drawPoint(int x, int y, float z, unsigned long color);
  void drawLine(int x0, int y0, float z0, int x1, int y1, float z1, unsigned long colour0, unsigned long colour1);
  void handleGourandShaping(int px0, int py0, float pz0, unsigned long color0,
                                     int px1, int py1, float pz1, unsigned long color1,
                                     int px2, int py2, float pz2, unsigned long color2);

  unsigned long clock_time();

  /* The width of the screen. */
  unsigned int width;
  unsigned int height;

  /* Front buffer. */
  Glib::RefPtr<Gdk::Pixbuf> frontBuffer;

  /* Depth buffer */
  std::unique_ptr<float[]> depthBuffer;

  /**
   * The target frame time in nanoseconds.
   *
   * 30 FPS = 33.3 milliseconds/frame == 33300000 nanoseconds/frame.
   */
  unsigned long targetFrameTime;
  unsigned long startFrameTime;
  unsigned long finishFrameTime;

  Camera camera;
  TriangleMesh cube;
};

/**
 * Creates an RGBA color as a long.
 */
unsigned long createRGBA(int r, int g, int b, int a);


}

#endif
