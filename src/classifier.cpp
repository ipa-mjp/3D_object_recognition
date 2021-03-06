#include <robot_vision/classifier.h>

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/filters/uniform_sampling.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/features/shot_omp.h>
#include <pcl/features/vfh.h>
#include <pcl/common/common.h>


#include<iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <time.h>


classifier::classifier(void){
        SVMclf = cv::ml::SVM::create();
    }

classifier::classifier(std::string filename){
        SVMclf = cv::ml::SVM::create();
        SVMclf->load(filename.c_str());
    }

void classifier::trainSVM(cv::Mat _trainingData, cv::Mat _trainingLabels){
    SVMclf->setType(cv::ml::SVM::Types::C_SVC);
	SVMclf->setKernel(cv::ml::SVM::KernelTypes::LINEAR);
	SVMclf->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));

    cv::Ptr<cv::ml::TrainData> tData = cv::ml::TrainData::create(_trainingData, cv::ml::ROW_SAMPLE,_trainingLabels);
    SVMclf->train(tData);
    std::cout << "SVM trained " << std::endl;
    SVMclf->save("SVMclf->xml");

}
void classifier::trainSVM(cv::Mat _trainingData,
                          cv::Mat _trainingLabels,
                          std::string fileName){
    SVMclf->setType(cv::ml::SVM::Types::C_SVC);
	SVMclf->setKernel(cv::ml::SVM::KernelTypes::LINEAR);
	SVMclf->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));

    cv::Ptr<cv::ml::TrainData> tData = cv::ml::TrainData::create(_trainingData, cv::ml::ROW_SAMPLE,_trainingLabels);
    SVMclf->train(tData);
    std::cout << "SVM trained " << std::endl;
    fileName+=".xml";
    SVMclf->save(fileName.c_str());

}

float classifier::getConfidence(float distance){
    float conf = 1.0 / ( 1.0 + exp(-1*distance));
    return conf;

}
void classifier::validateSVM(cv::Mat _testData, cv::Mat _testLabels){
    float hits = 0;
    float miss = 0;
    for(size_t idx=0;idx<_testData.cols;idx++){
        std::cout << " [test] "<< "idx:"<< idx << " " << SVMclf->predict(_testData.row(idx)) << " "
                  << _testLabels.at<float>(idx,0) << " [ confidence ] "
                  << getConfidence(SVMclf->predict(_testData.row(idx))) <<  std::endl;
        if (SVMclf->predict(_testData.row(idx))==_testLabels.at<float>(idx,0))
            hits++;
        else
            miss++;
    }
    printf(" [accuracy] %f ",(hits/(hits+miss)));
}

std::vector<float> classifier::predict(cv::Mat query){
    std::vector<float> result;
    result.push_back( SVMclf->predict(query));
    result.push_back( getConfidence(SVMclf->predict(query)));
    return result;
}

cv::Mat classifier::vector2Mat(pcl::PointCloud<pcl::VFHSignature308>::Ptr inputDescriptor){
    cv::Mat testArray = cv::Mat::zeros(1, 306, CV_32FC1);
        for(size_t j=0; j<306;j++){
            testArray.at<float>(0,j)= (float)inputDescriptor->points[0].histogram[j];
        }
    return testArray;
}
