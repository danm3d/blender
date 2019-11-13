#include "mixed.h"

#include "FN_generic_array_ref.h"
#include "FN_generic_vector_array.h"
#include "FN_multi_function_common_contexts.h"

#include "BLI_math_cxx.h"
#include "BLI_lazy_init_cxx.h"
#include "BLI_string_map.h"
#include "BLI_array_cxx.h"
#include "BLI_noise.h"
#include "BLI_hash.h"

#include "DNA_object_types.h"
#include "DNA_mesh_types.h"
#include "DNA_meshdata_types.h"

namespace FN {

using BLI::float3;
using BLI::float4x4;
using BLI::rgba_f;
using BLI::TemporaryArray;

MF_AddFloats::MF_AddFloats()
{
  MFSignatureBuilder signature("Add Floats");
  signature.readonly_single_input<float>("A");
  signature.readonly_single_input<float>("B");
  signature.single_output<float>("Result");
  this->set_signature(signature);
}

void MF_AddFloats::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  auto a = params.readonly_single_input<float>(0, "A");
  auto b = params.readonly_single_input<float>(1, "B");
  auto result = params.uninitialized_single_output<float>(2, "Result");

  for (uint i : mask.indices()) {
    result[i] = a[i] + b[i];
  }
}

MF_AddFloat3s::MF_AddFloat3s()
{
  MFSignatureBuilder signature("Add Float3s");
  signature.readonly_single_input<float3>("A");
  signature.readonly_single_input<float3>("B");
  signature.single_output<float3>("Result");
  this->set_signature(signature);
}

void MF_AddFloat3s::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  auto a = params.readonly_single_input<float3>(0, "A");
  auto b = params.readonly_single_input<float3>(1, "B");
  auto result = params.uninitialized_single_output<float3>(2, "Result");

  for (uint i : mask.indices()) {
    result[i] = a[i] + b[i];
  }
}

MF_CombineColor::MF_CombineColor()
{
  MFSignatureBuilder signature("Combine Color");
  signature.readonly_single_input<float>("R");
  signature.readonly_single_input<float>("G");
  signature.readonly_single_input<float>("B");
  signature.readonly_single_input<float>("A");
  signature.single_output<rgba_f>("Color");
  this->set_signature(signature);
}

void MF_CombineColor::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  VirtualListRef<float> r = params.readonly_single_input<float>(0, "R");
  VirtualListRef<float> g = params.readonly_single_input<float>(1, "G");
  VirtualListRef<float> b = params.readonly_single_input<float>(2, "B");
  VirtualListRef<float> a = params.readonly_single_input<float>(3, "A");
  MutableArrayRef<rgba_f> color = params.uninitialized_single_output<rgba_f>(4, "Color");

  for (uint i : mask.indices()) {
    color[i] = {r[i], g[i], b[i], a[i]};
  }
}

MF_SeparateColor::MF_SeparateColor()
{
  MFSignatureBuilder signature("Separate Color");
  signature.readonly_single_input<rgba_f>("Color");
  signature.single_output<float>("R");
  signature.single_output<float>("G");
  signature.single_output<float>("B");
  signature.single_output<float>("A");
  this->set_signature(signature);
}

void MF_SeparateColor::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  auto color = params.readonly_single_input<rgba_f>(0, "Color");
  auto r = params.uninitialized_single_output<float>(1, "R");
  auto g = params.uninitialized_single_output<float>(2, "G");
  auto b = params.uninitialized_single_output<float>(3, "B");
  auto a = params.uninitialized_single_output<float>(4, "A");

  for (uint i : mask.indices()) {
    rgba_f v = color[i];
    r[i] = v.r;
    g[i] = v.g;
    b[i] = v.b;
    a[i] = v.a;
  }
}

MF_CombineVector::MF_CombineVector()
{
  MFSignatureBuilder signature("Combine Vector");
  signature.readonly_single_input<float>("X");
  signature.readonly_single_input<float>("Y");
  signature.readonly_single_input<float>("Z");
  signature.single_output<float3>("Vector");
  this->set_signature(signature);
}

