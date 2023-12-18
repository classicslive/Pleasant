#include "cls_thread.h"

ClsThread::ClsThread(cl_task_t* task)
{
  m_Task = task;
}

ClsThread::~ClsThread()
{
  free(m_Task);
}

bool ClsThread::error(void)
{
  return m_Task && m_Task->error && !QString(m_Task->error).isEmpty();
}

void ClsThread::run(void)
{
  if (m_Task->handler)
    m_Task->handler(m_Task);
  if (m_Task->callback)
    m_Task->callback(m_Task);

  QThread::exit(error() ? 1 : 0);
}
