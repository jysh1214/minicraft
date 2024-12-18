#ifndef MC_LIB_VERTEX_FORMAT_H
#define MC_LIB_VERTEX_FORMAT_H

// clang-format off
/**
 * @brief `VertexFormat` 
 * 
 * 
 * Layout:
 * 
 * Color:
 *     GLfloat vertex[] = {
 *       // Position    // Color
 *       0, 0, 0,       0, 0, 0, 0
 *       ...
 *     };
 * 
 * Texture:
 *     GLfloat vertex[] = {
 *       // Position    // Texture
 *       0, 0, 0,       0, 0,
 *       ...
 *     };
 * 
 * ColorTexture:
 *     GLfloat vertex[] = {
 *       // Position    // Color      // Texture
 *       0, 0, 0,       0, 0, 0, 0,   0, 0
 *       ...
 *     };
 * 
*/
enum class VertexFormat {
  PosColor,
  PosTexCoor,
  PosColorTexCoor,
  PosFaceID,
};
// clang-format on

#endif // MC_LIB_VERTEX_FORMAT_H