void MF_CombineVector::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  VirtualListRef<float> x = params.readonly_single_input<float>(0, "X");
  VirtualListRef<float> y = params.readonly_single_input<float>(1, "Y");
  VirtualListRef<float> z = params.readonly_single_input<float>(2, "Z");
  MutableArrayRef<float3> vector = params.uninitialized_single_output<float3>(3, "Vector");

  for (uint i : mask.indices()) {
    vector[i] = {x[i], y[i], z[i]};
  }
}

MF_SeparateVector::MF_SeparateVector()
{
  MFSignatureBuilder signature("Separate Vector");
  signature.readonly_single_input<float3>("Vector");
  signature.single_output<float>("X");
  signature.single_output<float>("Y");
  signature.single_output<float>("Z");
  this->set_signature(signature);
}

void MF_SeparateVector::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  auto vector = params.readonly_single_input<float3>(0, "Vector");
  auto x = params.uninitialized_single_output<float>(1, "X");
  auto y = params.uninitialized_single_output<float>(2, "Y");
  auto z = params.uninitialized_single_output<float>(3, "Z");

  for (uint i : mask.indices()) {
    float3 v = vector[i];
    x[i] = v.x;
    y[i] = v.y;
    z[i] = v.z;
  }
}

MF_VectorDistance::MF_VectorDistance()
{
  MFSignatureBuilder signature("Vector Distance");
  signature.readonly_single_input<float3>("A");
  signature.readonly_single_input<float3>("A");
  signature.single_output<float>("Distances");
  this->set_signature(signature);
}

void MF_VectorDistance::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  auto a = params.readonly_single_input<float3>(0, "A");
  auto b = params.readonly_single_input<float3>(1, "B");
  auto distances = params.uninitialized_single_output<float>(2, "Distances");

  for (uint i : mask.indices()) {
    distances[i] = float3::distance(a[i], b[i]);
  }
}

MF_FloatArraySum::MF_FloatArraySum()
{
  MFSignatureBuilder signature("Float Array Sum");
  signature.readonly_vector_input<float>("Array");
  signature.single_output<float>("Sum");
  this->set_signature(signature);
}

void MF_FloatArraySum::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  auto arrays = params.readonly_vector_input<float>(0, "Array");
  MutableArrayRef<float> sums = params.uninitialized_single_output<float>(1, "Sum");

  for (uint i : mask.indices()) {
    float sum = 0.0f;
    VirtualListRef<float> array = arrays[i];
    for (uint j = 0; j < array.size(); j++) {
      sum += array[j];
    }
    sums[i] = sum;
  }
}

MF_FloatRange::MF_FloatRange()
{
  MFSignatureBuilder signature("Float Range");
  signature.readonly_single_input<int>("Amount");
  signature.readonly_single_input<float>("Start");
  signature.readonly_single_input<float>("Step");
  signature.vector_output<float>("Range");
  this->set_signature(signature);
}

void MF_FloatRange::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  VirtualListRef<int> amounts = params.readonly_single_input<int>(0, "Amount");
  VirtualListRef<float> starts = params.readonly_single_input<float>(1, "Start");
  VirtualListRef<float> steps = params.readonly_single_input<float>(2, "Step");
  auto lists = params.vector_output<float>(3, "Range");

  for (uint i : mask.indices()) {
    int amount = amounts[i];
    float start = starts[i];
    float step = steps[i];

    for (int j = 0; j < amount; j++) {
      float value = start + j * step;
      lists.append_single(i, value);
    }
  }
}

MF_ObjectVertexPositions::MF_ObjectVertexPositions()
{
  MFSignatureBuilder signature{"Object Vertex Positions"};
  signature.readonly_single_input<Object *>("Object");
  signature.vector_output<float3>("Positions");
  this->set_signature(signature);
}

void MF_ObjectVertexPositions::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  VirtualListRef<Object *> objects = params.readonly_single_input<Object *>(0, "Object");
  auto positions = params.vector_output<float3>(1, "Positions");

  for (uint i : mask.indices()) {
    Object *object = objects[i];
    if (object == nullptr || object->type != OB_MESH) {
      continue;
    }

    float4x4 transform = object->obmat;

    Mesh *mesh = (Mesh *)object->data;
    TemporaryArray<float3> coords(mesh->totvert);
    for (uint j = 0; j < mesh->totvert; j++) {
      coords[j] = transform.transform_position(mesh->mvert[j].co);
    }
    positions.extend_single(i, coords);
  }
}

