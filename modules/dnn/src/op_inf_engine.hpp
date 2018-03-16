// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Copyright (C) 2018, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.

#ifndef __OPENCV_DNN_OP_INF_ENGINE_HPP__
#define __OPENCV_DNN_OP_INF_ENGINE_HPP__

#ifdef HAVE_INF_ENGINE
#include <inference_engine.hpp>
#endif  // HAVE_INF_ENGINE

namespace cv { namespace dnn {

#ifdef HAVE_INF_ENGINE

class InfEngineBackendNet : public InferenceEngine::ICNNNetwork
{
public:
    InfEngineBackendNet();

    InfEngineBackendNet(InferenceEngine::CNNNetwork& net);

    virtual void Release() noexcept;

    virtual InferenceEngine::Precision getPrecision() noexcept;

    virtual void getOutputsInfo(InferenceEngine::OutputsDataMap &out) noexcept;

    virtual void getInputsInfo(InferenceEngine::InputsDataMap &inputs) noexcept;

    virtual void getInputsInfo(InferenceEngine::InputsDataMap &inputs) const noexcept;

    virtual InferenceEngine::InputInfo::Ptr getInput(const std::string &inputName) noexcept;

    virtual void getName(char *pName, size_t len) noexcept;

    virtual size_t layerCount() noexcept;

    virtual InferenceEngine::DataPtr& getData(const char *dname) noexcept;

    virtual void addLayer(const InferenceEngine::CNNLayerPtr &layer) noexcept;

    virtual InferenceEngine::StatusCode addOutput(const std::string &layerName,
                                                  size_t outputIndex = 0,
                                                  InferenceEngine::ResponseDesc *resp = nullptr) noexcept;

    virtual InferenceEngine::StatusCode getLayerByName(const char *layerName,
                                                       InferenceEngine::CNNLayerPtr &out,
                                                       InferenceEngine::ResponseDesc *resp) noexcept;

    virtual void setTargetDevice(InferenceEngine::TargetDevice device) noexcept;

    virtual InferenceEngine::TargetDevice getTargetDevice() noexcept;

    virtual InferenceEngine::StatusCode setBatchSize(const size_t size) noexcept;

    virtual size_t getBatchSize() const noexcept;

    void init();

    void addBlobs(const std::vector<Ptr<BackendWrapper> >& wrappers);

    void forward();

    bool isInitialized();

private:
    std::vector<InferenceEngine::CNNLayerPtr> layers;
    InferenceEngine::InputsDataMap inputs;
    InferenceEngine::OutputsDataMap outputs;
    InferenceEngine::BlobMap inpBlobs;
    InferenceEngine::BlobMap outBlobs;
    InferenceEngine::BlobMap allBlobs;
    InferenceEngine::InferenceEnginePluginPtr plugin;

    void initPlugin(InferenceEngine::ICNNNetwork& net);
};

class InfEngineBackendNode : public BackendNode
{
public:
    InfEngineBackendNode(const InferenceEngine::CNNLayerPtr& layer);

    void connect(std::vector<Ptr<BackendWrapper> >& inputs,
                 std::vector<Ptr<BackendWrapper> >& outputs);

    InferenceEngine::CNNLayerPtr layer;
    // Inference Engine network object that allows to obtain the outputs of this layer.
    Ptr<InfEngineBackendNet> net;
};

class InfEngineBackendWrapper : public BackendWrapper
{
public:
    InfEngineBackendWrapper(int targetId, const Mat& m);

    ~InfEngineBackendWrapper();

    virtual void copyToHost();

    virtual void setHostDirty();

    InferenceEngine::DataPtr dataPtr;
    InferenceEngine::TBlob<float>::Ptr blob;
};

InferenceEngine::TBlob<float>::Ptr wrapToInfEngineBlob(const Mat& m);

InferenceEngine::TBlob<float>::Ptr wrapToInfEngineBlob(const Mat& m, const std::vector<size_t>& shape);

InferenceEngine::DataPtr infEngineDataNode(const Ptr<BackendWrapper>& ptr);

// Fuses convolution weights and biases with channel-wise scales and shifts.
void fuseConvWeights(const std::shared_ptr<InferenceEngine::ConvolutionLayer>& conv,
                     const Mat& w, const Mat& b = Mat());

// This is a fake class to run networks from Model Optimizer. Objects of that
// class simulate responses of layers are imported by OpenCV and supported by
// Inference Engine. The main difference is that they do not perform forward pass.
class InfEngineBackendLayer : public Layer
{
public:
    InfEngineBackendLayer(const InferenceEngine::DataPtr& output);

    virtual bool getMemoryShapes(const std::vector<MatShape> &inputs,
                                 const int requiredOutputs,
                                 std::vector<MatShape> &outputs,
                                 std::vector<MatShape> &internals) const;

    virtual void forward(std::vector<Mat*> &input, std::vector<Mat> &output,
                         std::vector<Mat> &internals);

    virtual void forward(InputArrayOfArrays inputs, OutputArrayOfArrays outputs,
                         OutputArrayOfArrays internals);

    virtual bool supportBackend(int backendId);

private:
    InferenceEngine::DataPtr output;
};


#endif  // HAVE_INF_ENGINE

bool haveInfEngine();

void forwardInfEngine(Ptr<BackendNode>& node);

}}  // namespace dnn, namespace cv

#endif  // __OPENCV_DNN_OP_INF_ENGINE_HPP__
