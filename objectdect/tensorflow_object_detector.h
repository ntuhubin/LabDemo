#ifndef TENSORFLOW_OBJECT_DETECTOR_H_
#define TENSORFLOW_OBJECT_DETECTOR_H_

#include "object_detection_wrapper.h"
#include <opencv2/core/core.hpp>
#include "tensorflow/core/public/session.h"

using namespace cv;
using namespace std;

using tensorflow::Tensor;
using tensorflow::Status;

namespace michi_object_detector {

class ObjectDetector {
  public:
    bool LoadGraph(const string& frozen_graph_path);
    bool Detect(const Mat &input_img,
        DetectedObjectGroup* detected_object_group,
        const float score_threshold = 0.7,
                const int max_num_detections = 10) const;
    ~ObjectDetector();
  private:
    unique_ptr<tensorflow::Session> detect_session;
    Status GetTopDetections(const vector<Tensor>& outputs,
                            Tensor* indices, Tensor* scores,
                            const int max_num_detections = 10) const;
    Status StoreTopDetections(const vector<Tensor>& outputs,
                              const int image_width, const int image_height,
                              DetectedObjectGroup* detected_object_group,
                              const int max_num_detections = 10,
                              const float score_threshold = 0.7) const;
};

}  // namespace haohan_object_detector

#endif // TENSORFLOW_OBJECT_DETECTOR_H_
