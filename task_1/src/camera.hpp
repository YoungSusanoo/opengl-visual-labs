#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
  Camera(glm::vec3 center, float radius);

  glm::mat4 getView() const;

  void move(float dx, float dy);
  void scroll(float dradius);

private:
  glm::vec3 center_;
  float     azimuth_;
  float     elevation_;
  float     radius_;
};
