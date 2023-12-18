#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>

extern "C"
{
  #include <cl_common.h>
  #include <cl_main.h>
  #include <cl_memory.h>
}
#include <cl_frontend.h>

#include <QRetroCommon.h>

#include <Pleasant.h>
#include "cls_network_manager.h"
#include "cls_thread.h"

static ClsNetworkManager network_manager;

static Pleasant* _plthis(void)
{
  return reinterpret_cast<Pleasant*>(_qrthis());
}

void cl_fe_display_message(unsigned level, const char *msg)
{
  QMessageBox msg_box;

  msg_box.setText(msg);
  switch (level)
  {
  case CL_MSG_DEBUG:
  case CL_MSG_INFO:
    msg_box.setIcon(QMessageBox::Information);
    break;
  case CL_MSG_WARN:
    msg_box.setIcon(QMessageBox::Warning);
    break;
  case CL_MSG_ERROR:
    msg_box.setIcon(QMessageBox::Critical);
    break;
  default:
    msg_box.setIcon(QMessageBox::Question);
  }
  msg_box.exec();
}

bool cl_fe_install_membanks(void)
{
  auto _this = _plthis();

  return _this && _this->installMembanks();
}

const char* cl_fe_library_name(void)
{
  auto _this = _plthis();

  return _this ? _this->libraryName() : nullptr;
}

void cl_fe_pause(void)
{
  auto _this = _plthis();

  if (_this)
    _this->pause();
}

void cl_fe_unpause(void)
{
  auto _this = _plthis();

  if (_this)
    _this->unpause();
}

void cl_fe_network_post(const char *url, char *data, void(*callback)(cl_network_response_t))
{
  cls_net_cb cb = { callback };
  emit network_manager.request(url, data, cb);
}

void cl_fe_thread(cl_task_t *task)
{
  auto *thread = new ClsThread(task);
  thread->start();
}

bool cl_fe_user_data(cl_user_t *user, unsigned index)
{
  CL_UNUSED(index);
  snprintf(user->username, sizeof(user->username), "%s", "jacory");
  snprintf(user->password, sizeof(user->password), "%s", "jacory");
  snprintf(user->language, sizeof(user->language), "%s", "en_US");

  return true;
}
