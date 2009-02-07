enum {
  MD_NONE,
  
  MD_GET_CURRENT_KIT,
  MD_GET_KIT,

  MD_GET_CURRENT_GLOBAL,
  MD_GET_GLOBAL,
  
  MD_DONE
} mdNotesSysexStatus = MD_NONE;

void setupMDSysex() {
  MDSysex.setup();
  MDSysex.setOnStatusResponseCallback(onStatusResponseCallback);
  MDSysex.setOnKitMessageCallback(onKitMessageCallback);
  MDSysex.setOnGlobalMessageCallback(onGlobalMessageCallback);
}

void onStatusResponseCallback(uint8_t type, uint8_t value) {
  if (type == MD_CURRENT_KIT_REQUEST && 
      mdNotesSysexStatus == MD_GET_CURRENT_KIT) {
    mdNotesSysexStatus = MD_GET_CURRENT_GLOBAL;
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_GLOBAL_SLOT_REQUEST);
  } else if (type == MD_CURRENT_GLOBAL_SLOT_REQUEST && 
             mdNotesSysexStatus == MD_GET_CURRENT_GLOBAL) {
    mdNotesSysexStatus = MD_GET_KIT;
    MD.sendRequest(MD_KIT_REQUEST_ID, MD.currentKit);
  } else {
    LCD.line1("UNKNOWN");
  }
}

void onGlobalMessageCallback(md_global_t *global) {
  if (mdNotesSysexStatus == MD_GET_GLOBAL) {
    MD.baseChannel = global->baseChannel;
    mdNotesSysexStatus = MD_DONE;
    LCD.line1("FINISH");
  }
}

void onKitMessageCallback(md_kit_t *kit) {
  if (mdNotesSysexStatus == MD_GET_KIT) {
    m_memcpy(MD.name, kit->name, sizeof(MD.name));
    m_memcpy(MD.trackModels, kit->trackModels, sizeof(MD.trackModels));
    mdNotesSysexStatus = MD_GET_GLOBAL;
    MD.sendRequest(MD_GLOBAL_REQUEST_ID, MD.currentGlobal);
  }
}

void handleGuiSysex() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    mdNotesSysexStatus = MD_GET_CURRENT_KIT;
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_KIT_REQUEST);
  }
}
