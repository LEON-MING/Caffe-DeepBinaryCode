#include <vector>

#include "caffe/layer.hpp"
#include "caffe/util/io.hpp"
#include "caffe/util/math_functions.hpp"
#include "caffe/vision_layers.hpp"

namespace caffe {

template <typename Dtype>
void K2_EuclideanLossLayer<Dtype>::Reshape(
  const vector<Blob<Dtype>*>& bottom, vector<Blob<Dtype>*>* top) {
  LossLayer<Dtype>::Reshape(bottom, top);
  CHECK_EQ(bottom[0]->channels(), bottom[1]->channels());
  CHECK_EQ(bottom[0]->height(), bottom[1]->height());
  CHECK_EQ(bottom[0]->width(), bottom[1]->width());
  diff_.Reshape(bottom[0]->num(), bottom[0]->channels(),
      bottom[0]->height(), bottom[0]->width());
}

template <typename Dtype>
void K2_EuclideanLossLayer<Dtype>::Forward_cpu(const vector<Blob<Dtype>*>& bottom,
    vector<Blob<Dtype>*>* top) {

  int count = bottom[0]->count();
  const Dtype* bottom_data = bottom[0]->cpu_data();
  Dtype loss = 0;
  Dtype sum = 0;
  Dtype avg = 0;
  for (int i = 0; i < count; ++i) {
	sum = sum + bottom_data[i];
  } 
  avg = sum / count;
  loss = (avg - Dtype(0.5)) * (avg - Dtype(0.5));
  (*top)[0]->mutable_cpu_data()[0] = loss;


  diff_.mutable_cpu_data()[0] = avg - Dtype(0.5);

/*
  int count = bottom[0]->count();
  caffe_sub(
      count,
      bottom[0]->cpu_data(),
      bottom[1]->cpu_data(),
      diff_.mutable_cpu_data());
  Dtype dot = caffe_cpu_dot(count, diff_.cpu_data(), diff_.cpu_data());
  Dtype loss = dot / bottom[0]->num() / Dtype(2);
  (*top)[0]->mutable_cpu_data()[0] = loss;
*/
}

template <typename Dtype>
void K2_EuclideanLossLayer<Dtype>::Backward_cpu(const vector<Blob<Dtype>*>& top,
    const vector<bool>& propagate_down, vector<Blob<Dtype>*>* bottom) {
  for (int i = 0; i < 2; ++i) {
    if (propagate_down[i]) {
      const Dtype sign = (i == 0) ? 1 : -1;
      const Dtype alpha = sign * top[0]->cpu_diff()[0];
      Dtype result = (alpha * Dtype(2) * diff_.cpu_data()[0] / (*bottom)[i]->count());
      for (int j = 0; j < (*bottom)[i]->count(); j++)
      	(*bottom)[i]->mutable_cpu_diff()[j] = result; 
    }
  }
}

#ifdef CPU_ONLY
STUB_GPU(K2_EuclideanLossLayer);
#endif

INSTANTIATE_CLASS(K2_EuclideanLossLayer);

}  // namespace caffe
