#pragma once

// OpenGL 3.3 Core texture buffer adapter (requires GLEW and a current GL context).
#include <GL/glew.h>
#include "cluster_gpu_packing.h"

namespace nightdrive {

class ClusterGpuBuffers {
public:
    ClusterGpuBuffers() = default;
    ~ClusterGpuBuffers();

    ClusterGpuBuffers(const ClusterGpuBuffers&) = delete;
    ClusterGpuBuffers& operator=(const ClusterGpuBuffers&) = delete;

    // Must be called AFTER a GL 3.3 context and GLEW have been initialized.
    void initialize();
    // Must be called BEFORE destroying the GL context.
    void destroy();
    // Refresh buffers each frame after rebuilding CPU light lists.
    void upload(const PackedClusterGpuData& packed);
    // Caller must have activated the target shader using glUseProgram(program).
    // Bind textures and set sampler uniforms; requires three consecutive units.
    void bind(GLuint program, GLint firstUnit = 3) const;
    bool initialized() const { return initialized_; }

private:
    GLuint buffers_[3] = {0, 0, 0};
    GLuint textures_[3] = {0, 0, 0};
    bool initialized_ = false;
};
} // namespace nightdrive
