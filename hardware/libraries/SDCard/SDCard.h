#ifndef SDCARD_H__
#define SDCARD_H__

extern "C" {
#include "byteordering.hh"
#include "partition.hh"
#include "fat.hh"
#include "fat_config.hh"
#include "sd-reader_config.hh"
#include "sd_raw.hh"
}

class SDCardClass {
 public:
  struct partition_struct *partition;
  struct fat_fs_struct    *fs;

  struct fat_dir_entry_struct root_dir_entry;
  struct fat_dir_struct *root_dir;
  struct fat_dir_entry_struct current_dir_entry;
  struct fat_dir_struct *current_dir;
  
  SDCardClass();
  uint8_t init();

  bool writeFile(char *path, uint8_t *buf, uint8_t len);
  int readFile(char *path, uint8_t *buf, uint8_t len);
  bool openDirectory(char *path);
  bool createDirectory(char *path);
  
  offset_t getSize();
  offset_t getFree();
};

extern SDCardClass SDCard;

class SDCardFile {
 public:
  struct fat_dir_entry_struct file_dir_entry;
  struct fat_file_struct *fd;

  SDCardFile();
  ~SDCardFile() { close(); }
  bool open(char *path);
  bool create(char *path);
  void close();
  intptr_t read(uint8_t *buf, uint8_t len);
  intptr_t write(uint8_t *buf, uint8_t len);
  bool seek(int32_t *offset, uint8_t whence);
};

#endif /* SDCARD_H__ */
