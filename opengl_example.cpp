#include "tiny_opengl3_app.h"
#include "utils/tiny_chrome_trace_util.h"
#include "utils/tiny_logging.h"
#include <vector>

int main(int argc, char* argv[]) {
  TinyChromeUtilsStartTimings();

  TinyOpenGL3App app("test", 1024, 768);
  app.set_up_axis(2);
  
  int textureIndex = -1;
  int red = 0;
  int green = 128;
  int blue = 255;

  int texWidth = 1024;
  int texHeight = 1024;
  std::vector<unsigned char> texels;
  texels.resize(texWidth * texHeight * 3);
  for (int i = 0; i < texWidth * texHeight * 3; i++) texels[i] = 255;

  for (int i = 0; i < texWidth; i++) {
    for (int j = 0; j < texHeight; j++) {
      int a = i < texWidth / 2 ? 1 : 0;
      int b = j < texWidth / 2 ? 1 : 0;

      if (a == b) {
        texels[(i + j * texWidth) * 3 + 0] = red;
        texels[(i + j * texWidth) * 3 + 1] = green;
        texels[(i + j * texWidth) * 3 + 2] = blue;
      }
    }
  }

  
  
  
  while (!app.m_window->requested_exit()) {
    B3_PROFILE("mainloop");
    int upAxis = 2;
    float rgba[4] = {1,1,1,1};
    {
      app.swap_buffer();
    }
  }
  TinyChromeUtilsStopTimingsAndWriteJsonFile("diffsim.json");
}