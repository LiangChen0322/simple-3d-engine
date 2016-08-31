
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
bool g3::World::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
	
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

    for (unsigned int j = 0; j < 3; j++) {
      v[j] = transformP3( cube.vertices[ cube.faces[i].vertexIndex[j] ].pos, transformMatrix );

      mapToWin[2*j]   = mapXToWin( v[j][0] );
      mapToWin[2*j+1] = mapYToWin( v[j][1] );
    }

    unsigned long color = createRGBA(0, 0, 128, 255);
    drawLine(mapToWin[0], mapToWin[1], v[0][2], mapToWin[2], mapToWin[3], v[1][2], color);
    drawLine(mapToWin[2], mapToWin[3], v[1][2], mapToWin[4], mapToWin[5], v[2][2], color);
    drawLine(mapToWin[4], mapToWin[5], v[2][2], mapToWin[0], mapToWin[1], v[0][2], color);
  }
}

/**
 * Renders the axes and the grid ground.
 */
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
    drawLine(origoX, origoY, origo[2], endX, endY, axisEnd[2], axesColor[k]);
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
		drawLine(g1X, g1Y, g1[2], g2X, g2Y, g2[2], gridColor);
	}

}

/**
 * Maps the x coordinate to the window coordinate system
 */
inline int g3::World::mapXToWin(float x)
{
	return ( x * camera.zoomFactor / (width/(float)height)  ) + (width / 2.0f);
}

/**
 * Maps the y coordinate to the window coordinate system
 */
inline int g3::World::mapYToWin(float y)
{
	return ( -y * camera.zoomFactor ) + (height / 2.0f);
}


/**
 * Creates an RGBA color as a long.
 */
inline unsigned long g3::createRGBA(int r, int g, int b, int a)
{
	return ((r & 0xff) << 24) 
		+ ((g & 0xff) << 16) 
		+ ((b & 0xff) << 8) 
		+ (a & 0xff);
}


/**
 * Draws a line.
 */
