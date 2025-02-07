#include "dlr.h"

#include <gtest/gtest.h>

#include "dlr_common.h"
#include "test_utils.hpp"

DLRModelHandle GetDLRModel() {
  DLRModelHandle model = nullptr;
  const char* model_path = "./resnet_v1_5_50";
  int device_type = 1;  // cpu;
  if (CreateDLRModel(&model, model_path, device_type, 0) != 0) {
    LOG(INFO) << DLRGetLastError() << std::endl;
    throw std::runtime_error("Could not load DLR Model");
  }
  return model;
}

TEST(DLR, TestGetDLRDeviceType) {
  const char* model_path = "./resnet_v1_5_50";
  EXPECT_EQ(GetDLRDeviceType(model_path), -1);
}

TEST(DLR, TestGetDLRNumInputs) {
  auto model = GetDLRModel();
  int num_inputs;
  EXPECT_EQ(GetDLRNumInputs(&model, &num_inputs), 0);
  EXPECT_EQ(num_inputs, 1);
  DeleteDLRModel(&model);
}

TEST(DLR, TestGetDLRNumWeights) {
  auto model = GetDLRModel();
  int num_weights;
  EXPECT_EQ(GetDLRNumWeights(&model, &num_weights), 0);
  EXPECT_EQ(num_weights, 108);
  DeleteDLRModel(&model);
}

TEST(DLR, TestGetDLRInputName) {
  auto model = GetDLRModel();
  const char* input_name;
  EXPECT_EQ(GetDLRInputName(&model, 0, &input_name), 0);
  EXPECT_STREQ(input_name, "input_tensor");
  DeleteDLRModel(&model);
}

TEST(DLR, TestGetDLRInputType) {
  auto model = GetDLRModel();
  const char* input_type;
  EXPECT_EQ(GetDLRInputType(&model, 0, &input_type), 0);
  EXPECT_STREQ(input_type, "float32");
  DeleteDLRModel(&model);
}

TEST(DLR, TestGetDLRWeightName) {
  auto model = GetDLRModel();
  const char* weight_name;
  EXPECT_EQ(GetDLRWeightName(&model, 0, &weight_name), 0);
  EXPECT_STREQ(weight_name, "p45");
  EXPECT_EQ(GetDLRWeightName(&model, 107, &weight_name), 0);
  EXPECT_STREQ(weight_name, "p72");
  DeleteDLRModel(&model);
}

TEST(DLR, TestSetDLRInput) {
  auto model = GetDLRModel();
  size_t img_size = 224 * 224 * 3;
  std::vector<float> img = LoadImageAndPreprocess("cat224-3.txt", img_size, 1);
  int64_t shape[4] = {1, 224, 224, 3};
  const char* input_name = "input_tensor";
  std::vector<float> img2(img_size);
  EXPECT_EQ(SetDLRInput(&model, input_name, shape, img.data(), 4), 0);
  EXPECT_EQ(GetDLRInput(&model, input_name, img2.data()), 0);
  EXPECT_EQ(img, img2);
  DeleteDLRModel(&model);
}

TEST(DLR, TestGetDLRInputShape) {
  auto model = GetDLRModel();
  int64_t input_size;
  int input_dim;
  int index = 0;
  EXPECT_EQ(GetDLRInputSizeDim(&model, 0, &input_size, &input_dim), 0);
  EXPECT_EQ(input_dim, 4);
  EXPECT_EQ(input_size, 1 * 224 * 224 * 3);
  std::vector<int64_t> shape(input_dim);
  EXPECT_EQ(GetDLRInputShape(&model, 0, shape.data()), 0);
  EXPECT_EQ(shape[0], 1);
  EXPECT_EQ(shape[1], 224);
  EXPECT_EQ(shape[2], 224);
  EXPECT_EQ(shape[3], 3);
  DeleteDLRModel(&model);
}

