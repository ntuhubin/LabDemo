#include "object_detection_wrapper.h"
#include "tensorflow_object_detector.h"

static bool init_object = false;
michi_object_detector::ObjectDetector object_detector;

bool Detection(cv::Mat &input_img, DetectedObjectGroup* detected_object_group,
               const string& frozen_graph_path, DetectorType detector_type,
               const float score_threshold,
               const int max_num_detections) {
  if (detector_type != HUMAN) {
    cout << "Current only support object detection." << endl;
    return false;
  } else {
    if (init_object == false) {
      cout << "object model is not loaded yet." << endl
           << "Loading model..." << endl;
      object_detector.LoadGraph(frozen_graph_path);
      cout << "Loading model finished." << endl;
      init_object = true;
    }
    if (!object_detector.Detect(input_img, detected_object_group,
                              score_threshold, max_num_detections)) {
      cout << "Detection failed." << endl;
      return false;
    }
    cout << "Detection finished successfully." << endl;
    return true;
  }
}
