#ifndef itkLandmarkRegistrationEstimator_hxx
#define itkLandmarkRegistrationEstimator_hxx

#include "itkLandmarkRegistrationEstimator.h"
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkSimilarity3DTransform.h"

namespace itk
{

template <unsigned int dimension>
LandmarkRegistrationEstimator<dimension>::LandmarkRegistrationEstimator()
{
  this->deltaSquared = NumericTraits<double>::min();
  this->minForEstimate = dimension;
}


template <unsigned int dimension>
LandmarkRegistrationEstimator<dimension>::~LandmarkRegistrationEstimator()
{}


template <unsigned int dimension>
void
LandmarkRegistrationEstimator<dimension>::SetDelta(double delta)
{
  this->deltaSquared = delta * delta;
  std::cout << "Pranjal setting delta " << this->deltaSquared << std::endl;
}


template <unsigned int dimension>
double
LandmarkRegistrationEstimator<dimension>::GetDelta()
{
  return sqrt(this->deltaSquared);
}

template <unsigned int dimension>
void
LandmarkRegistrationEstimator<dimension>::Estimate(std::vector<Point<double, dimension> *> & data,
                                              std::vector<double> &                     parameters)
{
  std::cout << "Inside RANSAC Estimate " << data.size() << parameters.size() << std::endl;
  parameters.clear();

  using PixelType = float;
  constexpr unsigned int Dimension = 3;
  using FixedImageType = itk::Image<PixelType, Dimension>;
  using MovingImageType = itk::Image<PixelType, Dimension>;

  using TransformType = itk::Similarity3DTransform<double>;
  using TransformInitializerType =
    itk::LandmarkBasedTransformInitializer<TransformType, FixedImageType, MovingImageType>;
  auto initializer = TransformInitializerType::New();

  itk::Point<double, 3> point;
  typename TransformInitializerType::LandmarkPointContainer fixedLandmarks;
  typename TransformInitializerType::LandmarkPointContainer movingLandmarks;

  // Create landmark points from the 6D input points
  for (unsigned int i =0; i < data.size(); ++i)
  {
    point[0] = data[i]->GetElement(0);
    point[1] = data[i]->GetElement(1);
    point[2] = data[i]->GetElement(2);
    fixedLandmarks.push_back(point);

    point[0] = data[i]->GetElement(3);
    point[1] = data[i]->GetElement(4);
    point[2] = data[i]->GetElement(5);
    movingLandmarks.push_back(point);
  }

  // Obtain the parameters of the Similarity3DTransform
  using Similarity3DTransformType = Similarity3DTransform<double>;
  auto transform = Similarity3DTransformType::New();
  
  initializer->SetMovingLandmarks(movingLandmarks);
  initializer->SetFixedLandmarks(fixedLandmarks);
  initializer->SetTransform(transform);
  initializer->InitializeTransform();

  // Copy the transform parameters in the input variable
  parameters.clear();
  auto transformParameters =  transform->GetParameters();
  for(unsigned int i=0; i < transformParameters.Size(); ++i)
  {
    parameters.push_back(transformParameters.GetElement(i));
  }

  auto fixedParameters =  transform->GetFixedParameters();
  for(unsigned int i=0; i < fixedParameters.Size(); ++i)
  {
    parameters.push_back(fixedParameters.GetElement(i));
  }
  return;
}

template <unsigned int dimension>
void
LandmarkRegistrationEstimator<dimension>::Estimate(std::vector<Point<double, dimension>> & data,
                                              std::vector<double> &                     parameters)
{
  std::vector<Point<double, dimension> *> usedData;
  int                                     dataSize = data.size();
  for (int i = 0; i < dataSize; i++)
  {
    usedData.push_back(&(data[i]));
  }
  Estimate(usedData, parameters);
}

template <unsigned int dimension>
void
LandmarkRegistrationEstimator<dimension>::LeastSquaresEstimate(std::vector<Point<double, dimension> *> & data,
                                                          std::vector<double> &                     parameters)
{
  using PixelType = float;
  constexpr unsigned int Dimension = 3;
  using FixedImageType = itk::Image<PixelType, Dimension>;
  using MovingImageType = itk::Image<PixelType, Dimension>;

  using TransformType = itk::Similarity3DTransform<double>;
  using TransformInitializerType =
    itk::LandmarkBasedTransformInitializer<TransformType, FixedImageType, MovingImageType>;
  auto initializer = TransformInitializerType::New();

  itk::Point<double, 3> point;
  typename TransformInitializerType::LandmarkPointContainer fixedLandmarks;
  typename TransformInitializerType::LandmarkPointContainer movingLandmarks;

  // Create landmark points from the 6D input points
  for (unsigned int i =0; i < data.size(); ++i)
  {
    point[0] = data[i]->GetElement(0);
    point[1] = data[i]->GetElement(1);
    point[2] = data[i]->GetElement(2);
    fixedLandmarks.push_back(point);

    point[0] = data[i]->GetElement(3);
    point[1] = data[i]->GetElement(4);
    point[2] = data[i]->GetElement(5);
    movingLandmarks.push_back(point);
  }

  // Obtain the parameters of the Similarity3DTransform
  using Similarity3DTransformType = Similarity3DTransform<double>;
  auto transform = Similarity3DTransformType::New();
  
  initializer->SetMovingLandmarks(movingLandmarks);
  initializer->SetFixedLandmarks(fixedLandmarks);
  initializer->SetTransform(transform);
  initializer->InitializeTransform();

  // Copy the transform parameters in the input variable
  parameters.clear();
  auto transformParameters =  transform->GetParameters();
  for(unsigned int i=0; i < transformParameters.Size(); ++i)
  {
    parameters.push_back(transformParameters.GetElement(i));
  }

  auto fixedParameters =  transform->GetFixedParameters();
  for(unsigned int i=0; i < fixedParameters.Size(); ++i)
  {
    parameters.push_back(fixedParameters.GetElement(i));
  }

  std::cout << "Scale is " << transform->GetScale() << std::endl;
  return;
}

template <unsigned int dimension>
void
LandmarkRegistrationEstimator<dimension>::LeastSquaresEstimate(std::vector<Point<double, dimension>> & data,
                                                          std::vector<double> &                     parameters)
{
  std::vector<Point<double, dimension> *> usedData;
  int                                     dataSize = data.size();
  for (int i = 0; i < dataSize; i++)
    usedData.push_back(&(data[i]));
  LeastSquaresEstimate(usedData, parameters);
}

template <unsigned int dimension>
bool
LandmarkRegistrationEstimator<dimension>::Agree(std::vector<double> & parameters, Point<double, dimension> & data)
{
  using TransformType = itk::Similarity3DTransform<double>;
  using Similarity3DTransformType = Similarity3DTransform<double>;
  auto transform = Similarity3DTransformType::New();

  auto optParameters = itk::OptimizerParameters<double>();
  auto fixedParameters = itk::OptimizerParameters<double>();

  optParameters.Reserve(7);
  fixedParameters.Reserve(3);

  for (unsigned int i = 7; i < 10; ++i)
  {
    fixedParameters.SetElement(i, parameters[i]);
  }
  transform->SetFixedParameters(fixedParameters);

  for (unsigned int i = 0; i < 7; ++i)
  {
    optParameters.SetElement(i, parameters[i]);
  }
  transform->SetParameters(optParameters);


  itk::Point<double, 3>  p0;
  itk::Point<double, 3>  p1;

  p0[0] = data[0];
  p0[1] = data[1];
  p0[2] = data[2];

  p1[0] = data[3];
  p1[1] = data[4];
  p1[2] = data[5];

  auto transformedPoint = transform->TransformPoint(p1);
  auto distance = transformedPoint.EuclideanDistanceTo(p0);

  return (distance < this->deltaSquared);
}

} // end namespace itk

#endif