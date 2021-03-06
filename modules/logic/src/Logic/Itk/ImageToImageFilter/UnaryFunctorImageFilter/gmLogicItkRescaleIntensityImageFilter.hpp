#pragma once

#include "gmLogicItkUnaryFunctorImageFilter.hpp"

#include <itkRescaleIntensityImageFilter.h>

namespace gm
{
    namespace Logic
    {
        namespace Itk
        {
            class RescaleIntensityImageFilter : public UnaryFunctorImageFilter
            {
            public:
                struct ID
                {
                    static constexpr auto input = "input";
                    static constexpr auto outputMinimum = "output minimum";
                    static constexpr auto outputMaximum = "output maximum";
                    static constexpr auto output = "output";
                };
                
                static Register<RescaleIntensityImageFilter> Register;

                using Access = Enable::ImageTypes <
                    Enable::Dimension<2,3>,
                    Enable::Scalar<unsigned char, unsigned short, unsigned int, char, short, int, float, double>
                >;

                RescaleIntensityImageFilter() : UnaryFunctorImageFilter("RescaleIntensityImageFilter")
                {
                    log_trace(Log::New);
                    
                    this->addParamGroup("RescaleIntensityImageFilter");
                    
                    this->add(new Slot::Input<Data::Image>(ID::input, Access::MakeConstraints(), Data::Required));
                    this->add(new Param::Number(ID::outputMinimum, 0));
                    this->add(new Param::Number(ID::outputMaximum, 255));
                    this->add(new Slot::Output<Data::Image>(ID::output, Access::MakeConstraints()));
                }
                
                template <class TImage>
                auto process() -> void
                {
                    using ImageType     = typename TImage::itk_image_t;
                    using FilterType    = itk::RescaleIntensityImageFilter<ImageType, ImageType>;
                    
                    auto filter         = FilterType::New();
                    auto output         = this->getOutput<Data::Image>(ID::output);
                    
                    applyParameters<FilterType>(filter);
                    
                    filter->SetOutputMinimum(this->getParam<Param::Number>(ID::outputMinimum)->getValue());
                    filter->SetOutputMaximum(this->getParam<Param::Number>(ID::outputMaximum)->getValue());
                    
                    for (auto it : this->inputIterator())
                    {
                        filter->SetInput(Image::ToItk<ImageType>(it.image()));
                        filter->Update();
                        output->addImage(Image::FromItk<ImageType>(filter->GetOutput()));
                    }
                }
                
                auto run() -> void override
                {
                    log_trace(Log::Func);
                    
                    Invoke(this);
                }
            };
        }
    }
}