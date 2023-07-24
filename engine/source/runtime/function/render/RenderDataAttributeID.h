#pragma once

#include "utils/ID.h"

namespace MM {
namespace RenderSystem {
// clang-format off
/** sub_ID1_
 * | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - | - - - - - - - | - - - | - - - - - - - - | - - - | - - - | - - - | - - - |
 * |           pNext                 |              flags              |      empty    | IType |      format     | comp.r| comp.g| comp.b| comp.a|
 * |           16bit                 |              18bits             |      empty    | 3bits |      8bits      | 3bits | 3bits | 3bits | 3bits |
 * |  The last 16 bits of pointer.   |              Bitmask            |      empty    | value |      value      | value | value | value | value |
 * | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - | - - - - - - - | - - - | - - - - - - - - | - - - | - - - | - - - | - - - |
 *
 * sub_ID2_
 * | - - - - - - - | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - | - - - - - - - | - - - - - - - | - - - - - - - |
 * |    empty      |            image_               |     range.aspectMask      | range.MipLev  | range.LevCou  | range.ArrLey  | range.LayCou  |
 * |    empty      |            16bits               |         13bits            |    7bits      |    7bits      |    7bits      |    7bits      |
 * |    empty      |            value                |         BitMask           |    value      |    value      |    value      |    value      |
 * | - - - - - - - | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - | - - - - - - - | - - - - - - - | - - - - - - - |
**/
// clang-format on
using RenderImageViewAttributeID = Utils::ID2;

// clang-format off
/** sub_ID1_
 * | - - - - - - - - - - - - - - - - | - -   | - - | - - | - - | - - - | - - - | - - - |- - - - - - - |  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 * |           pNext                 |flags  |manFi|minFi|mipMd| adMoU | adMoV | adMoW | mip_lod_bias |                          empty                                       |
 * |           16bit                 |2bits  |2bits|2bits|2bits| 3bits | 3bits | 3bits |    7bits     |                          empty                                       |
 * |  The last 16 bits of pointer.   |Bitmask|value|value|value| value | value | value |    value     |                          empty                                       |
 * | - - - - - - - - - - - - - - - - | - - - | - - | - - | - - | - - - | - - - | - - - |- - - - - - - |  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 *
 * sub_ID2_
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - | -  | - - - - - - - | -  | - - - | - - - - - - - | - - - - - - - | - - - | -  |
 * |                                     empty                                         |ae  |    max_ani    |ce  |comp_op|    max_lod    |    min_lod    |bod_col|uc  |
 * |                                     empty                                         |1bit|     7bits     |1bit| 3bits |     7bits     |     7bits     | 3bits |1bit|
 * |                                     empty                                         |bit |     value     |bit | value |     value     |     value     | value |bit |
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - | -  | - - - - - - - | -  | - - - | - - - - - - - | - - - - - - - | - - - | -  |
**/
// clang-format on
using RenderSamplerAttributeID = Utils::ID2;

using RenderResourceDataAttributeID = Utils::ID3;

// clang-format off
/** sub_ID1_
 * | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - | - - | - - - - - - - - | - - - - - - | - - - - - - | - - - | - -  | - - |
 * |           pNext                 |              flags                    |IType|      format     |   mipLevels | arrayLayers |samples|tiling|SMod |
 * |           16bit                 |              19bits                   |2bits|      8bits      |     6bits   |    6bits    | 3bits |2bits |2bits|
 * |  The last 16 bits of pointer.   |              Bitmask                  |value|      value      |     value   |    value    | value |value |value|
 * | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - | - - | - - - - - - - - | - - - - - - | - - - - - - | - - - | - -  | - - |
 *
 * sub_ID2_
 * | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - - | - - - - -  |
 * |        ext.width          |         ext.height        |          ext.depth        |                 usage                   |image_layout|
 * |         13bits            |           13bits          |           13bits          |                 20bits                  |    5bits   |
 * |         value             |           value           |           value           |                 Bitmask                 |    value   |
 * | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - - | - - - - -  |
 *
 * sub_ID3_
 *  | - - - - - | - - - - - - - - - - - - - - - - - | - - - - | - - - - - - - - - - | - - - - - - - - - - | - - - - | - - - - - - - - - - - - - - |
 *  |   empty   |              flags                |  usage  |    requiredFlags    |    preferredFlags   | MemType |           priority          |                                                                                       |
 *  |   empty   |              17bits               |  4bits  |       10bits        |       10bits        |  4bits  |            14bits           |
 *  |   empty   |              BitMask              |  value  |       Bitmask       |       Bitmask       |  value  |            value            |
 *  | - - - - - | - - - - - - - - - - - - - - - - - | - - - - | - - - - - - - - - - | - - - - - - - - - - | - - - - | - - - - - - - - - - - - - - |
**/
// clang-format on
using RenderImageDataAttributeID = RenderResourceDataAttributeID;

// clang-format off
/** sub_ID1_
 * | - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - | - - | - - - - - - - - | - - - - - - - - - - - - - - - - - - - - - - - - - |
 * |               empty       |           pNext                 |sMod |      flags      |                     usage                         |
 * |               empty       |           16bit                 |2bits|      8bits      |                     25bits                        |
 * |               empty       |  the last 16 bits of pointer.   |value|      bitmask    |                     bitmask                       |
 * | - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - | - - | - - - - - - - - | - - - - - - - - - - - - - - - - - - - - - - - - - |
 *
 * sub_ID2_
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 * |                                                         size                                                                    |
 * |                                                         64bits                                                                  |
 * |                                                         value                                                                   |
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 *
 * sub_ID3_
 *  | - - - - - - - - - - - - - - - | - - - - | - - - - - - - - - - | - - - - - - - | - - - - - - - - - - | - - - - | - - - - - - - - - - - - - - |
 *  |          flags                |  usage  |    requiredFlags    |     empty     |    preferredFlags   | memType |           priority          |                                                                                       |
 *  |          15bits               |  4bits  |       10bits        |     empty     |       10bits        |  4bits  |            14bits           |
 *  |          bitmask              |  value  |       bitmask       |     empty     |       bitmask       |  value  |            value            |
 *  | - - - - - - - - - - - - - - - | - - - - | - - - - - - - - - - | - - - - - - - | - - - - - - - - - - | - - - - | - - - - - - - - - - - - - - |
**/
// clang-format on
using RenderBufferDataAttributeID = RenderResourceDataAttributeID;

// clang-format off
/** sub_ID1_
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 * |                                                         empty
 * |                                                         64bits                                                                  |
 * |                                                         value                                                                   |
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 *
 * sub_ID2_
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 * |                                                         empty
 * |                                                         64bits                                                                  |
 * |                                                         value                                                                   |
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 *
 * sub_ID3_
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 * |                                                         empty
 * |                                                         64bits                                                                  |
 * |                                                         value                                                                   |
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
**/
// clang-format on
using RenderMeshBufferDataAttributeID = RenderResourceDataAttributeID;
}  // namespace RenderSystem
}  // namespace MM