void g3::World::drawLine(int x0, int y0, float z0, int x1, int y1, float z1, unsigned long color)
{
  // The line visible?
  if ( (std::min(std::abs(x0), std::abs(x1)) > width) &&
      (std::min(std::abs(y0), std::abs(y1)) > height) ) {
    return;
  }

  int dx = std::abs(x1 - x0);
  int dy = std::abs(y1 - y0);
  float dz = std::abs(z1 - z0);
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;

  int err = dx - dy;
  float gradient = 0;

  int x = x0;
  int y = y0;
  int z = z0;

  while (true) {
    drawPoint(x, y, z, color);

    if ((x == x1) && (y == y1)) break;
    int e2 = 2 * err;
    if (e2 > -dy) { err -= dy; x += sx; }
    if (e2 < dx) { err += dx; y += sy; }

    // interpolate z depth values
    gradient = (dx > dy) ?  ((x-x0) / dx) : ((y-y0) / dy);
    z = z0 + (dz * gradient);
  }

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

/**
 * This idle callback function is executed as often as possible, 
 * hence it is ideal for processing intensive tasks.
 */
bool g3::World::on_idle()
{
  // finish current frame
  finishFrameTime = clock_time();

  // The amount of time that the frame spent in nanoseconds.
  unsigned long timeSpentInFrame = finishFrameTime - startFrameTime;

  if (timeSpentInFrame <= targetFrameTime)
  {
    // sleep
    std::chrono::duration<float, std::nano> wait { targetFrameTime - timeSpentInFrame };
    std::this_thread::sleep_for( wait );

    // redraw only if we were too fast in the prevoius frame.
    queue_draw();
  } else {
    // Here we were too slow so we will skip the next redrawing.
    std::cout << "Frame dropping: " << timeSpentInFrame << std::endl;
  }

  // start new frame
  startFrameTime = clock_time();

  // Elapsed time calculation between two frames goes here
  // ...

  // Rotates the cube around the y axis in radians.
  // cube.rotationX += 0.01;
  // cube.rotationY += 0.01;
  // cube.rotationZ += 0.001;

  return true;
}


#define SWAP(a, b)  { a = a + b; b = a - b; a = a - b; }

void g3::World::handleGourandShaping(int px0, int py0, unsigned long colour0,
                                     int px1, int py1, unsigned long colour1,
                                     int px2, int py2, unsigned long colour2)
{
  float rbg[3][3];
  float tv[3][2];       // triangle vertex
  unsigned long colour[3] = {colour0, colour1, colour2};
  float start_x, end_x, dsx, dex;

  /*
   * set the triangle to the shapes like following:
   * v1 ------ v2                 v0
   *   \      /     or           /  \
   *    \    /                  /    \
   *     \  /                  /      \
   *      v0                  v1 ---- v2
   */
  tv[0][0] = px0 * 1.0;
  tv[0][1] = py0 * 1.0;
  tv[1][0] = px1 * 1.0;
  tv[1][1] = py1 * 1.0;
  tv[2][0] = px2 * 1.0;
  tv[2][1] = py2 * 1.0;

  int v0 = 0, v1 = 1, v2 = 2;

  if (v[v0][1] < v[v1][1]) {
    SWAP(v0, v1);
  }
  if (v[v0][1] < v[v2][1]) {
    SWAP(v0, v2);
  }
  if (v[v1][0] > v[v2][0]) {
    SWAP(v1, v2);
  }

  for (int i = 0; i < 3; i++) {
    rgb[i][0] = (color[i] >> 24) & 0xff; // red
    rgb[i][1] = (color[i] >> 16) & 0xff; // blue
    rgb[i][2] = (color[i] >>  8) & 0xff; // green
  }
  int tmp_y = std::max(v[v1][1], v[v2][1]);
  int dy = v[v0][1] - tmp_y;
  for (int y = v[v0][1]; y > tmp_y; y--) {
    start_x = end_x = v[v0][0];
    if (v[v1][0] > v[v2][0]) {
      dex = (v[v1][0] - v[v0][0]) * 1.0 / dy;
      dsx = (v[v2][0] - v[v0][0]) * 1.0 / dy;
    } else {
      dsx = (v[v1][0] - v[v0][0]) * 1.0 / dy;
      dex = (v[v2][0] - v[v0][0]) * 1.0 / dy;
    }
    for (int x = start_x; x <= end_x; x++) {
      unsigned long color = createRGBA(0, 0, 128, 255);
      drawPoint();
    }
  }

  if (tmp_y != v[v1][1]) {
    for ()
  }

  if (v[0][1] > v[1][1]) {
    int dy = v[0][1] - v[1][1];
    int sr = rgb[0][0], sg = rgb[0][1], sb = rgb[0][2];
    int er = rgb[0][0], eg = rgb[0][1], eb = rgb[0][2];

    double dr1 = (rgb[1][0] - rgb[0][0]) * 1.0 / dy;
    double db1 = (rgb[1][2] - rgb[0][2]) * 1.0 / dy;
    double dg1 = (rgb[1][1] - rgb[0][1]) * 1.0 / dy;

    int dr2 = (rgb[2][0] - rgb[0][0]) * 1.0 / dy;
    int dg2 = (rgb[2][0] - rgb[0][1]) * 1.0 / dy;
    int db2 = (rgb[2][0] - rgb[0][2]) * 1.0 / dy;

    for (int y = v[1][1]; y < v[0][1]; y++) {
      int dx1 = (v[1][0] - v[0][0]) * 1.0 / dy;
      int tx1 = 
      for (int x = )
    }
  }
}

// void g3::World::handleGourandShaping(int px0, int py0, unsigned long colour0,
//                                      int px1, int py1, unsigned long colour1,
//                                      int px2, int py2, unsigned long colour2)
// {
//   unsigned int rgb[3][3];
//   int v[3][2] = {{px0, py0}, {px1, py1}, {px2, py2}};
//   unsigned long colour[3] = {colour0, colour1, colour2};

//   /*
//    * set the triangle to the shapes like following:
//    * v1 ------ v2                 v0
//    *   \      /     or           /  \
//    *    \    /                  /    \
//    *     \  /                  /      \
//    *      v0                  v1 ----- v2
//    */
//   if (v[0][1] == v[1][1]) {
//     SWAP(v[0][0], v[2][0]);
//     SWAP(v[0][1], v[2][1]);
//     SWAP(colour[0], colour[2]);
//   } else if (v[0][1] == v[2][1]) {
//     SWAP(v[0][0], v[1][0]);
//     SWAP(v[0][1], v[1][1]);
//     SWAP(colour[0], colour[1]);
//   }
//   if (v[1][0] > v[2][0]) {
//     SWAP(v[1][0], v[2][0]);
//     SWAP(v[1][1], v[2][1]);
//     SWAP(colour[1], colour[2]);
//   }

//   for (int i = 0; i < 3; i++) {
//     rgb[i][0] = (color[i] >> 24) & 0xff; // red
//     rgb[i][1] = (color[i] >> 16) & 0xff; // blue
//     rgb[i][2] = (color[i] >>  8) & 0xff; // grenn
//   }

//   if (v[0][1] > v[1][1]) {
//     int dy = v[0][1] - v[1][1];
//     int sr = rgb[0][0], sg = rgb[0][1], sb = rgb[0][2];
//     int er = rgb[0][0], eg = rgb[0][1], eb = rgb[0][2];

//     double dr1 = (rgb[1][0] - rgb[0][0]) * 1.0 / dy;
//     double db1 = (rgb[1][2] - rgb[0][2]) * 1.0 / dy;
//     double dg1 = (rgb[1][1] - rgb[0][1]) * 1.0 / dy;

//     int dr2 = (rgb[2][0] - rgb[0][0]) * 1.0 / dy;
//     int dg2 = (rgb[2][0] - rgb[0][1]) * 1.0 / dy;
//     int db2 = (rgb[2][0] - rgb[0][2]) * 1.0 / dy;

//     for (int y = v[1][1]; y < v[0][1]; y++) {
//       int dx1 = (v[1][0] - v[0][0]) * 1.0 / dy;
//       int tx1 = 
//       for (int x = )
//     }
//   }
// }

// void g3::World::drawTriangle(int x0, int y0, unsigned long colour0,
//                              int x1, int y1, unsigned long colour1,
//                              int x2, int y2, unsigned long colour2
// )
// {
//   int v[3][2];

//   if (y0 == y1 && y1 == y2)
//     return;

//   if (y0 < y1) {
//     SWAP(x0, x1);
//     SWAP(y0, y1);
//   }
//   if (y0 < y2) {
//     SWAP(x0, x2);
//     SWAP(y0, y2);
//   }
//   if (y1 < y2) {
//     SWAP(x1, x2);
//     SWAP(y1, y2);
//   }

//   if (y0 == y1)
// }
