#ifndef PLEASANT_H
#define PLEASANT_H

#include <QRetro.h>

#include "cls_network_manager.h"

typedef struct
{

} cl_ctx_t;

class Pleasant : public QRetro
{
public:
  const cl_ctx_t* clCtx() { return &m_ClCtx; }
  bool installMembanks(void);
  const char* libraryName(void);
  ClsNetworkManager *networkManager(void) { return &m_NetworkManager; }

private:
  cl_ctx_t m_ClCtx;
  ClsNetworkManager m_NetworkManager;
};

#endif
