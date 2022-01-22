// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NO_OPENGL3

#include "tiny_opengl3_app.h"

#ifdef BT_USE_EGL
#include "EGLOpenGLWindow.h"
#else
#endif  // BT_USE_EGL

#ifdef B3_USE_GLFW
#include "GLFWOpenGLWindow.h"
#else
#ifdef __APPLE__
#include "tiny_mac_opengl_window.h"
#else

#ifdef _WIN32
#include "tiny_win32_opengl_window.h"
#else
// let's cross the fingers it is Linux/X11
#include "tiny_x11_opengl_window.h"
#define BT_USE_X11  // for runtime backend selection, move to build?
#endif              //_WIN32
#endif              //__APPLE__
#endif              // B3_USE_GLFW

#include <stdio.h>

#include <assert.h>
#include <string.h>  //memset
#include <vector>

#include "utils/tiny_clock.h"
#include "tiny_gl_render_to_texture.h"


#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif  // _WIN32


struct TinyOpenGL3AppInternalData {
  GLuint m_fontTextureId;
  GLuint m_largeFontTextureId;
  struct sth_stash* m_fontStash;
  
  TinyClock m_clock;

  const char* m_frameDumpPngFileName;
  FILE* m_ffmpegFile;
  GLRenderToTexture* m_renderTexture;
  void* m_userPointer;
  int m_upAxis;  // y=1 or z=2 is supported
  int m_customViewPortWidth;
  int m_customViewPortHeight;
  int m_mp4Fps;

  TinyOpenGL3AppInternalData()
      : m_fontTextureId(0),
        m_largeFontTextureId(0),
        m_fontStash(0),
        m_frameDumpPngFileName(0),
        m_ffmpegFile(0),
        m_renderTexture(0),
        m_userPointer(0),
        m_upAxis(1),
        m_customViewPortWidth(-1),
        m_customViewPortHeight(-1),
        m_mp4Fps(60) {}
};

static TinyOpenGL3App* gApp = 0;

static void SimpleResizeCallback(float widthf, float heightf) {
  int width = (int)widthf;
  int height = (int)heightf;

  if (gApp)
  {
      gApp->set_viewport(width, height);
  }
    
}

static void SimpleKeyboardCallback(int key, int state) {
  if (key == TINY_KEY_ESCAPE && gApp && gApp->m_window) {
    gApp->m_window->set_request_exit();
  } else {
    // gApp->defaultKeyboardCallback(key,state);
  }
}

void SimpleMouseButtonCallback(int button, int state, float x, float y) {
  gApp->defaultMouseButtonCallback(button, state, x, y);
}
void SimpleMouseMoveCallback(float x, float y) {
  gApp->defaultMouseMoveCallback(x, y);
}

void SimpleWheelCallback(float deltax, float deltay) {
  gApp->defaultWheelCallback(deltax, deltay);
}




static void printGLString(const char* name, GLenum s) {
  const char* v = (const char*)glGetString(s);
  printf("%s = %s\n", name, v);
}

bool sOpenGLVerbose = true;


