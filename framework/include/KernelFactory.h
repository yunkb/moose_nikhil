#ifndef KERNELFACTORY_H
#define KERNELFACTORY_H

#include "Kernel.h"

// System includes
#include <map>
#include <string>
#include <vector>
#include <typeinfo>

// LibMesh includes
#include <parameters.h>


/**
 * Typedef to make things easier.
 */
typedef Kernel * (*kernelBuildPtr)(std::string name,
                                   Parameters parameters,
                                   std::string var_name,
                                   std::vector<std::string> coupled_to,
                                   std::vector<std::string> coupled_as);
/**
 * Typedef to make things easier.
 */
typedef Parameters (*kernelParamsPtr)();

/**
 * Templated build function used for generating function pointers to build classes on demand.
 */
template<typename KernelType>
Kernel * buildKernel(std::string name,
                     Parameters parameters,
                     std::string var_name,
                     std::vector<std::string> coupled_to,
                     std::vector<std::string> coupled_as)
{
  return new KernelType(name, parameters, var_name, coupled_to, coupled_as);
}

/**
 * Responsible for building Kernels on demand and storing them for retrieval
 */
class KernelFactory
{
public:
  static KernelFactory * instance();

  template<typename KernelType> 
  void registerKernel(std::string name)
  {
    name_to_build_pointer[name]=&buildKernel<KernelType>;
    name_to_params_pointer[name]=&valid_params<KernelType>;
  }

  Kernel * add(std::string kernel_name,
               std::string name,
               Parameters parameters,
               std::string var_name,
               std::vector<std::string> coupled_to=std::vector<std::string>(0),
               std::vector<std::string> coupled_as=std::vector<std::string>(0));
  

  Kernel * add(std::string kernel_name,
               std::string name,
               Parameters parameters,
               std::string var_name,
               std::vector<std::string> coupled_to,
               std::vector<std::string> coupled_as,
               unsigned int block_id);
  
  Parameters getValidParams(std::string name);
  
  std::vector<Kernel *>::iterator activeKernelsBegin(THREAD_ID tid);
  std::vector<Kernel *>::iterator activeKernelsEnd(THREAD_ID tid);

  std::vector<Kernel *>::iterator blockKernelsBegin(THREAD_ID tid, unsigned int block_id);
  std::vector<Kernel *>::iterator blockKernelsEnd(THREAD_ID tid, unsigned int block_id);

private:
  KernelFactory();

  virtual ~KernelFactory() 
    {

      
      
    }
  

  std::map<std::string, kernelBuildPtr> name_to_build_pointer;
  std::map<std::string, kernelParamsPtr> name_to_params_pointer;

  std::vector<std::vector<Kernel *> > active_kernels;

  std::vector<std::map<unsigned int, std::vector<Kernel *> > > block_kernels;
};

#endif //KERNELFACTORY_H
