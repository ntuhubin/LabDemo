#include "object_detection_wrapper.h"
#include "tensorflow_object_detector.h"

static bool init_truck = false;
haohan_object_detector::ObjectDetector truck_detector;

bool Detection(cv::Mat &input_img, DetectedObjectGroup* detected_object_group, 
               const string& frozen_graph_path, DetectorType detector_type,
               const float score_threshold, 
               const int max_num_detections) {
  if (detector_type != TRUCK) {
    cout << "Current only support truck detection." << endl;
    return false;
  } else {
    if (init_truck == false) {
      cout << "truck model is not loaded yet." << endl
           << "Loading model..." << endl;
      truck_detector.LoadGraph(frozen_graph_path);
      cout << "Loading model finished." << endl;
      init_truck = true;
    }
    if (!truck_detector.Detect(input_img, detected_object_group, 
                              score_threshold, max_num_detections)) {
      cout << "Detection failed." << endl;
      return false;
    }
    cout << "Detection finished successfully." << endl;
    return true;  
  }
}          
