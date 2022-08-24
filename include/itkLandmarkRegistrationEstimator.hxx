/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef itkLandmarkRegistrationEstimator_hxx
#define itkLandmarkRegistrationEstimator_hxx

#include "itkLandmarkRegistrationEstimator.h"
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkSimilarity3DTransform.h"

namespace itk
{

template <unsigned int Dimension>
LandmarkRegistrationEstimator<Dimension>::LandmarkRegistrationEstimator()
{
  this->deltaSquared = NumericTraits<double>::min();
  this->minForEstimate = Dimension;
}


template <unsigned int Dimension>
LandmarkRegistrationEstimator<Dimension>::~LandmarkRegistrationEstimator()
{}


template <unsigned int Dimension>
void
LandmarkRegistrationEstimator<Dimension>::SetDelta(double delta)
{
  this->deltaSquared = delta * delta;
}


template <unsigned int Dimension>
double
LandmarkRegistrationEstimator<Dimension>::GetDelta()
{
  return sqrt(this->deltaSquared);
}

template <unsigned int Dimension>
void
LandmarkRegistrationEstimator<Dimension>::Estimate(std::vector<Point<double, Dimension> *> & data,
                                                   std::vector<double> &                     parameters)
{
  parameters.clear();

  constexpr unsigned int DimensionPoint = 3;
  using PixelType = float;
  using FixedImageType = itk::Image<PixelType, DimensionPoint>;
  using MovingImageType = itk::Image<PixelType, DimensionPoint>;

  using TransformType = itk::Similarity3DTransform<double>;
  using TransformInitializerType =
    itk::LandmarkBasedTransformInitializer<TransformType, FixedImageType, MovingImageType>;
  auto initializer = TransformInitializerType::New();

  typename TransformInitializerType::LandmarkPointContainer fixedLandmarks;
  typename TransformInitializerType::LandmarkPointContainer movingLandmarks;

  // Obtain the parameters of the Similarity3DTransform
  auto transform = TransformType::New();

  fixedLandmarks.reserve(data.size());
  movingLandmarks.reserve(data.size());

  // Create landmark points from the 6D input points
  for (unsigned int i = 0; i < data.size(); ++i)
  {
    Point<double, Dimension> & pnt = *(data[i]);
    itk::Point<double, 3>      point1;
    point1[0] = pnt[0];
    point1[1] = pnt[1];
    point1[2] = pnt[2];
    fixedLandmarks.push_back(point1);

    itk::Point<double, 3> point2;
    point2[0] = pnt[3];
    point2[1] = pnt[4];
    point2[2] = pnt[5];
    movingLandmarks.push_back(point2);
  }

  initializer->SetMovingLandmarks(movingLandmarks);
  initializer->SetFixedLandmarks(fixedLandmarks);
  initializer->SetTransform(transform);
  initializer->InitializeTransform();

  // Copy the transform parameters in the input variable
  auto transformParameters = transform->GetParameters();
  for (unsigned int i = 0; i < 7; ++i)
  {
    parameters.push_back(transformParameters.GetElement(i));
  }

  auto fixedParameters = transform->GetFixedParameters();
  for (unsigned int i = 0; i < 3; ++i)
  {
    parameters.push_back(fixedParameters.GetElement(i));
  }

  fixedLandmarks.clear();
  movingLandmarks.clear();
  return;
}

template <unsigned int Dimension>
void
LandmarkRegistrationEstimator<Dimension>::Estimate(std::vector<Point<double, Dimension>> & data,
                                                   std::vector<double> &                   parameters)
{
  std::vector<Point<double, Dimension> *> usedData;
  int                                     dataSize = data.size();
  for (int i = 0; i < dataSize; i++)
  {
    usedData.push_back(&(data[i]));
  }
  Estimate(usedData, parameters);
}

template <unsigned int Dimension>
void
LandmarkRegistrationEstimator<Dimension>::LeastSquaresEstimate(std::vector<Point<double, Dimension> *> & data,
                                                               std::vector<double> &                     parameters)
{
  parameters.clear();

  using PixelType = float;
  constexpr unsigned int DimensionPoint = 3;
  using FixedImageType = itk::Image<PixelType, DimensionPoint>;
  using MovingImageType = itk::Image<PixelType, DimensionPoint>;

  using Similarity3DTransformType = Similarity3DTransform<double>;
  using TransformInitializerType =
    itk::LandmarkBasedTransformInitializer<Similarity3DTransformType, FixedImageType, MovingImageType>;
  // Obtain the parameters of the Similarity3DTransform
  

  auto transform = Similarity3DTransformType::New();
  auto initializer = TransformInitializerType::New();

  itk::Point<double, 3>                                     point;
  typename TransformInitializerType::LandmarkPointContainer fixedLandmarks;
  typename TransformInitializerType::LandmarkPointContainer movingLandmarks;

  // Create landmark points from the 6D input points
  for (unsigned int i = 0; i < data.size(); ++i)
  {
    Point<double, Dimension> & pnt = *(data[i]);

    point[0] = pnt[0];
    point[1] = pnt[1];
    point[2] = pnt[2];
    fixedLandmarks.push_back(point);

    point[0] = pnt[3];
    point[1] = pnt[4];
    point[2] = pnt[5];
    movingLandmarks.push_back(point);
  }

  initializer->SetMovingLandmarks(movingLandmarks);
  initializer->SetFixedLandmarks(fixedLandmarks);
  initializer->SetTransform(transform);
  initializer->InitializeTransform();

  // Copy the transform parameters in the input variable
  auto transformParameters = transform->GetParameters();
  for (unsigned int i = 0; i < 7; ++i)
  {
    parameters.push_back(transformParameters.GetElement(i));
  }

  auto fixedParameters = transform->GetFixedParameters();
  for (unsigned int i = 0; i < 3; ++i)
  {
    parameters.push_back(fixedParameters.GetElement(i));
  }
  return;
}

template <unsigned int Dimension>
void
LandmarkRegistrationEstimator<Dimension>::LeastSquaresEstimate(std::vector<Point<double, Dimension>> & data,
                                                               std::vector<double> &                   parameters)
{
  std::vector<Point<double, Dimension> *> usedData;
  int                                     dataSize = data.size();
  for (int i = 0; i < dataSize; i++)
  {
    usedData.push_back(&(data[i]));
  }
  LeastSquaresEstimate(usedData, parameters);
}

template <unsigned int Dimension>
bool
LandmarkRegistrationEstimator<Dimension>::Agree(std::vector<double> & parameters, Point<double, Dimension> & data)
{
  using Similarity3DTransformType = Similarity3DTransform<double>;
  auto transform = Similarity3DTransformType::New();

  auto optParameters = transform->GetParameters();
  auto fixedParameters = transform->GetFixedParameters();

  int counter = 0;
  for (unsigned int i = 7; i < 10; ++i)
  {
    fixedParameters.SetElement(counter, parameters[i]);
    counter = counter + 1;
  }
  transform->SetFixedParameters(fixedParameters);

  counter = 0;
  for (unsigned int i = 0; i < 7; ++i)
  {
    optParameters.SetElement(counter, parameters[i]);
    counter = counter + 1;
  }
  transform->SetParameters(optParameters);

  itk::Point<double, 3> p0;
  itk::Point<double, 3> p1;

  p0[0] = data[0];
  p0[1] = data[1];
  p0[2] = data[2];

  p1[0] = data[3];
  p1[1] = data[4];
  p1[2] = data[5];

  auto transformedPoint = transform->TransformPoint(p0);
  auto distance = transformedPoint.EuclideanDistanceTo(p1);
  return (distance < this->deltaSquared);
}


template <unsigned int Dimension>
std::vector<bool>
LandmarkRegistrationEstimator<Dimension>::AgreeMultiple(std::vector<double> & parameters, std::vector<Point<double, Dimension>> & data)
{
  using Similarity3DTransformType = Similarity3DTransform<double>;
  auto transform = Similarity3DTransformType::New();

  auto optParameters = transform->GetParameters();
  auto fixedParameters = transform->GetFixedParameters();

  int counter = 0;
  for (unsigned int i = 7; i < 10; ++i)
  {
    fixedParameters.SetElement(counter, parameters[i]);
    counter = counter + 1;
  }
  transform->SetFixedParameters(fixedParameters);

  counter = 0;
  for (unsigned int i = 0; i < 7; ++i)
  {
    optParameters.SetElement(counter, parameters[i]);
    counter = counter + 1;
  }
  transform->SetParameters(optParameters);

  std::vector<bool> output;
  for (unsigned int i=0; i < data.size(); ++i)
  {
    itk::Point<double, 3> p0;
    itk::Point<double, 3> p1;

    p0[0] = data[i][0];
    p0[1] = data[i][1];
    p0[2] = data[i][2];

    p1[0] = data[i][3];
    p1[1] = data[i][4];
    p1[2] = data[i][5];

    auto transformedPoint = transform->TransformPoint(p0);
    auto distance = transformedPoint.EuclideanDistanceTo(p1);
    output.push_back((distance < this->deltaSquared));
  }

  return output;
}


} // end namespace itk

#endif
