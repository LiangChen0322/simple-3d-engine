
#include "World.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "Mat.h"
#include "Quaternion.h"
#include "Mesh.h"
#include <memory>
#include <limits>

g3::World::World(unsigned int w, unsigned int h):
width {w},
height {h},
frontBuffer {Gdk::Pixbuf::create(Gdk::Colorspace::COLORSPACE_RGB, true, 8, width, height)},
targetFrameTime {33300000},
camera { Vec3{15, 15, 15}, Vec3{-1, -1, -1}, 1280 }//camera { Vec3{17, 10, -20}, Vec3{1, 0, 2}, 1280 }
{
  // initializes the depth buffer.
  depthBuffer.reset(new float[width * height]);

  // start frame time
  startFrameTime = clock_time();

  g3::loadCube(cube);

  // enable mouse wheel detection
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK);

  // register idle function
  Glib::signal_idle().connect(sigc::mem_fun(*this, &World::on_idle));
}

inline unsigned long g3::createRGBA(int r, int g, int b, int a)
{
  return ((r & 0xff) << 24) 
    + ((g & 0xff) << 16) 
    + ((b & 0xff) << 8) 
    + (a & 0xff);
}

static inline void getRGBA(unsigned long color, int *r, int *g, int *b, int *a)
{
  *r = (color >> 24) & 0xff; // red
  *g = (color >> 16) & 0xff; // blue
  *b = (color >>  8) & 0xff; // grenn
  *a = color & 0xff;
}

/**
 * Clears the buffers.
 */
void g3::World::clear()
{
  // Fill the buffer with color white
  frontBuffer->fill(0xfafad2ff);
  // Clears the depth buffer
  std::fill(depthBuffer.get(), depthBuffer.get() + (width*height), std::numeric_limits<float>::infinity());
}

/**
 * Detects mouse wheel movements. Called by the GUI.
 */
bool g3::World::on_scroll_event(GdkEventScroll* event)
{
  float zoomFactorPercent = 0.05;

  if (event->direction == GdkScrollDirection::GDK_SCROLL_UP) {
    camera.zoomFactor += camera.zoomFactor * zoomFactorPercent;
  } else {
    camera.zoomFactor -= camera.zoomFactor * zoomFactorPercent;
  }

  return true;
}

/**
 * The drawing function, called by the GUI.
 */
bool g3::World::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  render();

  // Draw the buffer
  Gdk::Cairo::set_source_pixbuf(cr, frontBuffer);
  cr->paint();

  return true;
}

/**
 * Renders the scene.
 */
void g3::World::render()
{
  clear();

  Vec3 upWorld {0,1,0};

  Mat4 viewProjMatrix = g3::createLookAtLHMatrix(camera.eye, camera.target, upWorld)
          * g3::createPerspectiveFovLHMatrix(0.78f, width / (float)height, 0.01f, 25.0f);

  renderAxesAndGrid(viewProjMatrix);	
  renderWireframe(viewProjMatrix);
}

/**
 * Renders the cube wireframe.
 */
void g3::World::renderWireframe(const g3::Mat4& viewProjMatrix)
{
  Mat4 transformMatrix = g3::getWorldMatrix(cube) * viewProjMatrix;

  for (unsigned int i = 0; i < cube.nFaces; i++) {
    Vec3 v[3];
    int mapToWin[6];
    unsigned long color[3];

    for (unsigned int j = 0; j < 3; j++) {
      v[j] = transformP3( cube.vertices[ cube.faces[i].vertexIndex[j] ].pos, transformMatrix );

      mapToWin[2*j]   = mapXToWin( v[j][0] );
      mapToWin[2*j+1] = mapYToWin( v[j][1] );

      ColorRGBA tc = cube.vertices[ cube.faces[i].vertexIndex[j] ].color;
      color[j] = createRGBA(tc.r, tc.g, tc.b, tc.a);
    }

    drawLine(mapToWin[0], mapToWin[1], v[0][2], mapToWin[2], mapToWin[3], v[1][2], color[0], color[1]);
    drawLine(mapToWin[2], mapToWin[3], v[1][2], mapToWin[4], mapToWin[5], v[2][2], color[1], color[2]);
    drawLine(mapToWin[4], mapToWin[5], v[2][2], mapToWin[0], mapToWin[1], v[0][2], color[2], color[0]);
    handleGourandShaping(mapToWin[0], mapToWin[1], v[0][2],  color[0],
                         mapToWin[2], mapToWin[3], v[1][2],  color[1],
                         mapToWin[4], mapToWin[5], v[2][2],  color[2]);
  }
}