TinyOpenGL3App::TinyOpenGL3App(const char* title, int width, int height,
                               bool allowRetina, int windowType,
                               int renderDevice, int maxNumObjectCapacity,
                               int maxShapeCapacityInBytes) {
  gApp = this;

  m_data = new TinyOpenGL3AppInternalData;
  m_primRenderer = 0;

  m_data->m_clock.reset();

  if (windowType == 0) {
    m_window = new TinyDefaultOpenGLWindow();
  } else if (windowType == 1) {
#ifdef BT_USE_X11
    m_window = new TinyX11OpenGLWindow();
#else
    printf("X11 requires Linux. Loading default window instead. \n");
    m_window = new TinyDefaultOpenGLWindow();
#endif
  } else if (windowType == 2) {
#ifdef BT_USE_EGL
    m_window = new EGLOpenGLWindow();
#else
    printf("EGL window requires compilation with BT_USE_EGL.\n");
    printf("Loading default window instead. \n");
    m_window = new TinyDefaultOpenGLWindow();
#endif
  } else {
    printf("Unknown window type %d must be (0=default, 1=X11, 2=EGL).\n",
           windowType);
    printf("Loading default window instead. \n");
    m_window = new TinyDefaultOpenGLWindow();
  }

  m_window->set_allow_retina(allowRetina);

  TinyWindowConstructionInfo ci;
  //ci.m_openglVersion = 2;
  ci.m_title = title;
  ci.m_width = width;
  ci.m_height = height;
  ci.m_renderDevice = renderDevice;
  m_window->create_window(ci);

  m_window->set_window_title(title);

  assert(glGetError() == GL_NO_ERROR);

  {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
  }

  glClearColor(m_backgroundColorRGB[0], m_backgroundColorRGB[1],
               m_backgroundColorRGB[2], 1.f);

  m_window->start_rendering();
  width = m_window->get_width();
  height = m_window->get_height();

  assert(glGetError() == GL_NO_ERROR);

  // gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

#ifdef USE_GLEW
#ifndef __APPLE__
#ifndef _WIN32
#ifndef B3_USE_GLFW
  // some Linux implementations need the 'glewExperimental' to be true
  glewExperimental = GL_TRUE;
#endif  // B3_USE_GLFW
#endif  //_WIN32

#ifndef B3_USE_GLFW
  if (glewInit() != GLEW_OK) exit(1);  // or handle the error in a nicer way
  if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
    exit(1);              // or handle the error in a nicer way
#endif                    // B3_USE_GLFW
#endif                    //__APPLE__
#endif                    // USE_GLEW

  glGetError();  // don't remove this call, it is needed for Ubuntu

  assert(glGetError() == GL_NO_ERROR);

  m_parameterInterface = 0;

  assert(glGetError() == GL_NO_ERROR);

#ifdef OPENGL_WINDOW_ENABLE_FONTS
  m_primRenderer = new TinyGLPrimitiveRenderer(width, height);
#endif
  m_window->set_resize_callback(SimpleResizeCallback);
#ifdef OPENGL_WINDOW_ENABLE_FONTS
  m_primRenderer->set_screen_size(width, height);
  assert(glGetError() == GL_NO_ERROR);
#endif

  m_window->set_mouse_move_callback(SimpleMouseMoveCallback);
  m_window->set_mouse_button_callback(SimpleMouseButtonCallback);
  m_window->set_keyboard_callback(SimpleKeyboardCallback);
  m_window->set_wheel_callback(SimpleWheelCallback);
#if OPENGL_WINDOW_ENABLE_FONTS
  TwGenerateDefaultFonts();
  m_data->m_fontTextureId = BindFont(g_DefaultNormalFont);
  m_data->m_largeFontTextureId = BindFont(g_DefaultLargeFont);

  {
    m_data->m_renderCallbacks = new OpenGL2RenderCallbacks(m_primRenderer);
    
    
    m_data->m_fontStash = sth_create(
        512, 512, m_data->m_renderCallbacks);  // 256,256);//,1024);//512,512);

    assert(glGetError() == GL_NO_ERROR);

    if (!m_data->m_fontStash) {
      printf("Could not create stash");
      // fprintf(stderr, "Could not create stash.\n");
    }

    
    unsigned char* data2 = OpenSansData;
    unsigned char* data = (unsigned char*)data2;
    if (!(m_data->m_droidRegular =
              sth_add_font_from_memory(m_data->m_fontStash, data))) {
      printf("error!\n");
    }
    
    assert(glGetError() == GL_NO_ERROR);
  }
#endif //OPENGL_WINDOW_ENABLE_FONTS
}


template <typename T>
inline int projectWorldCoordToScreen(T objx, T objy, T objz,
                                     const T modelMatrix[16],
                                     const T projMatrix[16],
                                     const int viewport[4], T* winx, T* winy,
                                     T* winz) {
  int i;
  T in2[4];
  T tmp[4];

  in2[0] = objx;
  in2[1] = objy;
  in2[2] = objz;
  in2[3] = T(1.0);

  for (i = 0; i < 4; i++) {
    tmp[i] = in2[0] * modelMatrix[0 * 4 + i] + in2[1] * modelMatrix[1 * 4 + i] +
             in2[2] * modelMatrix[2 * 4 + i] + in2[3] * modelMatrix[3 * 4 + i];
  }

  T out[4];
  for (i = 0; i < 4; i++) {
    out[i] = tmp[0] * projMatrix[0 * 4 + i] + tmp[1] * projMatrix[1 * 4 + i] +
             tmp[2] * projMatrix[2 * 4 + i] + tmp[3] * projMatrix[3 * 4 + i];
  }

  if (out[3] == T(0.0)) return 0;
  out[0] /= out[3];
  out[1] /= out[3];
  out[2] /= out[3];
  /* Map x, y and z to range 0-1 */
  out[0] = out[0] * T(0.5) + T(0.5);
  out[1] = out[1] * T(0.5) + T(0.5);
  out[2] = out[2] * T(0.5) + T(0.5);

  /* Map x,y to viewport */
  out[0] = out[0] * viewport[2] + viewport[0];
  out[1] = out[1] * viewport[3] + viewport[1];

  *winx = out[0];
  *winy = out[1];
  *winz = out[2];
  return 1;
}




