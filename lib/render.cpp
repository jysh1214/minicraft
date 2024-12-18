#include "render.h"

namespace {

template <VertexFormat format>
bool isOnFrustum(const Camera &camera, const Chunk<format> &chunk);

template <>
bool isOnFrustum<VertexFormat::PosColor>(
    const Camera &camera, const Chunk<VertexFormat::PosColor> &chunk) {

  const float radius = Chunk<VertexFormat::PosColor>::getRadius();

  // vector to chunk center
  const glm::vec3 centerVec = chunk.getCenter() - camera.getPos();

  auto sz = glm::dot(centerVec, camera.getFront());
  // not inside near and far
  if (!(camera.getNear() - radius <= sz <= camera.getFar() + radius)) {
    return false;
  }

  // self.factor_y = 1.0 / math.cos(half_y := V_FOV * 0.5)
  // self.tan_y = math.tan(half_y)
  // self.factor_x = 1.0 / math.cos(half_x := H_FOV * 0.5)
  // self.tan_x = math.tan(half_x)
  // # outside the TOP and BOTTOM planes?
  // sy = glm.dot(sphere_vec, self.cam.up)
  // dist = self.factor_y * CHUNK_SPHERE_RADIUS + sz * self.tan_y
  // if not (-dist <= sy <= dist):
  //     return False

  auto half_y = camera.getFovy() * 0.5;
  auto factor_y = 1.0f / std::cos(half_y);
  auto tan_y = std::tan(half_y);

  auto half_x = camera.getFovx() * 0.5;
  auto factor_x = 1.0f / std::cos(half_x);
  auto tan_x = std::tan(half_x);

  // outside the TOP and BOTTOM planes?
  auto sy = glm::dot(centerVec, camera.getUp());
  auto dist = factor_y * radius + sz * tan_y;
  if (!(-dist <= sy <= dist)) {
    return false;
  }

  //   # outside the LEFT and RIGHT planes?
  // sx = glm.dot(sphere_vec, self.cam.right)
  // dist = self.factor_x * CHUNK_SPHERE_RADIUS + sz * self.tan_x
  // if not (-dist <= sx <= dist):
  //     return False
  auto sx = glm::dot(centerVec, camera.getRight());
  auto dist0 = factor_x * radius + sz * tan_x;

  if (!(-dist0 <= sx <= dist0)) {
    return false;
  }

  return true;
}

template <>
bool isOnFrustum<VertexFormat::PosTexCoor>(
    const Camera &camera, const Chunk<VertexFormat::PosTexCoor> &chunk) {

  const float radius = Chunk<VertexFormat::PosTexCoor>::getRadius();

  // vector to chunk center
  const glm::vec3 centerVec = chunk.getCenter() - camera.getPos();

  auto sz = glm::dot(centerVec, camera.getFront());
  if (!(camera.getNear() - radius <= sz <= camera.getFar() + radius)) {
    return false;
  }

  auto half_y = camera.getFovy() * 0.5;
  auto factor_y = 1.0f / std::cos(half_y);
  auto tan_y = std::tan(half_y);

  auto half_x = camera.getFovx() * 0.5;
  auto factor_x = 1.0f / std::cos(half_x);
  auto tan_x = std::tan(half_x);

  auto sy = glm::dot(centerVec, camera.getUp());
  auto dist = factor_y * radius + sz * tan_y;
  if (!(-dist <= sy <= dist)) {
    return false;
  }

  auto sx = glm::dot(centerVec, camera.getRight());
  auto dist0 = factor_x * radius + sz * tan_x;

  if (!(-dist0 <= sx <= dist0)) {
    return false;
  }

  return true;
}

} // namespace

// PosColor
template <>
void Render<VertexFormat::PosColor>::draw(
    Camera &camera, Chunk<VertexFormat::PosColor> &chunk) {
  if (isOnFrustum<VertexFormat::PosColor>(camera, chunk))
    chunk.render();
}

template <>
void Render<VertexFormat::PosColor>::draw(
    Camera &camera, World<VertexFormat::PosColor> &world) {
  for (auto &chunk : world.chunks)
    this->draw(camera, chunk);
}

// PosTexCoor
template <>
void Render<VertexFormat::PosTexCoor>::draw(
    Camera &camera, Chunk<VertexFormat::PosTexCoor> &chunk) {
  if (isOnFrustum<VertexFormat::PosTexCoor>(camera, chunk))
    chunk.render();
}

template <>
void Render<VertexFormat::PosTexCoor>::draw(
    Camera &camera, World<VertexFormat::PosTexCoor> &world) {
  for (auto &chunk : world.chunks)
    this->draw(camera, chunk);
}