MF_ObjectWorldLocation::MF_ObjectWorldLocation()
{
  MFSignatureBuilder signature("Object Location");
  signature.readonly_single_input<Object *>("Object");
  signature.single_output<float3>("Location");
  this->set_signature(signature);
}

void MF_ObjectWorldLocation::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  auto objects = params.readonly_single_input<Object *>(0, "Object");
  auto locations = params.uninitialized_single_output<float3>(1, "Location");

  for (uint i : mask.indices()) {
    if (objects[i] != nullptr) {
      locations[i] = objects[i]->obmat[3];
    }
    else {
      locations[i] = float3(0, 0, 0);
    }
  }
}

MF_SwitchSingle::MF_SwitchSingle(const CPPType &type) : m_type(type)
{
  MFSignatureBuilder signature("Switch");
  signature.readonly_single_input<bool>("Condition");
  signature.readonly_single_input("True", m_type);
  signature.readonly_single_input("False", m_type);
  signature.single_output("Result", m_type);
  this->set_signature(signature);
}

void MF_SwitchSingle::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  VirtualListRef<bool> conditions = params.readonly_single_input<bool>(0, "Condition");
  GenericVirtualListRef if_true = params.readonly_single_input(1, "True");
  GenericVirtualListRef if_false = params.readonly_single_input(2, "False");
  GenericMutableArrayRef results = params.uninitialized_single_output(3, "Result");

  for (uint i : mask.indices()) {
    if (conditions[i]) {
      results.copy_in__uninitialized(i, if_true[i]);
    }
    else {
      results.copy_in__uninitialized(i, if_false[i]);
    }
  }
}

MF_SwitchVector::MF_SwitchVector(const CPPType &type) : m_type(type)
{
  MFSignatureBuilder signature("Switch");
  signature.readonly_single_input<bool>("Condition");
  signature.readonly_vector_input("True", m_type);
  signature.readonly_vector_input("False", m_type);
  signature.vector_output("Result", m_type);
  this->set_signature(signature);
}

void MF_SwitchVector::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  VirtualListRef<bool> conditions = params.readonly_single_input<bool>(0, "Condition");
  GenericVirtualListListRef if_true = params.readonly_vector_input(1, "True");
  GenericVirtualListListRef if_false = params.readonly_vector_input(2, "False");
  GenericVectorArray &results = params.vector_output(3, "Result");

  for (uint i : mask.indices()) {
    if (conditions[i]) {
      results.extend_single__copy(i, if_true[i]);
    }
    else {
      results.extend_single__copy(i, if_false[i]);
    }
  }
}

MF_TextLength::MF_TextLength()
{
  MFSignatureBuilder signature("Text Length");
  signature.readonly_single_input<std::string>("Text");
  signature.single_output<int>("Length");
  this->set_signature(signature);
}

void MF_TextLength::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  auto texts = params.readonly_single_input<std::string>(0, "Text");
  auto lengths = params.uninitialized_single_output<int>(1, "Length");

  for (uint i : mask.indices()) {
    lengths[i] = texts[i].size();
  }
}

MF_SimpleVectorize::MF_SimpleVectorize(const MultiFunction &function,
                                       ArrayRef<bool> input_is_vectorized)
    : m_function(function), m_input_is_vectorized(input_is_vectorized)
{
  BLI_assert(input_is_vectorized.contains(true));

  MFSignatureBuilder signature(function.name() + " (Vectorized)");

  bool found_output_param = false;
  UNUSED_VARS_NDEBUG(found_output_param);
  for (uint param_index : function.param_indices()) {
    MFParamType param_type = function.param_type(param_index);
    StringRef param_name = function.param_name(param_index);
    switch (param_type.category()) {
      case MFParamType::ReadonlyVectorInput:
      case MFParamType::VectorOutput:
      case MFParamType::MutableVector: {
        BLI_assert(false);
        break;
      }
      case MFParamType::ReadonlySingleInput: {
        BLI_assert(!found_output_param);
        if (input_is_vectorized[param_index]) {
          signature.readonly_vector_input(param_name + " (List)", param_type.type());
          m_vectorized_inputs.append(param_index);
        }
        else {
          signature.readonly_single_input(param_name, param_type.type());
        }
        break;
      }
      case MFParamType::SingleOutput: {
        signature.vector_output(param_name + " (List)", param_type.type());
        m_output_indices.append(param_index);
        found_output_param = true;
        break;
      }
    }
  }
  this->set_signature(signature);
}

