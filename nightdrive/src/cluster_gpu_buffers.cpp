#include "cluster_gpu_buffers.h"
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

namespace nightdrive {
namespace {
void checkGL(const char* action) {
    const GLenum e = glGetError();
    if (e != GL_NO_ERROR) {
        throw std::runtime_error(std::string(action) + " OpenGL error: " + std::to_string(e));
    }
}

template <class T>
GLsizeiptr bytesOf(const std::vector<T>& input) {
    if (input.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / sizeof(T)) {
        throw std::overflow_error("Texture buffer exceeds GLsizeiptr range");
    }
    return static_cast<GLsizeiptr>(input.size() * sizeof(T));
}

// Avoid a zero-byte GL_TEXTURE_BUFFER; empty logical lists get a 1-texel dummy.
void updateBuffer(GLuint id, const void* bytes, GLsizeiptr count, GLsizeiptr emptySize) {
    glBindBuffer(GL_TEXTURE_BUFFER, id);
    glBufferData(GL_TEXTURE_BUFFER, count ? count : emptySize,
                 count ? bytes : nullptr, GL_DYNAMIC_DRAW);
}
}

ClusterGpuBuffers::~ClusterGpuBuffers() {
    // Only use this class while its GL context remains current/alive.
    destroy();
}

void ClusterGpuBuffers::initialize() {
    if (initialized_) return;
    glGenBuffers(3, buffers_);
    glGenTextures(3, textures_);
    const GLenum formats[3] = {GL_RG32UI, GL_R32UI, GL_RGBA32F};
    for (int i = 0; i < 3; ++i) {
        glBindBuffer(GL_TEXTURE_BUFFER, buffers_[i]);
        glBufferData(GL_TEXTURE_BUFFER, 16, nullptr, GL_DYNAMIC_DRAW);
        glBindTexture(GL_TEXTURE_BUFFER, textures_[i]);
        glTexBuffer(GL_TEXTURE_BUFFER, formats[i], buffers_[i]);
    }
    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    checkGL("ClusterGpuBuffers::initialize");
    initialized_ = true;
}

void ClusterGpuBuffers::upload(const PackedClusterGpuData& packed) {
    if (!initialized_) throw std::logic_error("Call initialize before upload");
    if (packed.clusterRangesRG.size() % 2 != 0 ||
        packed.lightsRGBA.size() % 8 != 0) {
        throw std::invalid_argument("Invalid packed texture buffer layout");
    }
    GLint maxTexels = 0;
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxTexels);
    if (maxTexels <= 0 ||
        packed.clusterRangesRG.size() / 2 > std::size_t(maxTexels) ||
        packed.lightIndicesR.size() > std::size_t(maxTexels) ||
        packed.lightsRGBA.size() / 4 > std::size_t(maxTexels)) {
        throw std::runtime_error("Cluster buffers exceed GL_MAX_TEXTURE_BUFFER_SIZE");
    }
    updateBuffer(buffers_[0], packed.clusterRangesRG.data(),
                 bytesOf(packed.clusterRangesRG), sizeof(std::uint32_t) * 2);
    updateBuffer(buffers_[1], packed.lightIndicesR.data(),
                 bytesOf(packed.lightIndicesR), sizeof(std::uint32_t));
    updateBuffer(buffers_[2], packed.lightsRGBA.data(),
                 bytesOf(packed.lightsRGBA), sizeof(float) * 4);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    checkGL("ClusterGpuBuffers::upload");
}

void ClusterGpuBuffers::bind(GLuint program, GLint firstUnit) const {
    if (!initialized_) throw std::logic_error("Call initialize before bind");
    GLint limit = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &limit);
    if (firstUnit < 0 || firstUnit + 3 > limit) {
        throw std::invalid_argument("Not enough texture units for ClusterGpuBuffers");
    }
    const char* samplers[3] = {"uClusterRanges", "uClusterIndices", "uClusterLights"};
    for (int i = 0; i < 3; ++i) {
        glActiveTexture(GL_TEXTURE0 + firstUnit + i);
        glBindTexture(GL_TEXTURE_BUFFER, textures_[i]);
        const GLint location = glGetUniformLocation(program, samplers[i]);
        if (location >= 0) glUniform1i(location, firstUnit + i);
    }
    checkGL("ClusterGpuBuffers::bind");
}

void ClusterGpuBuffers::destroy() {
    if (!initialized_) return;
    glDeleteTextures(3, textures_);
    glDeleteBuffers(3, buffers_);
    for (int i = 0; i < 3; ++i) textures_[i] = buffers_[i] = 0;
    initialized_ = false;
}
} // namespace nightdrive
