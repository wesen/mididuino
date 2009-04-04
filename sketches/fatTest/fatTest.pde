#include <SDCard.h>

RangeEncoder enc(0, 127, "HEL");
EncoderPage page(&enc, NULL, &enc, NULL);

struct sd_raw_info info = { 0 } ;

void setup() {
  GUI.setPage(&page);
  SET_BIT(DDRD, PD4);
  SET_BIT(PORTD, PD4);
  if (sd_raw_init() == 0) {
    GUI.put_string_at_fill(0, "SD RAW FAILED");
    return;
  } 
  if (sd_raw_get_info(&info) == 0) {
    GUI.put_string_at_fill(0, "SD INFO FAILED");
    return;
  }
//  GUI.put_value16(0, (info.capacity >> 16));
//  GUI.put_value16(1, (info.capacity & 0xFFFF));
  
  struct partition_struct* partition = partition_open(sd_raw_read,
                                                      sd_raw_read_interval,
                                                      sd_raw_write,
                                                      sd_raw_write_interval,
                                                      0
                                                      );
  if(!partition) {
    GUI.put_string_at_fill(0, "PARTITION FAIL");
    return;
  }
  
  struct fat_fs_struct* fs = fat_open(partition);
  if(!fs) {
    GUI.put_string_at_fill(0, "FS FAIL");
    return;
  }
  
  struct fat_dir_entry_struct directory;
  fat_get_dir_entry_of_path(fs, "/", &directory);
  
  struct fat_dir_struct* dd = fat_open_dir(fs, &directory);
  if(!dd) {
    GUI.put_string_at_fill(0, "DIR FAIL");
    return;
  }
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

void handleGui() {
}
