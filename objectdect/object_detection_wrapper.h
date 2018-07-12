#ifndef OBJECT_DETECTION_WRAPPER_H_
#define OBJECT_DETECTION_WRAPPER_H_

#include <opencv2/core/core.hpp>

using namespace std;

enum DetectorType {HUMAN = 0};

struct ColorValues {
  int red;
  int green;
  int blue;
};

struct DetectedObject {
  int object_id;
  int left;
  int top;
  int width;
  int height;
  float score;
  ColorValues color_values;
  DetectedObject() {
    object_id = 0;
    left = 0;
    top = 0;
    width = 0;
    height = 0;
    score = 0.f;
  }
};

struct DetectedObjectGroup {
  int num_of_object;
  vector<DetectedObject> detected_objects;
  DetectedObjectGroup() {
    num_of_object = 0;
  }
};

bool Detection(cv::Mat &input_img, DetectedObjectGroup* detected_object_group,
               const string& frozen_graph_path, DetectorType detector_type,
               const float score_threshold = 0.5,
               const int max_num_detections = 10);

#endif // OBJECT_DETECTION_WRAPPER_H_