void g3::World::renderAxesAndGrid(const g3::Mat4& viewProjMat)
{
  Mat4 staticMatrix = createScaleMatrix(1) * viewProjMat;

  // render axes
  Vec3 origo {0, 0, 0};
  origo = transformP3( origo, staticMatrix );

  int origoX = mapXToWin( origo[0] );
  int origoY = mapYToWin( origo[1] );

  Vec3 axes[] { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };
  unsigned long axesColor[] {
    createRGBA(255, 0, 0, 255),
    createRGBA(0, 255, 0, 255),
    createRGBA(0, 0, 255, 255)
  };

  for (int k = 0; k < 3; k++) {
    Vec3 axisEnd = transformP3( axes[k], staticMatrix );
    int endX = mapXToWin( axisEnd[0] );
    int endY = mapYToWin( axisEnd[1] );
    drawLine(origoX, origoY, origo[2], endX, endY, axisEnd[2], axesColor[k], axesColor[k]);
  }

  // render grid ground
  float step = 1;
  int size = 8; // size X size
  Vec3 grid [ 4*(size+1) ];

  Vec3 startX {  size/2.0f*step, 0, size/2.0f*step  };
  for (int n = 0, m = 0; n < (size+1); n++, m+=4) {
    grid[m]   = { startX[0],               0, startX[2] - (n*step) };	
    grid[m+1] = { startX[0] - (size*step), 0, startX[2] - (n*step) };	
    grid[m+2] = { startX[0] - (n*step),    0, startX[2] };	
    grid[m+3] = { startX[0] - (n*step),    0, startX[2] - (size*step) };	
  }

	unsigned long gridColor = createRGBA(205, 201, 201, 255);
	for (int n = 0; n < 4*(size+1); n+=2) {
		Vec3 g1 = transformP3( grid[n], staticMatrix );
		int g1X = mapXToWin( g1[0] );
		int g1Y = mapYToWin( g1[1] );
		Vec3 g2 = transformP3( grid[n+1], staticMatrix );
		int g2X = mapXToWin( g2[0] );
		int g2Y = mapYToWin( g2[1] );
		drawLine(g1X, g1Y, g1[2], g2X, g2Y, g2[2], gridColor, gridColor);
	}

}

inline int g3::World::mapXToWin(float x)
{
	return ( x * camera.zoomFactor / (width/(float)height)  ) + (width / 2.0f);
}

inline int g3::World::mapYToWin(float y)
{
	return ( -y * camera.zoomFactor ) + (height / 2.0f);
}

void g3::World::drawLine(int x0, int y0, float z0, int x1, int y1, float z1, unsigned long color0, unsigned long color1)
{
  if ( (std::min(std::abs(x0), std::abs(x1)) > width) &&
      (std::min(std::abs(y0), std::abs(y1)) > height) ) {
    return;
  }

  float x = (float)x0;
  float y = (float)y0;
  float z = (float)z0;
  int steps = std::abs(x1 - x0) > std::abs(y1 - y0) ? std::abs(x1 - x0) : std::abs(y1 - y0);

  float dx = (float)(x1 - x0) / steps;
  float dy = (float)(y1 - y0) / steps;
  float dz = (z1 - z0) / steps;

  int rgbi[2][4];
  float rgb[2][3];
  float drgb[3];

  getRGBA(color0, rgbi[0], rgbi[0] + 1, rgbi[0] + 2, rgbi[0] + 3);
  getRGBA(color1, rgbi[1], rgbi[1] + 1, rgbi[1] + 2, rgbi[1] + 3);

  for (int i = 0; i < 3; i++) {
    rgb[0][i] = (float)rgbi[0][i];
    rgb[1][i] = (float)rgbi[1][i];
    drgb[i] = (rgb[1][i] - rgb[0][i]) / steps;
  }

  unsigned long color = color0;

  do {
    drawPoint((int)x, (int)y, (int)z, color);
    if ((int)x != x1)
      x += dx;
    if((int)y != y1)
      y += dy;
    for (int i = 0; i < 3; i++) {
      rgb[0][i] += drgb[i];
    }
    color = createRGBA((int)rgb[0][0], (int)rgb[0][1], (int)rgb[0][2], 255);

    if (((int)x == x1) && ((int)y == y1))
      break;
  } while (true);
}