void MF_SimpleVectorize::call(MFMask mask, MFParams params, MFContext context) const
{
  if (mask.indices_amount() == 0) {
    return;
  }
  uint array_size = mask.min_array_size();

  Vector<int> vectorization_lengths(array_size);
  vectorization_lengths.fill_indices(mask.indices(), -1);

  for (uint param_index : m_vectorized_inputs) {
    GenericVirtualListListRef values = params.readonly_vector_input(param_index,
                                                                    this->param_name(param_index));
    for (uint i : mask.indices()) {
      if (vectorization_lengths[i] != 0) {
        vectorization_lengths[i] = std::max<int>(vectorization_lengths[i], values[i].size());
      }
    }
  }

  Vector<GenericVectorArray *> output_vector_arrays;
  for (uint param_index : m_output_indices) {
    GenericVectorArray *vector_array = &params.vector_output(param_index,
                                                             this->param_name(param_index));
    output_vector_arrays.append(vector_array);
  }

  for (uint index : mask.indices()) {
    uint length = vectorization_lengths[index];
    MFParamsBuilder params_builder(m_function, length);

    for (uint param_index : m_function.param_indices()) {
      MFParamType param_type = m_function.param_type(param_index);
      switch (param_type.category()) {
        case MFParamType::ReadonlyVectorInput:
        case MFParamType::VectorOutput:
        case MFParamType::MutableVector: {
          BLI_assert(false);
          break;
        }
        case MFParamType::ReadonlySingleInput: {
          if (m_input_is_vectorized[param_index]) {
            GenericVirtualListListRef input_list_list = params.readonly_vector_input(
                param_index, this->param_name(param_index));
            GenericVirtualListRef repeated_input = input_list_list.repeated_sublist(index, length);
            params_builder.add_readonly_single_input(repeated_input);
          }
          else {
            GenericVirtualListRef input_list = params.readonly_single_input(
                param_index, this->param_name(param_index));
            GenericVirtualListRef repeated_input = input_list.repeated_element(index, length);
            params_builder.add_readonly_single_input(repeated_input);
          }
          break;
        }
        case MFParamType::SingleOutput: {
          GenericVectorArray &output_array_list = params.vector_output(
              param_index, this->param_name(param_index));
          GenericMutableArrayRef output_array = output_array_list.allocate_single(index, length);
          params_builder.add_single_output(output_array);
          break;
        }
      }
    }

    /* TODO: Call with updated context. */
    ArrayRef<uint> sub_mask_indices = IndexRange(length).as_array_ref();
    m_function.call(sub_mask_indices, params_builder, context);
  }
}

MF_ContextVertexPosition::MF_ContextVertexPosition()
{
  MFSignatureBuilder signature("Vertex Position");
  signature.single_output<float3>("Position");
  this->set_signature(signature);
}

void MF_ContextVertexPosition::call(MFMask mask, MFParams params, MFContext context) const
{
  MutableArrayRef<float3> positions = params.uninitialized_single_output<float3>(0, "Position");
  auto vertices_context = context.element_contexts().find_first<VertexPositionArray>();

  if (vertices_context.has_value()) {
    for (uint i : mask.indices()) {
      uint context_index = vertices_context.value().indices[i];
      positions[i] = vertices_context.value().data->positions[context_index];
    }
  }
  else {
    positions.fill_indices(mask.indices(), {0, 0, 0});
  }
}

MF_ContextCurrentFrame::MF_ContextCurrentFrame()
{
  MFSignatureBuilder signature("Current Frame");
  signature.single_output<float>("Frame");
  this->set_signature(signature);
}

