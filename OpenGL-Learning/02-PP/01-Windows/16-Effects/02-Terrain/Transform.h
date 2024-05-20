#pragma once

#include "vmath.h"

class Transform
{
public:
   Transform(){
      transform_mat = vmath::mat4(1.0f); //identity
   }

   void scale(vmath::vec3 vec){
      transform_mat = transform_mat * vmath::scale(vec);
   }

   void scale(float x, float y, float z){
      this->scale(vmath::vec3(x, y, z));
   }

   void translate(vmath::vec3 vec){
      transform_mat = transform_mat * vmath::translate(vec);
   }

   void translate(float x, float y, float z){
      this->translate(vmath::vec3(x, y, z));
   }

   void rotate(vmath::vec3 vec, float angle){
      transform_mat = transform_mat * vmath::rotate(angle, vec);
   }

   void rotate(float x, float y, float z, float angle){
      this->rotate(vmath::vec3(x, y, z), angle);
   }

   void mult(Transform t){
      this->transform_mat *= t.get_matrix();
   }

   void mult(vmath::mat4 m){
      this->transform_mat *= m;
   }

   vmath::mat4 get_matrix(){
      return transform_mat;
   }

   vmath::vec3 transform_point(vmath::vec3 point3){
      vmath::vec4 point4 = vmath::vec4(point3, 1.0f);
      point4 = point4 * transform_mat;
      return vmath::vec3(point4);
   }

   ~Transform(){
   }


protected:
   vmath::mat4 transform_mat;
};