TEST(DLR, TestGetDLROutputShape) {
  auto model = GetDLRModel();
  int64_t output_size;
  int output_dim;
  int index = 0;
  // output 0
  EXPECT_EQ(GetDLROutputSizeDim(&model, 0, &output_size, &output_dim), 0);
  EXPECT_EQ(output_dim, 1);
  EXPECT_EQ(output_size, 1);
  std::vector<int64_t> shape0(output_dim);
  EXPECT_EQ(GetDLROutputShape(&model, 0, shape0.data()), 0);
  EXPECT_EQ(shape0[0], 1);
  // output 1
  EXPECT_EQ(GetDLROutputSizeDim(&model, 1, &output_size, &output_dim), 0);
  EXPECT_EQ(output_dim, 2);
  EXPECT_EQ(output_size, 1001);
  std::vector<int64_t> shape1(output_dim);
  EXPECT_EQ(GetDLROutputShape(&model, 1, shape1.data()), 0);
  EXPECT_EQ(shape1[0], 1);
  EXPECT_EQ(shape1[1], 1001);
  DeleteDLRModel(&model);
}

TEST(DLR, TestGetDLRNumOutputs) {
  auto model = GetDLRModel();
  int num_outputs;
  EXPECT_EQ(GetDLRNumOutputs(&model, &num_outputs), 0);
  EXPECT_EQ(num_outputs, 2);
  DeleteDLRModel(&model);
}

TEST(DLR, TestGetDLROutputType) {
  auto model = GetDLRModel();
  // output 0
  const char* output_type0;
  EXPECT_EQ(GetDLROutputType(&model, 0, &output_type0), 0);
  EXPECT_STREQ(output_type0, "int32");
  // output 1
  const char* output_type1;
  EXPECT_EQ(GetDLROutputType(&model, 1, &output_type1), 0);
  EXPECT_STREQ(output_type1, "float32");
  DeleteDLRModel(&model);
}

TEST(DLR, GetDLRHasMetadata) {
  auto model = GetDLRModel();
  bool has_metadata;
  EXPECT_EQ(GetDLRHasMetadata(&model, &has_metadata), 0);
  EXPECT_TRUE(has_metadata);
  DeleteDLRModel(&model);
}

TEST(DLR, TestGetDLROutputName) {
  auto model = GetDLRModel();
  // output 0
  const char* output_name0;
  EXPECT_EQ(GetDLROutputName(&model, 0, &output_name0), 0);
  EXPECT_STREQ(output_name0, "ArgMax:0");
  // output 1
  const char* output_name1;
  EXPECT_EQ(GetDLROutputName(&model, 1, &output_name1), 0);
  EXPECT_STREQ(output_name1, "softmax_tensor:0");
  DeleteDLRModel(&model);
}

TEST(DLR, TestGetDLROutputIndex) {
  auto model = GetDLRModel();
  // output 0
  int output_index0;
  EXPECT_EQ(GetDLROutputIndex(&model, "ArgMax:0", &output_index0), 0);
  EXPECT_EQ(output_index0, 0);
  // output 1
  int output_index1;
  EXPECT_EQ(GetDLROutputIndex(&model, "softmax_tensor:0", &output_index1), 0);
  EXPECT_EQ(output_index1, 1);
  DeleteDLRModel(&model);
}

TEST(DLR, TestGetDLRBackend) {
  auto model = GetDLRModel();
  const char* backend;
  EXPECT_EQ(GetDLRBackend(&model, &backend), 0);
  EXPECT_STREQ(backend, "tvm");
  DeleteDLRModel(&model);
}

