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

#ifndef TINY_OPENGL3_APP_H
#define TINY_OPENGL3_APP_H

#include "tiny_common_graphics_app_interface.h"
#include "tiny_window_interface.h"

struct TinyOpenGL3App : public TinyCommonGraphicsApp {
  struct TinyOpenGL3AppInternalData* m_data;

  class TinyGLPrimitiveRenderer* m_primRenderer;
  
  
  virtual void set_background_color(float red, float green, float blue);
  virtual void set_mp4_fps(int fps);

  TinyOpenGL3App(const char* title, int width, int height,
                 bool allowRetina = true, int windowType = 0,
                 int renderDevice = -1, int maxNumObjectCapacity = 128 * 1024,
                 int maxShapeCapacityInBytes = 128 * 1024 * 1024);

  virtual ~TinyOpenGL3App();

  
  void dump_next_frame_to_png(const char* pngFilename);
  void dump_frames_to_video(const char* mp4Filename);
  virtual void get_screen_pixels(unsigned char* rgbaBuffer,
                                 int bufferSizeInBytes, float* depthBuffer,
                                 int depthBufferSizeInBytes);
  virtual void set_viewport(int width, int height);

  
  virtual void set_up_axis(int axis);
  virtual int get_up_axis() const;

  virtual void swap_buffer();
 
  void setUserPointer(void* ptr);
  void* getUserPointer();

  double getTimeSeconds();
  unsigned long long int getTimeMicroSeconds();
  unsigned long long int getTimeNanoSeconds();

};

#endif  // TINY_OPENGL3_APP_H
