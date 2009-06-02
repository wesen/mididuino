
/*
 * Copyright (c) 2006-2009 by Roland Riegel <feedback@roland-riegel.de>, Manuel Odendahl <wesen@ruinwesen.com>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <string.h>
#include <SDCard.h>

#ifndef NULL
#define NULL 0
#endif

SDCardClass::SDCardClass() {
  partition = NULL;
  fs = NULL;
  root_dir = NULL;
  current_dir = NULL;
}

uint8_t SDCardClass::init() {
  if (current_dir != NULL) {
    fat_close_dir(current_dir);
    current_dir = NULL;
  }
  if (root_dir != NULL) {
    fat_close_dir(root_dir);
    root_dir = NULL;
  }

  if (fs != NULL) {
    fat_close(fs);
    fs = NULL;
  }

  if (partition != NULL) {
    partition_close(partition);
    partition = NULL;
  }
  
  if (!sd_raw_init()) {
    return 1;
  }

  partition = partition_open(sd_raw_read, sd_raw_read_interval,
			     sd_raw_write, sd_raw_write_interval,
			     0);

  if (!partition) {
    return 2;
  }

  fs = fat_open(partition);
  if (!fs) {
    return 3;
  }

  fat_get_dir_entry_of_path(fs, "/", &root_dir_entry);
  root_dir = fat_open_dir(fs, &root_dir_entry);
  if (!root_dir) {
    return 4;
  }
  // initialize current directory to root directory
  current_dir = fat_open_dir(fs, &root_dir_entry);
}

bool SDCardClass::openDirectory(char *path) {
  if (current_dir != NULL) {
    fat_close_dir(current_dir);
    current_dir = NULL;
  }
  fat_get_dir_entry_of_path(fs, path, &current_dir_entry);
  current_dir = fat_open_dir(fs, &current_dir_entry);
  return current_dir != NULL;
}

bool SDCardClass::createDirectory(char *path) {
  if (current_dir != NULL) {
    fat_close_dir(current_dir);
    current_dir = NULL;
  }

  return false;
}

offset_t SDCardClass::getSize() {
  if (fs)
    return fat_get_fs_size(fs);
  else
    return 0;
}

offset_t SDCardClass::getFree() {
  if (fs)
    return fat_get_fs_free(fs);
  else
    return 0;
}



uint8_t find_file_in_dir(struct fat_fs_struct* fs,
			 struct fat_dir_struct* dd,
			 const char* name,
			 struct fat_dir_entry_struct* dir_entry) {
  while (fat_read_dir(dd, dir_entry)) {
    if (strcmp(dir_entry->long_name, name) == 0) {
	fat_reset_dir(dd);
	return 1;
      }
  }
  
  return 0;
}


SDCardFile::SDCardFile() {
  file = NULL;
}

bool SDCardFile::open(char *path) {
  return false;
}

void SDCardFile::close() {
}

intptr_t SDCardFile::read(uint8_t *buf, uint8_t len) {
  return 0;
}
