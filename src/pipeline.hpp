#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <list>
#include <memory>
#include <iostream>
#include <opencv2/core/mat.hpp>

#include "modules/imodule.hpp"

class Pipeline final 
{
  public:
    Pipeline() = default;
    ~Pipeline() = default;

  public:
    template<typename Module, typename... Argc> 
    inline std::shared_ptr<Module> createModule(Argc&& ...args)
    {
      std::cout << __PRETTY_FUNCTION__ << " creating module" << std::endl;
      mModules.emplace_back(std::make_shared<Module>(std::forward<Argc>(args)...));
      const auto &module = mModules.back();
      module->setDebugMode(mDebugMode);
      return std::dynamic_pointer_cast<Module>(module);
    }        

  public:
    cv::Mat run(const cv::Mat &image);

  public:
    void setDebugMode(bool debugMode) noexcept { mDebugMode = debugMode; }
    bool getDebugMode() const noexcept { return mDebugMode; }

  private:
    bool mDebugMode { false };

  private:
    typedef std::shared_ptr<IModule> ModulePtr;
    std::list<ModulePtr> mModules;
};

#endif