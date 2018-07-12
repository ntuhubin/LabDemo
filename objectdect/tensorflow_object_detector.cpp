#include "tensorflow_object_detector.h"

#include "tensorflow/cc/ops/const_op.h"
#include "tensorflow/cc/ops/image_ops.h"
#include "tensorflow/cc/ops/standard_ops.h"

using tensorflow::int32;
using tensorflow::uint8;

namespace michi_object_detector {

bool ObjectDetector::LoadGraph(const string& frozen_graph_path) {
  tensorflow::GraphDef graph_def;
  tensorflow::SessionOptions opts;

  // Set GPU options
  // tensorflow::graph::SetDefaultDevice("/gpu:0", &graph_def);
  opts.config.mutable_gpu_options()->set_per_process_gpu_memory_fraction(0.4);
  opts.config.mutable_gpu_options()->set_allow_growth(true);

  Status load_graph_status =
    ReadBinaryProto(tensorflow::Env::Default(), frozen_graph_path, &graph_def);
  if (!load_graph_status.ok()) {
    LOG(ERROR) << "Failed to load compute graph : " << frozen_graph_path;
    return false;
  }
  detect_session.reset(tensorflow::NewSession(tensorflow::SessionOptions(opts)));
  Status session_create_status = detect_session->Create(graph_def);
  if (!session_create_status.ok()) {
    LOG(ERROR) << "Failed to create graph : ";
    return false;
  }
  return true;
}

bool ObjectDetector::Detect(const Mat &input_img,
                            DetectedObjectGroup* detected_object_group,
                      const float score_threshold,
                            const int max_num_detections) const {
  const int height = input_img.rows;
  const int width = input_img.cols;
  const int depth = input_img.channels();

  Tensor input_tensor(tensorflow::DT_UINT8,
          tensorflow::TensorShape({1, height, width, depth}));
  auto input_tensor_mapped = input_tensor.tensor<unsigned char, 4>();
  unsigned char* input_img_data_ptr = (unsigned char*)(input_img.data);
  for (int y = 0; y < height; ++y) {
    unsigned char* input_img_row = input_img_data_ptr + (y * width * depth);
    for (int x = 0; x < width; ++x) {
        unsigned char* input_img_pixel = input_img_row + (x * depth);
        for (int c = 0; c < depth; ++c) {
           unsigned char* source_value = input_img_pixel + c;
           input_tensor_mapped(0, y, x, c) = *source_value;
        }
    }
  }

  string input_layer = "image_tensor:0";
  string output_location_layer = "detection_boxes:0";
  string output_score_layer = "detection_scores:0";
  string output_class_layer = "detection_classes:0";

  // Actually run the image through the model.
  std::vector<Tensor> outputs;
  Status run_status =
      detect_session->Run({{input_layer, input_tensor}},
        {output_score_layer, output_location_layer,
               output_class_layer}, {}, &outputs);
  if (!run_status.ok()) {
    LOG(ERROR) << "Running model failed: " << run_status;
    return false;
  }

  Status store_detection_status =
      StoreTopDetections(outputs, width, height, detected_object_group,
                         max_num_detections, score_threshold);
  if (!store_detection_status.ok()) {
    LOG(ERROR) << "Store detection results failed: " << store_detection_status;
    return false;
  }

  return true;
}

Status ObjectDetector::GetTopDetections(const vector<Tensor>& outputs,
                                        Tensor* indices, Tensor* scores,
                      const int max_num_detections) const {
  auto root = tensorflow::Scope::NewRootScope();
  using namespace ::tensorflow::ops;  // NOLINT(build/namespaces)

  string output_name = "top_k";
  TopK(root.WithOpName(output_name), outputs[0], max_num_detections);
  // This runs the GraphDef network definition that we've just constructed, and
  // returns the results in the output tensors.
  tensorflow::GraphDef graph;
  TF_RETURN_IF_ERROR(root.ToGraphDef(&graph));

  std::unique_ptr<tensorflow::Session> session(
      tensorflow::NewSession(tensorflow::SessionOptions()));
  TF_RETURN_IF_ERROR(session->Create(graph));
  // The TopK node returns two outputs, the scores and their original indices,
  // so we have to append :0 and :1 to specify them both.
  std::vector<Tensor> out_tensors;
  TF_RETURN_IF_ERROR(session->Run({}, {output_name + ":0", output_name + ":1"},
                                  {}, &out_tensors));
  *scores = out_tensors[0];
  *indices = out_tensors[1];
  return Status::OK();
}

Status ObjectDetector::StoreTopDetections(
                           const vector<Tensor>& outputs,
                           const int image_width, const int image_height,
                           DetectedObjectGroup* detected_object_group,
                           const int max_num_detections,
                           const float score_threshold) const {
  Tensor indices;
  Tensor scores;
  TF_RETURN_IF_ERROR(
      GetTopDetections(outputs, &indices, &scores, max_num_detections));

  tensorflow::TTypes<float>::Flat scores_flat = scores.flat<float>();

  tensorflow::TTypes<int32>::Flat indices_flat = indices.flat<int32>();

  const Tensor& locations = outputs[1];
  auto locations_flat = locations.flat<float>();

  const Tensor& object_ids = outputs[2];
  LOG(INFO) << object_ids.DebugString();
  auto object_ids_flat = object_ids.flat<float>();

  LOG(INFO) << "===== Top " << max_num_detections << " Detections ======";
  LOG(INFO) << "===Detection score threshold :" << score_threshold << "===";
  detected_object_group->num_of_object = 0;
  detected_object_group->detected_objects.clear();
  for (int pos = 0; pos < max_num_detections; ++pos) {
    const int label_index = indices_flat(pos);
    const float score = scores_flat(pos);
    if (score < score_threshold) {
  continue;
    }
    const float *location = &locations_flat(label_index * 4);
    float left = location[1] * image_width;
    float top = location[0] * image_height;
    float right = location[3] * image_width;
    float bottom = location[2] * image_height;
    LOG(INFO) << "Detection " << pos << ": "
              << "Left:" << left << " "
              << "Top:" << top << " "
              << "Right:" << right << " "
              << "Bottom:" << bottom << " "
              << "Object Id: " << object_ids_flat(label_index)  << " "
              << "(" << label_index << ") score: " << score;
    DetectedObject detect_object;
    ColorValues color_values;
    color_values.red = 0;
    color_values.green = 0;
    color_values.blue = 0;

    detect_object.color_values = color_values;
    detect_object.object_id = object_ids_flat(label_index);
    detect_object.score = score;
    detect_object.left = location[1] * image_width;
    detect_object.top = location[0] * image_height;
    detect_object.width = (location[3] - location[1]) * image_width;
    detect_object.height = (location[2] - location[0]) * image_height;
    detected_object_group->detected_objects.push_back(detect_object);
    detected_object_group->num_of_object++;
  }

  return Status::OK();
}

ObjectDetector::~ObjectDetector() {
  if (detect_session != nullptr) {
    detect_session->Close();
  }
}
}  // namespace haohan_object_detector
