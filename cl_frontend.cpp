#include <QDir>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

extern "C"
{
  #include <cl_abi.h>
  #include <cl_common.h>
  #include <cl_main.h>
  #include <cl_memory.h>
}

#include <QRetroCommon.h>

#include <Pleasant.h>
#include "cls_network_manager.h"
#include "cls_thread.h"

static Pleasant* _plthis(void)
{
  return reinterpret_cast<Pleasant*>(_qrthis());
}

static cl_error cls_abi_display_message(unsigned level, const char *msg)
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

  return CL_OK;
}

static cl_error cls_abi_install_memory_regions(cl_memory_region_t **regions,
  unsigned *region_count)
{
  auto _this = _plthis();

  if (_this)
    return _this->installMemoryRegions(regions, region_count);
  else
    return CL_ERR_PARAMETER_NULL;
}

static cl_error cls_abi_library_name(const char **name)
{
  auto _this = _plthis();

  if (_this)
    return _this->libraryName(name);
  else
    return CL_ERR_PARAMETER_NULL;
}

static cl_error cls_abi_network_post(const char *url, char *data,
  cl_network_cb_t callback, void *userdata)
{
  auto _this = _plthis();

  if (_this)
  {
    cls_net_cb cb = { callback, userdata };
    QString url_string = QString(url);
    QString data_string = QString(data);
    emit _this->networkManager()->request(url_string, data_string, cb);

    return CL_OK;
  }
  else
    return CL_ERR_PARAMETER_NULL;
}

static cl_error cls_abi_set_pause(unsigned mode)
{
  auto _this = _plthis();

  if (!_this)
    return CL_ERR_PARAMETER_NULL;
  else
  {
    if (mode)
      _this->pause();
    else
      _this->unpause();

    return CL_OK;
  }
}

static cl_error cls_abi_thread(cl_task_t *task)
{
  if (!task)
    return CL_ERR_PARAMETER_NULL;
  else
  {
    auto *thread = new ClsThread(task);
    thread->start();
  }

  return CL_OK;
}

static cl_error cls_abi_user_data(cl_user_t *user, unsigned index)
{
  CL_UNUSED(index);

  QSettings settings(QDir::currentPath() + "/history.ini", QSettings::IniFormat);
  QByteArray username = settings.value("clsUsername", "").toString().toUtf8();
  QByteArray password = settings.value("clsPassword", "").toString().toUtf8();

  snprintf(user->username, sizeof(user->username), "%s", username.constData());
  snprintf(user->password, sizeof(user->password), "%s", password.constData());
  snprintf(user->language, sizeof(user->language), "%s", "en_US");
  user->token[0] = '\0';

  return CL_OK;
}

const cl_abi_t cls_abi
{
  CL_ABI_VERSION,
  {
    {
      cls_abi_display_message,
      cls_abi_install_memory_regions,
      cls_abi_library_name,
      cls_abi_network_post,
      cls_abi_set_pause,
      cls_abi_thread,
      cls_abi_user_data
    },
    { NULL, NULL, NULL, NULL }
  }
};

cl_error cls_abi_register(void)
{
  return cl_abi_register(&cls_abi);
}