TEST(DLR, TestRunDLRModel_GetDLROutput) {
  auto model = GetDLRModel();
  size_t img_size = 224 * 224 * 3;
  std::vector<float> img = LoadImageAndPreprocess("cat224-3.txt", img_size, 1);
  int64_t shape[4] = {1, 224, 224, 3};
  const char* input_name = "input_tensor";
  EXPECT_EQ(SetDLRInput(&model, input_name, shape, img.data(), 4), 0);
  EXPECT_EQ(RunDLRModel(&model), 0);
  // output 0
  int output0[1];
  EXPECT_EQ(GetDLROutput(&model, 0, output0), 0);
  EXPECT_EQ(output0[0], 112);
  EXPECT_EQ(GetDLROutputByName(&model, "ArgMax:0", output0), 0);
  EXPECT_EQ(output0[0], 112);
  const void* output0_p;
  EXPECT_EQ(GetDLROutputPtr(&model, 0, &output0_p), 0);
  EXPECT_EQ(((int*)output0_p)[0], 112);
  // output 1
  float output1[1001];
  EXPECT_EQ(GetDLROutput(&model, 1, output1), 0);
  EXPECT_GT(output1[112], 0.01);
  EXPECT_EQ(GetDLROutputByName(&model, "softmax_tensor:0", output1), 0);
  EXPECT_GT(output1[112], 0.01);
  float* output1_p;
  EXPECT_EQ(GetDLROutputPtr(&model, 1, (const void**)&output1_p), 0);
  EXPECT_GT(output1_p[112], 0.01);
  for (int i = 0; i < 1001; i++) {
    EXPECT_EQ(output1_p[i], output1[i]);
  }
  DeleteDLRModel(&model);
}

TEST(DLR, TestCreateFromPaths_TVM) {
  DLRModelHandle model = nullptr;
  const char* model_paths =
      "./resnet_v1_5_50/compiled.so:./resnet_v1_5_50/compiled_model.json:./resnet_v1_5_50/"
      "compiled.params";
  int device_type = 1;  // cpu;
  if (CreateDLRModel(&model, model_paths, device_type, 0) != 0) {
    LOG(INFO) << DLRGetLastError() << std::endl;
    throw std::runtime_error("Could not load DLR Model");
  }

  // set input
  int64_t shape[4] = {1, 224, 224, 3};
  DLTensor input = GetInputDLTensor(4, shape, "cat224-3.txt");
  EXPECT_EQ(SetDLRInputTensor(&model, "input_tensor", &input), 0);

  // run inference
  EXPECT_EQ(RunDLRModel(&model), 0);

  // output 0
  int output0_d[1];
  EXPECT_EQ(GetDLROutput(&model, 0, output0_d), 0);
  EXPECT_EQ(output0_d[0], 112);
  int64_t output0_shape[1] = {1};
  DLTensor output0 = GetEmptyDLTensor(1, output0_shape, kDLInt, 32);
  EXPECT_EQ(GetDLROutputTensor(&model, 0, &output0), 0);
  EXPECT_EQ(((int*)(output0.data))[0], 112);

  // output 1
  float output1_d[1001];
  EXPECT_EQ(GetDLROutput(&model, 1, output1_d), 0);
  EXPECT_GT(output1_d[112], 0.01);
  int64_t output1_shape[2] = {1, 1001};
  DLTensor output1 = GetEmptyDLTensor(2, output1_shape, kDLFloat, 32);
  EXPECT_EQ(GetDLROutputTensor(&model, 1, &output1), 0);
  EXPECT_GT(((float*)(output1.data))[112], 0.01);
  DLManagedTensor* output1_p;
  EXPECT_EQ(GetDLROutputManagedTensorPtr(&model, 1, (const void**)&output1_p), 0);
  EXPECT_GT(((float*)(output1_p->dl_tensor.data))[112], 0.01);
  for (int i = 0; i < 1001; i++) {
    EXPECT_EQ(((float*)(output1_p->dl_tensor.data))[i], ((float*)(output1.data))[i]);
  }

  output1_p->deleter(output1_p);
  DeleteDLTensor(output0);
  DeleteDLTensor(output1);
  DeleteDLTensor(input);
  DeleteDLRModel(&model);
}

