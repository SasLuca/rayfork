#ifndef RAYFORK_H
#define RAYFORK_H

// Foundation
#include "rayfork/foundation/allocator.h"
#include "rayfork/foundation/analysis-annotations.h"
#include "rayfork/foundation/arr.h"
#include "rayfork/foundation/basicdef.h"
#include "rayfork/foundation/error.h"
#include "rayfork/foundation/io.h"
#include "rayfork/foundation/logger.h"
#include "rayfork/foundation/min-max.h"
#include "rayfork/foundation/numeric-limits.h"
#include "rayfork/foundation/rand.h"
#include "rayfork/foundation/str.h"
#include "rayfork/foundation/telemetry.h"

// Math
#include "rayfork/math/base64.h"
#include "rayfork/math/collision-detection.h"
#include "rayfork/math/math-types.h"
#include "rayfork/math/maths.h"

// Image
#include "rayfork/image/color.h"
#include "rayfork/image/dds-loading.h"
#include "rayfork/image/gif.h"
#include "rayfork/image/image.h"
#include "rayfork/image/image-gen.h"
#include "rayfork/image/image-loading.h"
#include "rayfork/image/image-manipulation.h"
#include "rayfork/image/ktx-loading.h"
#include "rayfork/image/mipmap.h"
#include "rayfork/image/pixel-format.h"
#include "rayfork/image/pkm-loading.h"

// GFX
#include "rayfork/gfx/bitmap-font.h"
#include "rayfork/gfx/builtin-camera.h"
#include "rayfork/gfx/camera.h"
#include "rayfork/gfx/context.h"
#include "rayfork/gfx/gfx.h"
#include "rayfork/gfx/gfx-types.h"
#include "rayfork/gfx/render-batch.h"
#include "rayfork/gfx/renderer.h"
#include "rayfork/gfx/backend.h"
#include "rayfork/gfx/texture.h"

// Model
#include "rayfork/model/animations.h"
#include "rayfork/model/materials.h"
#include "rayfork/model/mesh-gen.h"
#include "rayfork/model/model.h"

#endif // RAYFORK_H