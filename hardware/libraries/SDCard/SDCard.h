#ifndef SDCARD_H__
#define SDCARD_H__

#include "WProgram.h"
#include <inttypes.h>

extern "C" {
#include "byteordering.hh"
#include "partition.hh"
#include "fat.hh"
#include "fat_config.hh"
#include "sd-reader_config.hh"
#include "sd_raw.hh"
}

class SDCardEntry;

class SDCardClass {
 public:
  struct partition_struct *partition;
  struct fat_fs_struct    *fs;

  SDCardClass();
  uint8_t init();
  offset_t getSize();
  offset_t getFree();

  bool findFile(char *path, struct fat_dir_entry_struct *dir_entry);
  bool findFile(char *path, SDCardEntry *entry);
  
  bool writeFile(char *path, uint8_t *buf, uint8_t len);
  int readFile(char *path, uint8_t *buf, uint8_t len);
  bool deleteFile(char *path, bool recursive = false);

  bool createDirectory(char *path, SDCardEntry *entry = NULL);
  int listDirectory(char *path, SDCardEntry entries[], int maxCount);
};

extern SDCardClass SDCard;

class SDCardEntry {
 public:
  struct fat_dir_entry_struct dir_entry;

  bool exists;
  char dir[128];
  char name[32];

  SDCardEntry();
  SDCardEntry(char *path);

  bool setPath(char *path);

  void setFromParentEntry(SDCardEntry *parent);

  bool deleteEntry(bool recursive = false);

  /* file functions to slurp and write full file */
  int readFile(uint8_t *buf, uint8_t len);
  int writeFile(uint8_t *buf, uint8_t len);

  /* directory functions */
  bool isDirectory();
  bool findFile(char *name, struct fat_dir_entry_struct *entry);
  bool findFile(char *name, SDCardEntry *entry) {
    return findFile(name, &entry->dir_entry);
  }
  int listDirectory(SDCardEntry entries[], int maxCount);

  bool createSubDirectory(char *path, struct fat_dir_entry_struct *new_entry);
  bool createSubDirectory(char *path, SDCardEntry *entry) {
    return createSubDirectory(path, &entry->dir_entry);
  }
};

class SDCardFile : public SDCardEntry {
 public:
  struct fat_file_struct *fd;
  
 SDCardFile(char *path) : SDCardEntry(path) {
    fd = NULL;
  }

  ~SDCardFile() { close(); }
  bool open(bool create);
  void close();
  
  intptr_t read(uint8_t *buf, uint8_t len);
  intptr_t write(uint8_t *buf, uint8_t len);
  bool seek(int32_t *offset, uint8_t whence);
};

#endif /* SDCARD_H__ */
