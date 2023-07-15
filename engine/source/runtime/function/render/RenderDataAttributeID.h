#pragma once

#include "utils/DataAttributeID.h"

namespace MM {
namespace RenderSystem {
// clang-format off
/** resource_attribute1_
 * | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - | - - - - - - - | - - - | - - - - - - - - | - - - | - - - | - - - | - - - |
 * |           pNext                 |              flags              |      empty    | IType |      format     | comp.r| comp.g| comp.b| comp.a|
 * |           16bit                 |              18bits             |      empty    | 3bits |      8bits      | 3bits | 3bits | 3bits | 3bits |
 * |  The last 16 bits of pointer.   |              Bitmask            |      empty    | value |      value      | valus | valus | valus | valus |
 * | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - | - - - - - - - | - - - | - - - - - - - - | - - - | - - - | - - - | - - - |
 *
 * resource_attribute2_
 * | - - - - - - - | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - | - - - - - - - | - - - - - - - | - - - - - - - |
 * |    empty      |            image_               |     range.aspectMask      | range.MipLev  | range.LevCou  | range.ArrLey  | range.LayCou  |
 * |    empty      |            16bits               |         13bits            |    7bits      |    7bits      |    7bits      |    7bits      |
 * |    empty      |            value                |         BitMask           |    value      |    value      |    value      |    value      |
 * | - - - - - - - | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - | - - - - - - - | - - - - - - - | - - - - - - - |
**/
// clang-format on
using RenderImageViewAttributeID = Utils::DataAttributeID2;

// clang-format off
/** resource_attribute1_
 * | - - - - - - - - - - - - - - - - | - -   | - - | - - | - - | - - - | - - - | - - - |- - - - - - - |  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 * |           pNext                 |flags  |manFi|minFi|mipMd| adMoU | adMoV | adMoW | mip_lod_bias |                          empty                                       |
 * |           16bit                 |2bits  |2bits|2bits|2bits| 3bits | 3bits | 3bits |    7bits     |                          empty                                       |
 * |  The last 16 bits of pointer.   |Bitmask|value|value|value| value | value | value |    value     |                          empty                                       |
 * | - - - - - - - - - - - - - - - - | - - - | - - | - - | - - | - - - | - - - | - - - |- - - - - - - |  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 *
 * resource_attribute2_
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - | -  | - - - - - - - | -  | - - - | - - - - - - - | - - - - - - - | - - - | -  |
 * |                                     empty                                         |ae  |    max_ani    |ce  |comp_op|    max_lod    |    min_lod    |bod_col|uc  |
 * |                                     empty                                         |1bit|     7bits     |1bit| 3bits |     7bits     |     7bits     | 3bits |1bit|
 * |                                     empty                                         |bit |     value     |bit | value |     value     |     value     | value |bit |
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - | -  | - - - - - - - | -  | - - - | - - - - - - - | - - - - - - - | - - - | -  |
**/
// clang-format on
using RenderSamplerAttributeID = Utils::DataAttributeID2;

using RenderResourceDataAttributeID = Utils::DataAttributeID3;

// clang-format off
/** resource_attribute1_
 * | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - | - - | - - - - - - - - | - - - - - - | - - - - - - | - - - | - -  | - - |
 * |           pNext                 |              flags                    |IType|      format     |   mipLevels | arrayLayers |samples|tiling|SMod |
 * |           16bit                 |              19bits                   |2bits|      8bits      |     6bits   |    6bits    | 3bits |2bits |2bits|
 * |  The last 16 bits of pointer.   |              Bitmask                  |value|      value      |     value   |    value    | value |value |value|
 * | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - | - - | - - - - - - - - | - - - - - - | - - - - - - | - - - | - -  | - - |
 *
 * resource_attribute2_
 * | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - - | - - - - -  |
 * |        ext.width          |         ext.height        |          ext.depth        |                 usage                   |image_layout|
 * |         13bits            |           13bits          |           13bits          |                 20bits                  |    5bits   |
 * |         value             |           value           |           value           |                 Bitmask                 |    value   |
 * | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - - | - - - - -  |
 *
 * resource_attribute3_
 *  | - - - - - | - - - - - - - - - - - - - - - - - | - - - - | - - - - - - - - - - | - - - - - - - - - - | - - - - | - - - - - - - - - - - - - - |
 *  |   empty   |              flags                |  usage  |    requiredFlags    |    preferredFlags   | MemType |           priority          |                                                                                       |
 *  |   empty   |              17bits               |  4bits  |       10bits        |       10bits        |  4bits  |            14bits           |
 *  |   empty   |              BitMask              |  value  |       Bitmask       |       Bitmask       |  value  |            value            |
 *  | - - - - - | - - - - - - - - - - - - - - - - - | - - - - | - - - - - - - - - - | - - - - - - - - - - | - - - - | - - - - - - - - - - - - - - |
**/
// clang-format on
using RenderImageDataAttributeID = RenderResourceDataAttributeID;

// clang-format off
/** resource_attribute1_
 * | - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - | - - | - - - - - - - - | - - - - - - - - - - - - - - - - - - - - - - - - - |
 * |               empty       |           pNext                 |SMod |      flags      |                     usage                         |
 * |               empty       |           16bit                 |2bits|      8bits      |                     25bits                        |
 * |               empty       |  The last 16 bits of pointer.   |value|      Bitmask    |                     Bitmask                       |
 * | - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - | - - | - - - - - - - - | - - - - - - - - - - - - - - - - - - - - - - - - - |
 *
 * resource_attribute2_
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 * |                                                         size                                                                    |
 * |                                                         64bits                                                                  |
 * |                                                         value                                                                   |
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 *
 * resource_attribute3_
 *  | - - - - - - - - - - - - - - - | - - - - | - - - - - - - - - - | - - - - - - - | - - - - - - - - - - | - - - - | - - - - - - - - - - - - - - |
 *  |          flags                |  usage  |    requiredFlags    |     empty     |    preferredFlags   | MemType |           priority          |                                                                                       |
 *  |          15bits               |  4bits  |       10bits        |     empty     |       10bits        |  4bits  |            14bits           |
 *  |          BitMask              |  value  |       Bitmask       |     empty     |       Bitmask       |  value  |            value            |
 *  | - - - - - - - - - - - - - - - | - - - - | - - - - - - - - - - | - - - - - - - | - - - - - - - - - - | - - - - | - - - - - - - - - - - - - - |
**/
// clang-format on
using RenderBufferDataAttributeID = RenderResourceDataAttributeID;
}  // namespace RenderSystem
}  // namespace MM