void TinyOpenGL3App::set_background_color(float red, float green, float blue) {
  TinyCommonGraphicsApp::set_background_color(red, green, blue);
  glClearColor(m_backgroundColorRGB[0], m_backgroundColorRGB[1],
               m_backgroundColorRGB[2], 1.f);
}

TinyOpenGL3App::~TinyOpenGL3App() {
  
#ifdef OPENGL_WINDOW_ENABLE_FONTS
  delete m_primRenderer;
  sth_delete(m_data->m_fontStash);
  delete m_data->m_renderCallbacks;
  TwDeleteDefaultFonts();
#endif

  m_window->close_window();

  delete m_window;
  delete m_data;
}

void TinyOpenGL3App::set_viewport(int width, int height) {
  m_data->m_customViewPortWidth = width;
  m_data->m_customViewPortHeight = height;
  if (width >= 0) {
    glViewport(0, 0, width, height);
  } 
}

void TinyOpenGL3App::get_screen_pixels(unsigned char* rgbaBuffer,
                                       int bufferSizeInBytes,
                                       float* depthBuffer,
                                       int depthBufferSizeInBytes) {
  int width = m_data->m_customViewPortWidth;
  int height = m_data->m_customViewPortHeight;

  assert((width * height * 4) == bufferSizeInBytes);
  if ((width * height * 4) == bufferSizeInBytes) {
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgbaBuffer);
    int glstat;
    glstat = glGetError();
    assert(glstat == GL_NO_ERROR);
  }
  assert((width * height * sizeof(float)) == depthBufferSizeInBytes);
  if ((width * height * sizeof(float)) == depthBufferSizeInBytes) {
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT,
                 depthBuffer);
    int glstat;
    glstat = glGetError();
    assert(glstat == GL_NO_ERROR);
  }
}

#ifdef STB_TEXTURE_TO_FILE
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image/stb_image_write.h"
static void writeTextureToFile(int textureWidth, int textureHeight,
                               const char* fileName, FILE* ffmpegVideo) {
  int numComponents = 4;
  // glPixelStorei(GL_PACK_ALIGNMENT,1);

  assert(glGetError() == GL_NO_ERROR);
  // glReadBuffer(GL_BACK);//COLOR_ATTACHMENT0);

  float* orgPixels =
      (float*)malloc(textureWidth * textureHeight * numComponents * 4);
  glReadPixels(0, 0, textureWidth, textureHeight, GL_RGBA, GL_FLOAT, orgPixels);
  // it is useful to have the actual float values for debugging purposes

  // convert float->char
  char* pixels = (char*)malloc(textureWidth * textureHeight * numComponents);
  assert(glGetError() == GL_NO_ERROR);

  for (int j = 0; j < textureHeight; j++) {
    for (int i = 0; i < textureWidth; i++) {
      pixels[(j * textureWidth + i) * numComponents] =
          char(orgPixels[(j * textureWidth + i) * numComponents] * 255.f);
      pixels[(j * textureWidth + i) * numComponents + 1] =
          char(orgPixels[(j * textureWidth + i) * numComponents + 1] * 255.f);
      pixels[(j * textureWidth + i) * numComponents + 2] =
          char(orgPixels[(j * textureWidth + i) * numComponents + 2] * 255.f);
      pixels[(j * textureWidth + i) * numComponents + 3] =
          char(orgPixels[(j * textureWidth + i) * numComponents + 3] * 255.f);
    }
  }

  if (ffmpegVideo) {
    fwrite(pixels, textureWidth * textureHeight * numComponents, 1,
           ffmpegVideo);
    // fwrite(pixels,
    // 100,1,ffmpegVideo);//textureWidth*textureHeight*numComponents, 1,
    // ffmpegVideo);
  } else {
    if (1) {
      // swap the pixels
      unsigned char tmp;

      for (int j = 0; j < textureHeight / 2; j++) {
        for (int i = 0; i < textureWidth; i++) {
          for (int c = 0; c < numComponents; c++) {
            tmp = pixels[(j * textureWidth + i) * numComponents + c];
            pixels[(j * textureWidth + i) * numComponents + c] =
                pixels[((textureHeight - j - 1) * textureWidth + i) *
                           numComponents +
                       c];
            pixels[((textureHeight - j - 1) * textureWidth + i) *
                       numComponents +
                   c] = tmp;
          }
        }
      }
    }
    stbi_write_png(fileName, textureWidth, textureHeight, numComponents, pixels,
                   textureWidth * numComponents);
  }

  free(pixels);
  free(orgPixels);
}
#endif //STB_TEXTURE_TO_FILE