TEST(DLR, TestCreateFromPaths_RelayVM) {
  DLRModelHandle model = nullptr;
  const char* model_paths =
      "./ssd_mobilenet_v1/compiled.so:./ssd_mobilenet_v1/compiled.meta:./ssd_mobilenet_v1/code.ro";
  int device_type = 1;  // cpu;
  if (CreateDLRModel(&model, model_paths, device_type, 0) != 0) {
    LOG(INFO) << DLRGetLastError() << std::endl;
    throw std::runtime_error("Could not load DLR Model");
  }

  // set input
  int64_t input_shape[4] = {1, 512, 512, 3};
  DLTensor input = GetEmptyDLTensor(4, input_shape, kDLUInt, 8);
  const char* input_name = "image_tensor";
  EXPECT_EQ(SetDLRInputTensor(&model, input_name, &input), 0);

  // run inference
  EXPECT_EQ(RunDLRModel(&model), 0);

  // output 3
  float output3_d[100];
  EXPECT_EQ(GetDLROutput(&model, 3, output3_d), 0);
  int64_t output3_size;
  int output3_dim;
  EXPECT_EQ(GetDLROutputSizeDim(&model, 3, &output3_size, &output3_dim), 0);
  std::vector<int64_t> output3_shape(output3_dim);
  EXPECT_EQ(GetDLROutputShape(&model, 3, output3_shape.data()), 0);
  DLTensor output3 = GetEmptyDLTensor(output3_dim, output3_shape.data(), kDLFloat, 32);
  EXPECT_EQ(GetDLROutputTensor(&model, 3, &output3), 0);
  for (int i = 0; i < output3_size; i++) {
    EXPECT_EQ(((float*)(output3.data))[i], output3_d[i]);
  }
  DLManagedTensor* output3_p;
  EXPECT_EQ(GetDLROutputManagedTensorPtr(&model, 3, (const void**)&output3_p), 0);
  for (int i = 0; i < output3_size; i++) {
    EXPECT_EQ(((float*)(output3_p->dl_tensor.data))[i], ((float*)(output3.data))[i]);
  }

  output3_p->deleter(output3_p);
  DeleteDLTensor(output3);
  DeleteDLTensor(input);
  DeleteDLRModel(&model);
}

TEST(DLR, TestMakePathVecForSingleWindowsPath) {
  std::string windows_path = "C:\\a\\b\\c:";
  std::vector<std::string> path_vec = dlr::MakePathVec(windows_path);
  EXPECT_EQ(path_vec.size(), 1);
  EXPECT_EQ(path_vec[0], "C:\\a\\b\\c");
}

TEST(DLR, TestMakePathVecForMultipleWindowsPaths) {
  std::string windows_path = "C:\\a\\b\\c:D:\\a\\b\\c";
  std::vector<std::string> path_vec = dlr::MakePathVec(windows_path);
  EXPECT_EQ(path_vec[0], "C:\\a\\b\\c");
  EXPECT_EQ(path_vec[1], "D:\\a\\b\\c");
}

TEST(DLR, TestMakePathVecForSingleLinuxPath) {
  std::string linux_path = "/a/b/c";
  std::vector<std::string> path_vec = dlr::MakePathVec(linux_path);
  EXPECT_EQ(path_vec.size(), 1);
  EXPECT_EQ(path_vec[0], "/a/b/c");
  linux_path = "/a/b/c:";
  path_vec = dlr::MakePathVec(linux_path);
  EXPECT_EQ(path_vec.size(), 1);
  EXPECT_EQ(path_vec[0], "/a/b/c");
}

TEST(DLR, TestMakePathVecForMultipleSingleCharacterLinuxPaths) {
  std::string linux_path = "ab:b:c";
  std::vector<std::string> path_vec = dlr::MakePathVec(linux_path);
  EXPECT_EQ(path_vec.size(), 3);
  EXPECT_EQ(path_vec[0], "ab");
  EXPECT_EQ(path_vec[1], "b");
  EXPECT_EQ(path_vec[2], "c");
}