void MF_ContextCurrentFrame::call(MFMask mask, MFParams params, MFContext context) const
{
  MutableArrayRef<float> frames = params.uninitialized_single_output<float>(0, "Frame");

  auto time_context = context.element_contexts().find_first<SceneTimeContext>();

  if (time_context.has_value()) {
    float current_frame = time_context.value().data->time;
    frames.fill_indices(mask.indices(), current_frame);
  }
  else {
    frames.fill_indices(mask.indices(), 0.0f);
  }
}

MF_PerlinNoise_3D_to_1D::MF_PerlinNoise_3D_to_1D()
{
  MFSignatureBuilder signature("Perlin Noise 3D to 1D");
  signature.readonly_single_input<float3>("Position");
  signature.readonly_single_input<float>("Amplitude");
  signature.readonly_single_input<float>("Scale");
  signature.single_output<float>("Noise");
  this->set_signature(signature);
}

void MF_PerlinNoise_3D_to_1D::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  VirtualListRef<float3> positions = params.readonly_single_input<float3>(0, "Position");
  VirtualListRef<float> amplitudes = params.readonly_single_input<float>(1, "Amplitude");
  VirtualListRef<float> scales = params.readonly_single_input<float>(2, "Scale");
  MutableArrayRef<float> r_noise = params.uninitialized_single_output<float>(3, "Noise");

  for (uint i : mask.indices()) {
    float3 pos = positions[i];
    float noise = BLI_gNoise(scales[i], pos.x, pos.y, pos.z, false, 1);
    r_noise[i] = noise * amplitudes[i];
  }
}

MF_PerlinNoise_3D_to_3D::MF_PerlinNoise_3D_to_3D()
{
  MFSignatureBuilder signature("Perlin Noise 3D to 3D");
  signature.readonly_single_input<float3>("Position");
  signature.readonly_single_input<float>("Amplitude");
  signature.readonly_single_input<float>("Scale");
  signature.single_output<float3>("Noise");
  this->set_signature(signature);
}

void MF_PerlinNoise_3D_to_3D::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  VirtualListRef<float3> positions = params.readonly_single_input<float3>(0, "Position");
  VirtualListRef<float> amplitudes = params.readonly_single_input<float>(1, "Amplitude");
  VirtualListRef<float> scales = params.readonly_single_input<float>(2, "Scale");
  MutableArrayRef<float3> r_noise = params.uninitialized_single_output<float3>(3, "Noise");

  for (uint i : mask.indices()) {
    float3 pos = positions[i];
    float x = BLI_gNoise(scales[i], pos.x, pos.y, pos.z + 1000.0f, false, 1);
    float y = BLI_gNoise(scales[i], pos.x, pos.y + 1000.0f, pos.z, false, 1);
    float z = BLI_gNoise(scales[i], pos.x + 1000.0f, pos.y, pos.z, false, 1);
    r_noise[i] = float3(x, y, z) * amplitudes[i];
  }
}

MF_ParticleAttribute::MF_ParticleAttribute(StringRef attribute_name, const CPPType &attribute_type)
    : m_attribute_name(attribute_name), m_attribute_type(attribute_type)
{
  MFSignatureBuilder signature("Particle Attribute");
  signature.single_output(attribute_name, attribute_type);
  this->set_signature(signature);
}

void MF_ParticleAttribute::call(MFMask mask, MFParams params, MFContext context) const
{
  auto context_data = context.element_contexts().find_first<ParticleAttributesContext>();
  GenericMutableArrayRef r_output = params.uninitialized_single_output(0, m_attribute_name);

  if (context_data.has_value()) {
    AttributesRef attributes = context_data.value().data->attributes;
    Optional<GenericMutableArrayRef> opt_array = attributes.try_get(m_attribute_name,
                                                                    m_attribute_type);
    if (opt_array.has_value()) {
      GenericMutableArrayRef array = opt_array.value();
      for (uint i : mask.indices()) {
        m_attribute_type.copy_to_uninitialized(array[i], r_output[i]);
      }
      return;
    }
  }

  /* Fallback */
  for (uint i : mask.indices()) {
    m_attribute_type.construct_default(r_output[i]);
  }
}

