#ifndef PLEASANT_H
#define PLEASANT_H

#include <QRetro.h>

#include "cls_network_manager.h"

typedef struct
{

} cl_ctx_t;

cl_error cls_abi_register(void);

class Pleasant : public QRetro
{
public:
  const cl_ctx_t* clCtx(void) { return &m_ClCtx; }

  cl_error installMemoryRegions(cl_memory_region_t **regions,
                                unsigned *region_count);

  cl_error libraryName(const char **name);

  ClsNetworkManager *networkManager(void) { return &m_NetworkManager; }

private:
  cl_ctx_t m_ClCtx;
  ClsNetworkManager m_NetworkManager;
};

#endif
