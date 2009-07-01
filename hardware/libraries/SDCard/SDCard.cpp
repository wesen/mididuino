
/*
 * Copyright (c) 2006-2009 by Roland Riegel <feedback@roland-riegel.de>, Manuel Odendahl <wesen@ruinwesen.com>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "helpers.h"
#include <string.h>
#include <SDCard.h>

#ifndef NULL
#define NULL 0
#endif

/************************** SDCARD CLASS ***********************************/

SDCardClass::SDCardClass() {
  partition = NULL;
  fs = NULL;
}


uint8_t SDCardClass::init() {
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

  return 0;
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

bool SDCardClass::findFile(char *path, struct fat_dir_entry_struct *dir_entry) {
  return fat_get_dir_entry_of_path(SDCard.fs, path, dir_entry);
}

bool SDCardClass::findFile(char *path, SDCardEntry *entry) {
  return findFile(path, &entry->dir_entry);
}

bool SDCardClass::writeFile(char *path, uint8_t *buf, uint8_t len) {
  SDCardEntry entry(path);
  return false;
}

bool SDCardClass::createDirectory(char *path, SDCardEntry *entry) {
  SDCardEntry root((char *)"/");
  if (entry == NULL) {
    SDCardEntry bla;
    return root.createSubDirectory(path, &bla);
  } else {
    return root.createSubDirectory(path, entry);
  }
}

bool SDCardClass::deleteFile(char *path, bool recursive) {
  SDCardEntry file(path);
  return file.deleteEntry(recursive);
}

#if 0
bool SDCardClass::writeFile(char *path, uint8_t *buf, uint8_t len) {
  SDCardFile file;
  
  if (!file.create(path)) {
    return false;
  }
  uint8_t l2 = file.write(buf, len);
  if (l2 != len) {
    file.close();
    return false;
  }
  file.close();
  return true;
}

int SDCardClass::readFile(char *path, uint8_t *buf, uint8_t len) {
  SDCardFile file;
  if (!file.open(path)) {
    return -1;
  }
  uint8_t l2 = file.read(buf, len);
  file.close();
  return l2;
}


#endif

int SDCardClass::listDirectory(char *path, SDCardEntry entries[], int maxCount) {
  SDCardEntry entry(path);
  return entry.listDirectory(entries, maxCount);
}

SDCardClass SDCard;

/****************************** SDCARD ENTRY ***********************************/

SDCardEntry::SDCardEntry() {
  exists = false;
  dir[0] = '\0';
  name[0] = '\0';
}

SDCardEntry::SDCardEntry(char *path) {
  setPath(path);
}

bool SDCardEntry::setPath(char *path) {
  exists = SDCard.findFile(path, this);

  char *pos = strrchr(path, '/');
  if (pos != NULL) {
    uint8_t len = MIN(sizeof(dir), pos - path);
    if (len == 0) {
      m_strncpy(dir, "/", sizeof(dir));
    } else {
      m_strncpy(dir, path, len);
    }
    m_strncpy(name, pos + 1, sizeof(name));
  } else {
    m_strncpy(dir, path, sizeof(dir));
  }

  return exists;
}

bool SDCardEntry::isDirectory() {
  return exists && dir_entry.attributes & FAT_ATTRIB_DIR;
}

bool SDCardEntry::findFile(char *name, struct fat_dir_entry_struct *entry) {
  if (!isDirectory())
    return false;

  fat_dir_struct *dd = fat_open_dir(SDCard.fs, &dir_entry);
  if (dd == NULL)
    return false;

  while (fat_read_dir(dd, entry)) {
    if (strcmp(entry->long_name, name) == 0) {
      fat_close_dir(dd);
      return true;
    }
  }

  fat_close_dir(dd);
  return false;
}

int SDCardEntry::listDirectory(SDCardEntry entries[], int maxCount) {
  if (!isDirectory())
    return -1;
  
  int entryCount = 0;

  fat_dir_struct *dd = fat_open_dir(SDCard.fs, &dir_entry);
  if (dd == NULL)
    return -1;
  
  while (fat_read_dir(dd, &entries[entryCount].dir_entry) && (entryCount < maxCount)) {
    entries[entryCount].setFromParentEntry(this);
    entryCount++;
  }
  
  fat_close_dir(dd);
  return entryCount;
}

void SDCardEntry::setFromParentEntry(SDCardEntry *parent) {
  m_strncpy(dir, parent->dir, sizeof(dir));
  m_strnappend(dir, parent->dir_entry.long_name, sizeof(dir));
  
  m_strncpy(name, dir_entry.long_name, sizeof(name));
  exists = true;
}

bool SDCardEntry::createSubDirectory(char *path, struct fat_dir_entry_struct *new_entry) {
  if (path[0] == '/')
    path++;

  fat_dir_struct *dd = fat_open_dir(SDCard.fs, &dir_entry);
  if (dd == NULL)
    return false;
  
  char subDir[64];
  while (1) {
    char *pos = strchr(path, '/');
    if (pos == NULL) {
      m_strncpy(subDir, path, sizeof(subDir) - 1);
    } else {
      int len = pos - path;
      memcpy(subDir, path, len);
      subDir[len] = '\0';
      path = pos + 1;
    }
    
    struct fat_dir_entry_struct new_dir_entry;
    
    int result = fat_create_dir(dd, subDir, &new_dir_entry);

    if (result == 0 && strcmp(subDir, new_dir_entry.long_name)) {
      fat_close_dir(dd);
      return false;
    } else {
      memcpy(new_entry, &new_dir_entry, sizeof(new_dir_entry));
      fat_close_dir(dd);
      dd = fat_open_dir(SDCard.fs, &new_dir_entry);

      if (dd == NULL) {
	return false;
      } else {
	if (pos == NULL) {
	  sd_raw_sync();
	  fat_close_dir(dd);
	  return true;
	}
      }
    }
  }
}

bool SDCardEntry::deleteEntry(bool recursive) {
  if (!isDirectory()) {
    return fat_delete_file(SDCard.fs, &dir_entry);
  } else {
    return false;
  }
}

/******************* SD CARD *****************************/

bool SDCardFile::open(bool create) {
#if 0
  if (create) {
    if (!fat_create_file(SDCard.current_dir, path, &dir_entry)
	&& strcmp(path, file_dir_entry.long_name))
      return false;
  }
  
 fd = fat_open_file(SDCard.fs, &file_dir_entry);
  if (fd != NULL)
    return true;
  else
    return false;
#endif
  return false;
}

void SDCardFile::close() {
  if (fd != NULL) {
    fat_close_file(fd);
    sd_raw_sync();
    fd = NULL;
  }
}

intptr_t SDCardFile::read(uint8_t *buf, uint8_t len) {
  if (fd == NULL)
    return -1;

  return fat_read_file(fd, buf, len);
}

intptr_t SDCardFile::write(uint8_t *buf, uint8_t len) {
  if (fd == NULL)
    return -1;

  return fat_write_file(fd, buf, len);
}

bool SDCardFile::seek(int32_t *offset, uint8_t whence) {
  if (fd == NULL)
    return -1;

  if (fat_seek_file(fd, offset, whence))
    return true;
  else
    return false;
}