void TinyOpenGL3App::swap_buffer() {
#ifdef STB_TEXTURE_TO_FILE
  if (m_data->m_frameDumpPngFileName) {
    int width = m_data->m_customViewPortWidth;
    int height = m_data->m_customViewPortHeight;

    writeTextureToFile(width, height, m_data->m_frameDumpPngFileName,
                       m_data->m_ffmpegFile);
    m_data->m_renderTexture->disable();
    if (m_data->m_ffmpegFile == 0) {
      m_data->m_frameDumpPngFileName = 0;
    }
  }
#endif //STB_TEXTURE_TO_FILE
  m_window->end_rendering();
  m_window->start_rendering();
}

void TinyOpenGL3App::set_mp4_fps(int fps) { m_data->m_mp4Fps = fps; }

// see also
// http://blog.mmacklin.com/2013/06/11/real-time-video-capture-with-ffmpeg/
void TinyOpenGL3App::dump_frames_to_video(const char* mp4FileName) {
  if (mp4FileName) {
    int width = m_data->m_customViewPortWidth;
    int height = m_data->m_customViewPortHeight;

    char cmd[8192];

    sprintf(cmd,
            "ffmpeg -r %d -f rawvideo -pix_fmt rgba -s %dx%d -i - "
            "-threads 0 -y -b:v 50000k   -c:v libx264 -preset slow -crf 22 -an "
            "  -pix_fmt yuv420p -vf vflip %s",
            m_data->m_mp4Fps, width, height, mp4FileName);

    if (m_data->m_ffmpegFile) {
      pclose(m_data->m_ffmpegFile);
    }
    if (mp4FileName) {
      m_data->m_ffmpegFile = popen(cmd, "w");

      m_data->m_frameDumpPngFileName = mp4FileName;
    }
  } else {
    if (m_data->m_ffmpegFile) {
      fflush(m_data->m_ffmpegFile);
      pclose(m_data->m_ffmpegFile);
      m_data->m_frameDumpPngFileName = 0;
    }
    m_data->m_ffmpegFile = 0;
  }
}
void TinyOpenGL3App::dump_next_frame_to_png(const char* filename) {
  // open pipe to ffmpeg's stdin in binary write mode

  m_data->m_frameDumpPngFileName = filename;

  // you could use m_renderTexture to allow to render at higher resolutions,
  // such as 4k or so
  if (!m_data->m_renderTexture) {
    m_data->m_renderTexture = new GLRenderToTexture();
    GLuint renderTextureId;
    glGenTextures(1, &renderTextureId);

    // "Bind" the newly created texture : all future texture functions will
    // modify this texture
    glBindTexture(GL_TEXTURE_2D, renderTextureId);

    // Give an empty image to OpenGL ( the last "0" )
    // glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, g_OpenGLWidth,g_OpenGLHeight,
    // 0,GL_RGBA, GL_UNSIGNED_BYTE, 0); glTexImage2D(GL_TEXTURE_2D,
    // 0,GL_RGBA32F, g_OpenGLWidth,g_OpenGLHeight, 0,GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA32F,
            m_data->m_customViewPortWidth,
            m_data->m_customViewPortHeight,
        0, GL_RGBA, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_data->m_renderTexture->init(
         m_data->m_customViewPortWidth,
            m_data->m_customViewPortHeight,
        renderTextureId, RENDERTEXTURE_COLOR);
  }

  m_data->m_renderTexture->enable();
}

void TinyOpenGL3App::set_up_axis(int axis) {
  assert((axis == 1) || (axis == 2));  // only Y or Z is supported at the moment
  m_data->m_upAxis = axis;
}
int TinyOpenGL3App::get_up_axis() const { return m_data->m_upAxis; }



void TinyOpenGL3App::setUserPointer(void* ptr)
{
    m_data->m_userPointer = ptr;
}
void* TinyOpenGL3App::getUserPointer()
{
    return m_data->m_userPointer;
}

double TinyOpenGL3App::getTimeSeconds()
{
    return m_data->m_clock.get_time_seconds();
}

unsigned long long int TinyOpenGL3App::getTimeMicroSeconds()
{
    return m_data->m_clock.get_time_microseconds();
}

unsigned long long int TinyOpenGL3App::getTimeNanoSeconds()
{
    return m_data->m_clock.get_time_nanoseconds();
}

#endif  //#ifndef NO_OPENGL3

