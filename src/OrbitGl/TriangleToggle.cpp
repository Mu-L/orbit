// Copyright (c) 2020 The Orbit Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "TriangleToggle.h"

#include <GteVector.h>
#include <math.h>

#include "Batcher.h"
#include "Geometry.h"
#include "GlCanvas.h"
#include "TimeGraph.h"

TriangleToggle::TriangleToggle(State initial_state, StateChangeHandler handler,
                               TimeGraph* time_graph)
    : state_(initial_state),
      initial_state_(initial_state),
      handler_(handler),
      time_graph_(time_graph) {}

void TriangleToggle::Draw(GlCanvas* canvas, PickingMode picking_mode, float z_offset) {
  Batcher* batcher = canvas->GetBatcher();
  const float z = GlCanvas::kZValueTrack + z_offset;

  const bool picking = picking_mode != PickingMode::kNone;
  const Color kWhite(255, 255, 255, 255);
  const Color kGrey(100, 100, 100, 255);
  Color color = state_ == State::kInactive ? kGrey : kWhite;

  // Draw triangle.
  static float half_sqrt_three = 0.5f * sqrtf(3.f);
  float half_w = 0.5f * size_;
  float half_h = half_sqrt_three * half_w;

  if (!picking) {
    Vec3 position(pos_[0], pos_[1], 0.0f);

    Triangle triangle;
    if (state_ == State::kCollapsed) {
      triangle = Triangle(position + Vec3(-half_h, half_w, z), position + Vec3(-half_h, -half_w, z),
                          position + Vec3(half_w, 0.f, z));
    } else {
      triangle = Triangle(position + Vec3(half_w, half_h, z), position + Vec3(-half_w, half_h, z),
                          position + Vec3(0.f, -half_w, z));
    }
    batcher->AddTriangle(triangle, color, shared_from_this());
  } else {
    // When picking, draw a big square for easier picking.
    float original_width = 2 * half_w;
    float large_width = 2 * original_width;
    Box box(Vec2(pos_[0] - original_width, pos_[1] - original_width),
            Vec2(large_width, large_width), z);
    batcher->AddBox(box, color, shared_from_this());
  }
}

void TriangleToggle::OnPick(int /*x*/, int /*y*/) {}

void TriangleToggle::OnRelease() {
  if (IsInactive()) {
    return;
  }

  state_ = IsCollapsed() ? State::kExpanded : State::kCollapsed;
  handler_(state_);
  time_graph_->NeedsUpdate();
}

void TriangleToggle::SetState(State state, InitialStateUpdate behavior) {
  state_ = state;
  if (behavior == InitialStateUpdate::kReplaceInitialState) {
    initial_state_ = state;
  }
}
