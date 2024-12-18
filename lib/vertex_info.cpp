#include "vertex_info.h"

// Position(3) + Color(4)
template <> const GLuint VertexInfo<VertexFormat::PosColor>::stride = 7;

template <>
const std::vector<GLuint> VertexInfo<VertexFormat::PosColor>::attrSize = {
    3 /* position size */, 4 /* color size */};

// PosTexCoor
template <> const GLuint VertexInfo<VertexFormat::PosTexCoor>::stride = 5;

template <>
const std::vector<GLuint> VertexInfo<VertexFormat::PosTexCoor>::attrSize = {
    3 /* position size */, 2 /* texture coor size */};

// PosFaceID
template <> const GLuint VertexInfo<VertexFormat::PosFaceID>::stride = 4;

template <>
const std::vector<GLuint> VertexInfo<VertexFormat::PosFaceID>::attrSize = {
    3 /* position size */, 1 /* face id */};