/**
 * Draws a point on the screen.
 */
void g3::World::drawPoint(int x, int y, float z, unsigned long color)
{
  if ((x >= 0) && (y >=0) && (x < width) && (y < height)) {
    // depth test
    int targetPixel = y * width + x;
    if ( z < depthBuffer[targetPixel] ) {
      // saves the new depth value
      depthBuffer[targetPixel] = z;

      // sets the color of the pixel
      int offset  = y * frontBuffer->get_rowstride() + x * frontBuffer->get_n_channels();
      guchar* pixel = &frontBuffer->get_pixels()[ offset ];

      pixel[0] = (color >> 24) & 0xff; // red
      pixel[1] = (color >> 16) & 0xff; // blue
      pixel[2] = (color >>  8) & 0xff; // grenn

      // alpha ignored
      // pixel[3] =  color & 0xff;         // alpha
    }
  }
}

/**
 * Returns a time point in nanoseconds.
 *
 * @return Time point in nanoseconds.
 */
unsigned long g3::World::clock_time()
{
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return (ts.tv_sec * 1000000000) + ts.tv_nsec;
}

bool g3::World::on_idle()
{
  finishFrameTime = clock_time();
  unsigned long timeSpentInFrame = finishFrameTime - startFrameTime;

  if (timeSpentInFrame <= targetFrameTime) {
    // sleep
    std::chrono::duration<float, std::nano> wait { targetFrameTime - timeSpentInFrame };
    std::this_thread::sleep_for( wait );

    // redraw only if we were too fast in the prevoius frame.
    queue_draw();
  } else {
    // Here we were too slow so we will skip the next redrawing.
    //std::cout << "Frame dropping: " << timeSpentInFrame << std::endl;
  }

  // start new frame
  startFrameTime = clock_time();

  // Elapsed time calculation between two frames goes here
  // ...

  // Rotates the cube around the y axis in radians.
  // cube.rotationX += 0.01;
  // cube.rotationY += 0.01;
  // cube.rotationZ += 0.01;

  return true;
}

#define SWAP(a, b)  { a = a + b; b = a - b; a = a - b; }

