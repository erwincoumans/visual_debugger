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

#ifndef TINY_COMMON_GRAPHICS_APP_INTERFACE_H
#define TINY_COMMON_GRAPHICS_APP_INTERFACE_H


#include "tiny_window_interface.h"

struct DrawGridData {
  int gridSize;
  float upOffset;
  int upAxis;
  bool drawAxis;
  float gridColor[4];

  DrawGridData(int upAx = 2) : gridSize(10), upOffset(0.001f), drawAxis(false), upAxis(upAx) {
    gridColor[0] = 0.6f;
    gridColor[1] = 0.6f;
    gridColor[2] = 0.6f;
    gridColor[3] = 1.f;
  }
  void set_color(float red, float green, float blue, float alpha)
  {
      gridColor[0] = red;
      gridColor[1] = green;
      gridColor[2] = blue;
      gridColor[3] = alpha;
  }
};

enum EnumSphereLevelOfDetail {
  SPHERE_LOD_POINT_SPRITE = 0,
  SPHERE_LOD_LOW,
  SPHERE_LOD_MEDIUM,
  SPHERE_LOD_HIGH,
};
struct TinyCommonGraphicsApp {
  enum drawText3DOption {
    eDrawText3D_OrtogonalFaceCamera = 1,
    eDrawText3D_TrueType = 2,
    eDrawText3D_TrackObject = 4,
  };
  class TinyWindowInterface* m_window;
  class TinyGLInstancingRenderer* m_renderer;
  struct CommonParameterInterface* m_parameterInterface;
  struct Common2dCanvasInterface* m_2dCanvasInterface;

  bool m_leftMouseButton;
  bool m_middleMouseButton;
  bool m_rightMouseButton;
  float m_wheelMultiplier;
  float m_mouseMoveMultiplier;
  float m_mouseXpos;
  float m_mouseYpos;
  bool m_mouseInitialized;
  float m_backgroundColorRGB[3];

  TinyCommonGraphicsApp()
      : m_window(0),
        m_renderer(0),
        m_parameterInterface(0),
        m_2dCanvasInterface(0),
        m_leftMouseButton(false),
        m_middleMouseButton(false),
        m_rightMouseButton(false),
        m_wheelMultiplier(0.01f),
        m_mouseMoveMultiplier(0.4f),
        m_mouseXpos(0.f),
        m_mouseYpos(0.f),
        m_mouseInitialized(false) {
    m_backgroundColorRGB[0] = 0.95;
    m_backgroundColorRGB[1] = 0.95;
    m_backgroundColorRGB[2] = 0.95;
  }
  virtual ~TinyCommonGraphicsApp() {}

  virtual void dump_next_frame_to_png(const char* pngFilename) {}
  virtual void dump_frames_to_video(const char* mp4Filename) {}

  virtual void get_screen_pixels(unsigned char* rgbaBuffer,
                                 int bufferSizeInBytes, float* depthBuffer,
                                 int depthBufferSizeInBytes) {}
  virtual void set_viewport(int width, int height) {}

  virtual void getBackgroundColor(float* red, float* green, float* blue) const {
    if (red) *red = m_backgroundColorRGB[0];
    if (green) *green = m_backgroundColorRGB[1];
    if (blue) *blue = m_backgroundColorRGB[2];
  }
  virtual void set_mp4_fps(int fps) {}
  virtual void set_background_color(float red, float green, float blue) {
    m_backgroundColorRGB[0] = red;
    m_backgroundColorRGB[1] = green;
    m_backgroundColorRGB[2] = blue;
  }
  virtual void setMouseWheelMultiplier(float mult) { m_wheelMultiplier = mult; }
  virtual float getMouseWheelMultiplier() const { return m_wheelMultiplier; }

  virtual void setMouseMoveMultiplier(float mult) {
    m_mouseMoveMultiplier = mult;
  }

  virtual float getMouseMoveMultiplier() const { return m_mouseMoveMultiplier; }

  
  virtual void set_up_axis(int axis) = 0;
  virtual int get_up_axis() const = 0;

  virtual void swap_buffer() = 0;

  
  void defaultMouseButtonCallback(int button, int state, float x, float y) {
    if (button == 0) m_leftMouseButton = (state == 1);
    if (button == 1) m_middleMouseButton = (state == 1);

    if (button == 2) m_rightMouseButton = (state == 1);

    m_mouseXpos = x;
    m_mouseYpos = y;
    m_mouseInitialized = true;
  }
  void defaultMouseMoveCallback(float x, float y) {
    
    m_mouseXpos = x;
    m_mouseYpos = y;
    m_mouseInitialized = true;
  }
  //	void defaultKeyboardCallback(int key, int state)
  //	{
  //	}
  void defaultWheelCallback(float deltax, float deltay) {
  }
};

#endif  // TINY_COMMON_GRAPHICS_APP_INTERFACE_H