TEST(DLR, TestMakePathVecForMultipleLinuxPaths) {
  std::string linux_path = "/a/b/c:/d/e/f";
  std::vector<std::string> path_vec = dlr::MakePathVec(linux_path);
  EXPECT_EQ(path_vec.size(), 2);
  EXPECT_EQ(path_vec[0], "/a/b/c");
  EXPECT_EQ(path_vec[1], "/d/e/f");
}

TEST(DLR, TestSetInputTensorZeroCopy_TVM) {
  auto model = GetDLRModel();
  // set input
  int64_t shape[4] = {1, 224, 224, 3};
  DLTensor input = GetInputDLTensor(4, shape, "cat224-3.txt");
  EXPECT_EQ(SetDLRInputTensorZeroCopy(&model, "input_tensor", &input), 0);

  // run inference
  EXPECT_EQ(RunDLRModel(&model), 0);

  // output 0
  int output0_d[1];
  EXPECT_EQ(GetDLROutput(&model, 0, output0_d), 0);
  EXPECT_EQ(output0_d[0], 112);
  int64_t output0_shape[1] = {1};
  DLTensor output0 = GetEmptyDLTensor(1, output0_shape, kDLInt, 32);
  EXPECT_EQ(GetDLROutputTensor(&model, 0, &output0), 0);
  EXPECT_EQ(((int*)(output0.data))[0], 112);

  // output 1
  float output1_d[1001];
  EXPECT_EQ(GetDLROutput(&model, 1, output1_d), 0);
  EXPECT_GT(output1_d[112], 0.01);
  int64_t output1_shape[2] = {1, 1001};
  DLTensor output1 = GetEmptyDLTensor(2, output1_shape, kDLFloat, 32);
  EXPECT_EQ(GetDLROutputTensor(&model, 1, &output1), 0);
  EXPECT_GT(((float*)(output1.data))[112], 0.01);
  DLManagedTensor* output1_p;
  EXPECT_EQ(GetDLROutputManagedTensorPtr(&model, 1, (const void**)&output1_p), 0);
  EXPECT_GT(((float*)(output1_p->dl_tensor.data))[112], 0.01);
  for (int i = 0; i < 1001; i++) {
    EXPECT_EQ(((float*)(output1_p->dl_tensor.data))[i], ((float*)(output1.data))[i]);
  }

  output1_p->deleter(output1_p);
  DeleteDLTensor(output0);
  DeleteDLTensor(output1);
  DeleteDLTensor(input);
  DeleteDLRModel(&model);
}

TEST(DLR, TestDLRInputOrder) {
  DLRModelHandle model = nullptr;
  const char* model_path = "./input_order";
  int device_type = 1;  // cpu;
  if (CreateDLRModel(&model, model_path, device_type, 0) != 0) {
    LOG(INFO) << DLRGetLastError() << std::endl;
    throw std::runtime_error("Could not load DLR Model");
  }
  const char* input_names[4];
  const char* input_types[4];
  for (int i = 0; i < 4; ++i) {
    EXPECT_EQ(GetDLRInputName(&model, i, &input_names[i]), 0);
    EXPECT_EQ(GetDLRInputType(&model, i, &input_types[i]), 0);
  }
  EXPECT_STREQ(input_names[0], "image");
  EXPECT_STREQ(input_names[1], "transform");
  EXPECT_STREQ(input_names[2], "bbox");
  EXPECT_STREQ(input_names[3], "index");
  EXPECT_STREQ(input_types[0], "float32");
  EXPECT_STREQ(input_types[1], "float32");
  EXPECT_STREQ(input_types[2], "float32");
  EXPECT_STREQ(input_types[3], "int32");
  DeleteDLRModel(&model);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
#ifndef _WIN32
  testing::FLAGS_gtest_death_test_style = "threadsafe";
#endif  // _WIN32
  return RUN_ALL_TESTS();
}
