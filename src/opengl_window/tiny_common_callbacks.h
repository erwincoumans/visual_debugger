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

#ifndef TINY_COMMON_CALLBACKS_H
#define TINY_COMMON_CALLBACKS_H

typedef void (*TinyWheelCallback)(float deltax, float deltay);
typedef void (*TinyResizeCallback)(float width, float height);
typedef void (*TinyMouseMoveCallback)(float x, float y);
typedef void (*TinyMouseButtonCallback)(int button, int state, float x,
                                        float y);
typedef void (*TinyKeyboardCallback)(int keycode, int state);
typedef void (*TinyRenderCallback)();


// key codes matching GLFW (user must remap for other frameworks)
#define mjKEY_ESCAPE     256
#define mjKEY_ENTER      257
#define mjKEY_TAB        258
#define mjKEY_BACKSPACE  259
#define mjKEY_INSERT     260
#define mjKEY_DELETE     261
#define mjKEY_RIGHT      262
#define mjKEY_LEFT       263
#define mjKEY_DOWN       264
#define mjKEY_UP         265
#define mjKEY_PAGE_UP    266
#define mjKEY_PAGE_DOWN  267
#define mjKEY_HOME       268
#define mjKEY_END        269
#define mjKEY_F1         290
#define mjKEY_F2         291
#define mjKEY_F3         292
#define mjKEY_F4         293
#define mjKEY_F5         294
#define mjKEY_F6         295
#define mjKEY_F7         296
#define mjKEY_F8         297
#define mjKEY_F9         298
#define mjKEY_F10        299
#define mjKEY_F11        300
#define mjKEY_F12        301

enum {
  TINY_KEY_ESCAPE = 256,
  TINY_KEY_ENTER = 257,
  TINY_KEY_TAB=258,
  TINY_KEY_BACKSPACE=259,
  TINY_KEY_INSERT=260,
  TINY_KEY_DELETE=261,
  TINY_KEY_RIGHT_ARROW=262,
  TINY_KEY_LEFT_ARROW=263,
  TINY_KEY_DOWN_ARROW=264,
  TINY_KEY_UP_ARROW=265,
  TINY_KEY_PAGE_UP=266,
  TINY_KEY_PAGE_DOWN=267,
  TINY_KEY_HOME=268,
  TINY_KEY_END=269,

  TINY_KEY_SPACE = 32,
  TINY_KEY_F1 = 290,
  TINY_KEY_F2,
  TINY_KEY_F3,
  TINY_KEY_F4,
  TINY_KEY_F5,
  TINY_KEY_F6,
  TINY_KEY_F7,
  TINY_KEY_F8,
  TINY_KEY_F9,
  TINY_KEY_F10,
  TINY_KEY_F11,
  TINY_KEY_F12,
  TINY_KEY_F13,
  TINY_KEY_F14,
  TINY_KEY_F15,
  
  TINY_KEY_SHIFT,
  TINY_KEY_CONTROL,
  TINY_KEY_ALT,
  TINY_KEY_RETURN,
  
};

#endif  // TINY_COMMON_CALLBACKS_H