void g3::World::handleGourandShaping(int px0, int py0, float pz0, unsigned long color0,
                                     int px1, int py1, float pz1, unsigned long color1,
                                     int px2, int py2, float pz2, unsigned long color2)
{
  float v[3][3];       // triangle vertex
  unsigned long color[3] = {color0, color1, color2};
  int v0 = 0, v1 = 1, v2 = 2;

  float sx0, ex0, dx0;
  float sx1, ex1, dx1;
  float sz0, ez0, dz0;
  float sz1, ez1, dz1;
  int sy, ey0, ey1;

  float rgb[3][3];
  float sr0, er0, dr0, sg0, eg0, dg0, sb0, eb0, db0;
  float sr1, er1, dr1, sg1, eg1, dg1, sb1, eb1, db1;

  v[0][0] = (float)px0;
  v[0][1] = (float)py0;
  v[0][2] = pz0;
  v[1][0] = (float)px1;
  v[1][1] = (float)py1;
  v[0][2] = pz1;
  v[2][0] = (float)px2;
  v[2][1] = (float)py2;
  v[0][2] = pz2;

  for (int i = 0; i < 3; i++) {
    rgb[i][0] = (float)((color[i] >> 24) & 0xff); // red
    rgb[i][1] = (float)((color[i] >> 16) & 0xff); // blue
    rgb[i][2] = (float)((color[i] >>  8) & 0xff); // green
  }

  /* sort by y position */
  if (v[v0][1] > v[v1][1]) {
    SWAP(v0, v1);
  }
  if (v[v0][1] > v[v2][1]) {
    SWAP(v0, v2);
  }
  if (v[v1][0] > v[v2][0]) {
    SWAP(v1, v2);
  }

  if (v[v0][1] == v[v1][1]) {
    /*
     *      v2
     *     /  \
     *    /    \
     *   /      \
     *  v0 ---- v1
     */
    sx0 = v[v0][0];
    sx1 = v[v1][0];
    ex0 = ex1 = v[v2][0];
    sy = v[v0][1];
    ey0 = ey1 = v[v2][1];

    sz0 = v[v0][2];
    sz1 = v[v1][2];
    ez0 = ez1 = v[v2][2];

    sr0 = rgb[v0][0];
    sg0 = rgb[v0][1];
    sb0 = rgb[v0][2];
    sr1 = rgb[v1][0];
    sg1 = rgb[v1][1];
    sb1 = rgb[v1][2];
    er0 = er1 = rgb[v2][0];
    eg0 = eg1 = rgb[v2][1];
    eb0 = eb1 = rgb[v2][2];
  } else {
    /*
     * v1 ------ v2
     *   \      /
     *    \    /
     *     \  /
     *      v0
     */
    sx0 = sx1 = v[v0][0];
    ex0 = v[v1][0];
    ex1 = v[v2][0];
    sy = v[v0][1];
    ey0 = v[v1][1];
    ey1 = v[v2][1];

    sz0 = sz1 = v[v0][2];
    ez0 = v[v1][2];
    ez1 = v[v2][2];

    sr0 = sr1 = rgb[v0][0];
    sg0 = sg1 = rgb[v0][1];
    sb0 = sb1 = rgb[v0][2];
    er0 = rgb[v1][0];
    eg0 = rgb[v1][1];
    eb0 = rgb[v1][2];
    er1 = rgb[v2][0];
    eg1 = rgb[v2][1];
    eb1 = rgb[v2][2];
  }
  //  else {
  //   sx0 = sx1 = v[v0][0];
  //   ex0 = v[v1][0];
  //   ex1 = v[v2][0];
  //   sy = v[v0][1];
  //   ey0 = v[v1][1];   // different with case 2
  //   ey1 = v[v2][1];
  //   sz0 = sz1 = v[v0][2];
  //   ez0 = v[v1][2];
  //   ez1 = v[v2][2];
  // }

  int steps0 = ey0 - sy;
  int steps1 = ey1 - sy;
  dx0 = (float)(ex0 - sx0) / steps0;
  dx1 = (float)(ex1 - sx1) / steps1;
  dz0 = (ez0 - sz0) / steps0;
  dz1 = (ez1 - sz1) / steps1;
  float tsx = sx0;
  float tex = sx1;
  float tsz = sz0;
  float tez = sz1;

  dr0 = (er0 - sr0) / steps0;
  dg0 = (eg0 - sg0) / steps0;
  db0 = (eb0 - sb0) / steps0;
  dr1 = (er1 - sr1) / steps1;
  dg1 = (eg1 - sg1) / steps1;
  db1 = (eb1 - sb1) / steps1;

  for (int y = sy; y < ey0; y++) {
    float z = tsz;
    float tdz;
    float tdr, tdg, tdb;
    int r = sr0, g = sg0, b = sb0;
    int tdx;
    if (tex > tsx) {
      tdx = 1;
      tdz = (tez - tsz) / (tex - tsx);
      tdr = (sr1 - sr0) / (tex - tsx);
      tdg = (sg1 - sg0) / (tex - tsx);
      tdb = (sb1 - sb0) / (tex - tsx);
    } else if (tex == tsx) {
      tdx = tdz = tdr = tdg = tdb = 0.0;
    } else {
      tdx = -1;
      tdz = (tez - tsz) / (tsx - tex);
      tdr = (sr1 - sr0) / (tsx - tex);
      tdg = (sg1 - sg0) / (tsx - tex);
      tdb = (sb1 - sb0) / (tsx - tex);
    }
    for (int x = (int)tsx; x != (int)tex; ) {
      unsigned long color = createRGBA(r,g,b,255);
      drawPoint(x, y, z, color);
      x += tdx;
      z += tdz;
      r += tdr;
      g += tdg;
      b += tdb;
    }
    tsx += dx0;
    tex += dx1;
    tsz += dz0;
    tez += dz1;
    sr0 += dr0;
    sg0 += dg0;
    sb0 += db0;
    sr1 += dr1;
    sg1 += dg1;
    sb1 += db1;
  }
}
