#ifndef MONOME_HOST_H__
#define MONOME_HOST_H__

#include "Monome.h"
#include <select.hh>

class MonomeHost : public MonomeParentClass {
 protected:
  int fd;
  FDSelect fds;

 public:
  MonomeHost(const char *file);

  ~MonomeHost();

  virtual void sendBuf(uint8_t *data, uint8_t len);
  virtual void sendMessage(uint8_t byte1, uint8_t byte2);

  bool isAvailable(unsigned long timeout = 1);
  void handle();
};

#endif /* MONOME_HOST_H__ */
