#ifndef Renderer_H_
#define Renderer_H_

#include "AGL.h"
#include "AGLM.h"
#include "image.h"
#include <string>

namespace agl {
   enum BlendMode {DEFAULT, ADD, ALPHA};
   class Renderer
   {
   public:
      Renderer();
      virtual ~Renderer();

      virtual void init(const std::string& vertex, const std::string& fragment);
      virtual GLuint loadTexture(const std::string& imageName);

      virtual void begin(GLuint textureId, BlendMode mode);
      virtual void quad(const glm::vec3& pos, const glm::vec4& color, float size);
      virtual void end();

      virtual bool initialized() const;

      GLuint program_id() const;


   protected:

      virtual void blendMode(BlendMode mode);
      GLuint loadShader(const std::string& vertex, const std::string& fragment);
      std::string loadShaderFromFile(const std::string& fileName);

   protected:
      GLuint mShaderId;
      GLuint mVboPosId;
      GLuint mVaoId;
      bool mInitialized;
   };
}

#endif