MF_ClosestPointOnObject::MF_ClosestPointOnObject()
{
  MFSignatureBuilder signature("Closest Point on Object");
  signature.readonly_single_input<Object *>("Object");
  signature.readonly_single_input<float3>("Position");
  signature.single_output<float3>("Closest Point");
  this->set_signature(signature);
}

void MF_ClosestPointOnObject::call(MFMask mask, MFParams params, MFContext context) const
{
  auto context_data = context.element_contexts().find_first<ExternalObjectBVHTreesContext>();

  VirtualListRef<Object *> objects = params.readonly_single_input<Object *>(0, "Object");
  VirtualListRef<float3> positions = params.readonly_single_input<float3>(1, "Position");
  MutableArrayRef<float3> r_points = params.uninitialized_single_output<float3>(2,
                                                                                "Closest Point");

  if (!context_data.has_value()) {
    r_points.fill_indices(mask.indices(), {0, 0, 0});
    return;
  }

  for (uint i : mask.indices()) {
    Object *object = objects[i];
    if (object == nullptr) {
      r_points[i] = {0, 0, 0};
      continue;
    }

    BVHTreeFromMesh *bvhtree = context_data.value().data->get_bvh_tree(object);
    if (bvhtree == nullptr) {
      r_points[i] = {0, 0, 0};
      continue;
    }

    BVHTreeNearest nearest = {0};
    nearest.dist_sq = 10000000.0f;
    nearest.index = -1;
    BLI_bvhtree_find_nearest(
        bvhtree->tree, positions[i], &nearest, bvhtree->nearest_callback, (void *)bvhtree);

    if (nearest.index == -1) {
      r_points[i] = {0, 0, 0};
      continue;
    }

    float4x4 local_to_world_matrix = object->obmat;
    r_points[i] = local_to_world_matrix.transform_position(nearest.co);
  }
}

MF_MapRange::MF_MapRange()
{
  MFSignatureBuilder signature("Map Range");
  signature.readonly_single_input<float>("Value");
  signature.readonly_single_input<float>("From Min");
  signature.readonly_single_input<float>("From Max");
  signature.readonly_single_input<float>("To Min");
  signature.readonly_single_input<float>("To Max");
  signature.single_output<float>("Value");
  this->set_signature(signature);
}

void MF_MapRange::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  VirtualListRef<float> values = params.readonly_single_input<float>(0, "Value");
  VirtualListRef<float> from_min = params.readonly_single_input<float>(1, "From Min");
  VirtualListRef<float> from_max = params.readonly_single_input<float>(2, "From Max");
  VirtualListRef<float> to_min = params.readonly_single_input<float>(3, "To Min");
  VirtualListRef<float> to_max = params.readonly_single_input<float>(4, "To Max");
  MutableArrayRef<float> r_values = params.uninitialized_single_output<float>(5, "Value");

  for (uint i : mask.indices()) {
    float diff = from_max[i] - from_min[i];
    if (diff != 0.0f) {
      r_values[i] = (values[i] - from_min[i]) / diff * (to_max[i] - to_min[i]) + to_min[i];
    }
    else {
      r_values[i] = to_min[i];
    }
  }
}

MF_Clamp::MF_Clamp()
{
  MFSignatureBuilder signature("Clamp");
  signature.readonly_single_input<float>("Value");
  signature.readonly_single_input<float>("Min");
  signature.readonly_single_input<float>("Max");
  signature.single_output<float>("Value");
  this->set_signature(signature);
}

void MF_Clamp::call(MFMask mask, MFParams params, MFContext UNUSED(context)) const
{
  VirtualListRef<float> values = params.readonly_single_input<float>(0, "Value");
  VirtualListRef<float> min_values = params.readonly_single_input<float>(1, "Min");
  VirtualListRef<float> max_values = params.readonly_single_input<float>(2, "Max");
  MutableArrayRef<float> r_values = params.uninitialized_single_output<float>(3, "Value");

  for (uint i : mask.indices()) {
    float value = std::min(std::max(values[i], min_values[i]), max_values[i]);
    r_values[i] = value;
  }
}

}  // namespace FN
