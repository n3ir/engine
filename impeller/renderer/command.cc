// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/renderer/command.h"

#include <utility>

#include "impeller/base/validation.h"
#include "impeller/core/formats.h"
#include "impeller/renderer/vertex_descriptor.h"

namespace impeller {

bool Command::BindVertices(const VertexBuffer& buffer) {
  if (buffer.index_type == IndexType::kUnknown) {
    VALIDATION_LOG << "Cannot bind vertex buffer with an unknown index type.";
    return false;
  }

  vertex_buffers = {buffer.vertex_buffer};
  vertex_buffer_count = 1u;
  element_count = buffer.vertex_count;
  index_buffer = buffer.index_buffer;
  index_type = buffer.index_type;
  return true;
}

bool Command::BindResource(ShaderStage stage,
                           DescriptorType type,
                           const ShaderUniformSlot& slot,
                           const ShaderMetadata& metadata,
                           BufferView view) {
  FML_DCHECK(slot.ext_res_0 != VertexDescriptor::kReservedVertexBufferIndex);
  if (!view) {
    return false;
  }

  switch (stage) {
    case ShaderStage::kVertex:
      vertex_bindings.buffers.emplace_back(BufferAndUniformSlot{
          .slot = slot, .view = BufferResource(metadata, std::move(view))});
      return true;
    case ShaderStage::kFragment:
      fragment_bindings.buffers.emplace_back(BufferAndUniformSlot{
          .slot = slot, .view = BufferResource(metadata, std::move(view))});
      return true;
    case ShaderStage::kCompute:
      VALIDATION_LOG << "Use ComputeCommands for compute shader stages.";
    case ShaderStage::kUnknown:
      return false;
  }

  return false;
}

bool Command::BindResource(
    ShaderStage stage,
    DescriptorType type,
    const ShaderUniformSlot& slot,
    const std::shared_ptr<const ShaderMetadata>& metadata,
    BufferView view) {
  FML_DCHECK(slot.ext_res_0 != VertexDescriptor::kReservedVertexBufferIndex);
  if (!view) {
    return false;
  }

  switch (stage) {
    case ShaderStage::kVertex:
      vertex_bindings.buffers.emplace_back(BufferAndUniformSlot{
          .slot = slot, .view = BufferResource(*metadata, std::move(view))});
      return true;
    case ShaderStage::kFragment:
      fragment_bindings.buffers.emplace_back(BufferAndUniformSlot{
          .slot = slot, .view = BufferResource(*metadata, std::move(view))});
      return true;
    case ShaderStage::kCompute:
      VALIDATION_LOG << "Use ComputeCommands for compute shader stages.";
    case ShaderStage::kUnknown:
      return false;
  }

  return false;
}

bool Command::BindResource(ShaderStage stage,
                           DescriptorType type,
                           const SampledImageSlot& slot,
                           const ShaderMetadata& metadata,
                           std::shared_ptr<const Texture> texture,
                           const std::unique_ptr<const Sampler>& sampler) {
  if (!sampler) {
    return false;
  }
  if (!texture || !texture->IsValid()) {
    return false;
  }

  switch (stage) {
    case ShaderStage::kVertex:
      vertex_bindings.sampled_images.emplace_back(TextureAndSampler{
          .slot = slot,
          .texture = TextureResource(metadata, std::move(texture)),
          .sampler = &sampler,
      });
      return true;
    case ShaderStage::kFragment:
      fragment_bindings.sampled_images.emplace_back(TextureAndSampler{
          .slot = slot,
          .texture = TextureResource(metadata, std::move(texture)),
          .sampler = &sampler,
      });
      return true;
    case ShaderStage::kCompute:
      VALIDATION_LOG << "Use ComputeCommands for compute shader stages.";
    case ShaderStage::kUnknown:
      return false;
  }

  return false;
}

}  // namespace impeller
