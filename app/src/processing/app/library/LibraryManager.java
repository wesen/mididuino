/*
  LibraryManager.java - Library System for Wiring
  Copyright (c) 2006-07 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

package processing.app.library;

import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Vector;

import javax.swing.JMenu;

import processing.app.Base;
import processing.app.Preferences;
import processing.app.debug.RunnerException;
import processing.app.debug.Target;

/*
 * Provides information about and builds libraries
 */
public class LibraryManager {

  private File libDir;
  private List libraries = new ArrayList();
  private Target target;

  /*
   * Create a LibraryManager.
   */
  public LibraryManager() throws IOException
  {
    libDir = new File(Base.getLibrariesPath());
    target = new Target(Base.getHardwarePath() + File.separator + "cores",
                        Preferences.get("boards." + Preferences.get("board") + ".build.core"));
    refreshLibraries();
  }

  public Target getTarget()
  {
    return target;
  }

  /*
   * Scans for libraries and refreshes internal list 
   */
  private void refreshLibraries()
  {
    FileFilter onlyDirs = new FileFilter() {
      public boolean accept(File file) {
        return file.isDirectory();
      }
    };
    libraries.clear();
    File[] libs = libDir.listFiles(onlyDirs);
    for(int i = 0; i < libs.length; ++i){
      libraries.add(new Library(this, libs[i]));
    }
  }

  /*
   * Returns a collection of all library objects
   * @return A read-only collection of Library objects
   */
  public Collection getAll() {
    refreshLibraries();
    return Collections.unmodifiableList(libraries);
  }

  public Library get(String name) {
    for (int i = 0; i < libraries.size(); i++) {
      if (name.equals(((Library)libraries.get(i)).getName())) {
        return (Library)libraries.get(i);
      }
    }
    return null;
  }

  /*
   * Returns a collection of all built library objects
   * @return A read-only collection of built Library objects
   */
  public Collection getBuiltLibraries() {
    refreshLibraries();
    List builtLibraries = new ArrayList();
    Library library;
    ListIterator libIterator = libraries.listIterator();
    while(libIterator.hasNext()){
      library = (Library)libIterator.next();
      if(library.isBuilt()){
        builtLibraries.add(library);
      }
    }
    return Collections.unmodifiableList(builtLibraries);
  }

  /*
   * Returns a collection of all buildable library objects
   * @return A read-only collection of built Library objects
   */
  public Collection getLibrariesToBuild() {
    refreshLibraries();
    List buildableLibraries = new ArrayList();
    Library library;
    ListIterator libIterator = libraries.listIterator();
    while(libIterator.hasNext()){
      library = (Library)libIterator.next();
      if(library.isUnbuiltBuildable()){
        buildableLibraries.add(library);
      }
    }
    return Collections.unmodifiableList(buildableLibraries);
  }

  /*
   * Rebuilds built libraries
   * @return Number of libraries built as int, -1 & exception on error
   */
  public int rebuildAllBuilt() throws RunnerException {
    Collection builtLibraries = getBuiltLibraries();
    Library library;
    Iterator libIterator = builtLibraries.iterator();
    int countBuilt = 0;
    while(libIterator.hasNext()){
      library = (Library)libIterator.next();
      // System.out.println("Building library \"" + library.getName() + "\"");
      try {
        if(library.build()){
          ++countBuilt;
        }else{
          return -1;
        }
      }catch (RunnerException re) {
        throw new RunnerException(re.getMessage());
      } catch (Exception ex) {
        throw new RunnerException(ex.toString());
      }
    }
    return countBuilt;
  }

    /*
   * Gathers filenames of header files
   * @return Array of strings of filenames of header files
   */
  public String[] getHeaderFiles() {
    ArrayList filesArrayList = new ArrayList();
    Collection builtLibraries = getBuiltLibraries();
    Library library;
    File[] files;
    Iterator libIterator = builtLibraries.iterator();
    while(libIterator.hasNext()){
      library = (Library)libIterator.next();
      files = library.getHeaderFiles();
      for(int i = 0; i < files.length; ++i){
        filesArrayList.add(files[i].getName());
      }
    }
    String[] filesArray = new String[filesArrayList.size()];
    filesArrayList.toArray(filesArray); 
    return filesArray;
  }
  
  public Library getLibraryWithFile(String fileName) {
    Collection<Library> builtLibraries = getBuiltLibraries();
    for (Library library : builtLibraries) {
      if (library.hasHeaderFile(fileName)) {
        return library;
      }
    }
    return null;
  }

  /*
   * Gathers paths to library folders
   * @return Array of strings of paths to library folders
   */
  public String[] getFolderPaths() {
    ArrayList foldersArrayList = new ArrayList();
    //Collection builtLibraries = getBuiltLibraries();
    Collection libraries = getAll();
    Library library;
    //Iterator libIterator = builtLibraries.iterator();
    Iterator libIterator = libraries.iterator();
    while(libIterator.hasNext()){
      library = (Library)libIterator.next();
      foldersArrayList.add(library.getFolder().getPath());
    }
    String[] foldersArray = new String[foldersArrayList.size()];
    foldersArrayList.toArray(foldersArray);
    return foldersArray;
  }

  /*
   * Builds unbuilt libraries
   * @return Number of libraries built as int, -1 & exception on error
   */
  public int buildAllUnbuilt() throws RunnerException {
    Collection buildableLibraries = getLibrariesToBuild();
    Library library;
    Iterator libIterator = buildableLibraries.iterator();
    int countBuilt = 0;
    while(libIterator.hasNext()){
      library = (Library)libIterator.next();
      //      System.out.println("Building library \"" + library.getName() + "\"");
      try {
        if(library.build()){
          ++countBuilt;
        }else{
          return -1;
        }
      }catch (RunnerException re) {
        throw new RunnerException(re.getMessage());
      } catch (Exception ex) {
        throw new RunnerException(ex.toString());
      }
    }
    return countBuilt;
  }

  /*
   * Populates examples menu with library folders
   */
  public void populateExamplesMenu(JMenu examplesMenu, ActionListener listener) {
    Library library;
    Collection libraries = getBuiltLibraries();
    Iterator iterator = libraries.iterator();
    JMenu libraryExamples;
    while(iterator.hasNext()){
      library = (Library)iterator.next();
      libraryExamples = library.getExamplesMenu(listener);
      if(null != libraryExamples){
        examplesMenu.add(libraryExamples);
      }
    }
  }

  public void addLibrary(Vector libraries, Library library) {
    if (!libraries.contains(library)) {
      libraries.add(library);
      System.out.println("Adding library " + library.getName());
      String depends[] = library.getDepends();
      for (int bla = 0; bla < depends.length; bla++) {
        Library dependLibrary = get(depends[bla]);
        if (!libraries.contains(dependLibrary)) {
          System.out.println("adding depended on library " + dependLibrary.getName());
          addLibrary(libraries, dependLibrary);
        }
      }
    }
  }

}
