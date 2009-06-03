bool writeSettings() {
  SDCardFile file;
  if (!file.create("/MidiController/settings")) {
    return false;
  }
  
  if (file.write((unsigned char *)ccs, sizeof(ccs)) != sizeof(ccs))
    goto end;
  if (file.write((unsigned char *)channels, sizeof(channels)) != sizeof(channels))
    goto end;
  if (file.write((unsigned char *)mins, sizeof(mins)) != sizeof(mins))
    goto end;
  if (file.write((unsigned char *)maxs, sizeof(maxs)) != sizeof(maxs))
    goto end;
  if (file.write((unsigned char *)names, sizeof(names)) != sizeof(names))
    goto end;
  file.close();
  return true;

end:
  file.close();
  return false;
}

bool loadSettings() {
  SDCardFile file;
  if (!file.open("/MidiController/settings")) {
    initSettings();
    writeSettings();
    return false;
  }
  
  if (file.read((unsigned char *)ccs, sizeof(ccs)) != sizeof(ccs))
    goto end;
  if (file.read((unsigned char *)channels, sizeof(channels)) != sizeof(channels))
    goto end;
  if (file.read((unsigned char *)mins, sizeof(mins)) != sizeof(mins))
    goto end;
  if (file.read((unsigned char *)maxs, sizeof(maxs)) != sizeof(maxs))
    goto end;
  if (file.read((unsigned char *)names, sizeof(names)) != sizeof(names))
    goto end;
  
  file.close();
  initEncoders();
  return true;
  
end:
  file.close();
  initSettings();
  writeSettings();
  return false;
}

