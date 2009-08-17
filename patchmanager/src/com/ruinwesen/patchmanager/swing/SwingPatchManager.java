/**
 * Copyright (c) 2009, Christian Schneider
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the names of the authors nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
package com.ruinwesen.patchmanager.swing;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.MouseEvent;
import java.awt.event.WindowEvent;
import java.io.File;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.MissingResourceException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.ProgressMonitor;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import name.cs.csutils.CSAction;
import name.cs.csutils.CSEventAdapter;
import name.cs.csutils.CSFileSelectionContext;
import name.cs.csutils.CSProperties;
import name.cs.csutils.CSUtils;
import name.cs.csutils.SwingActionData;
import name.cs.csutils.SynchronizedField;
import name.cs.csutils.collector.Collector;
import name.cs.csutils.concurrent.SimpleSwingWorker;
import name.cs.csutils.debug.Debug;
import name.cs.csutils.debug.DebugFactory;
import name.cs.csutils.i18n.I18N;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.patch.DefaultPatch;
import com.ruinwesen.patchmanager.client.PatchManager;
import com.ruinwesen.patchmanager.client.index.IndexedPatch;
import com.ruinwesen.patchmanager.client.index.NewsIndex;
import com.ruinwesen.patchmanager.client.index.PatchIndex;
import com.ruinwesen.patchmanager.client.protocol.Auth;
import com.ruinwesen.patchmanager.client.protocol.PatchManagerClient;
import com.ruinwesen.patchmanager.client.protocol.RequestRegisterNewUser;
import com.ruinwesen.patchmanager.client.repository.ClientRepository;
import com.ruinwesen.patchmanager.swing.components.SearchOptionsControl;
import com.ruinwesen.patchmanager.swing.components.PatchDetailsView;
import com.ruinwesen.patchmanager.swing.components.PatchListCellRenderer;
import com.ruinwesen.patchmanager.swing.components.SearchPanel;
import com.ruinwesen.patchmanager.swing.forms.AboutDialog;
import com.ruinwesen.patchmanager.swing.forms.BugreportForm;
import com.ruinwesen.patchmanager.swing.forms.NewLoginForm;
import com.ruinwesen.patchmanager.swing.model.Order;
import com.ruinwesen.patchmanager.swing.model.PatchListModel;
import com.ruinwesen.patchmanager.swing.tasks.BugReportBuilder;
import com.ruinwesen.patchmanager.swing.tasks.CheckForUpdatesTask;
import com.ruinwesen.patchmanager.swing.tasks.DeletePatchTask;
import com.ruinwesen.patchmanager.swing.tasks.GetLatestNewsTask;
import com.ruinwesen.patchmanager.swing.tasks.PerformQueryTask;
import com.ruinwesen.patchmanager.swing.tasks.SynchronizeRepositoryTask;
import com.ruinwesen.patchmanager.swing.wizards.CreatePublishPatchWizard;
import com.ruinwesen.patchmanager.swing.wizards.MidiSendWizard;

public class SwingPatchManager {

	private static 
	final Debug debug = DebugFactory.getDebug(SwingPatchManager.class);

	private static Log log = LogFactory.getLog(SwingPatchManager.class);

	public static boolean adminMode = false;

	public static final String KEY_LAST_SELECTED_FILE
	= SwingPatchManager.class.getName()+".last-selected-file";
	public static final String KEY_SIDEBAR_FILTER_VISIBLE
	= SwingPatchManager.class.getName()+".sidebar.filter.visible";
	public static final String KEY_SIDEBAR_PATCH_DETAILS_VISIBLE
	= SwingPatchManager.class.getName()+".sidebar.patch-details.visible";

	private JFrame frame = null;
	private PatchManager patchmanager;
	private File applicationUserdataDir;
	public File getApplicationUserdataDir() {
		return applicationUserdataDir;
	}

	private File applicationPropertiesFile;
	private CSProperties appProperties = new CSProperties();

	private JPanel leftPanel;
	private JList patchListView;
	private JScrollPane scrollPatchList;
	private JScrollPane scrollPatchDetails;
	private PatchListModel<IndexedPatch> patchListModel;
	private PatchListCellRenderer patchListCellRenderer;
	private SearchPanel searchPanel;
	private EventHandler eventHandler ;
	private SearchOptionsControl searchOptionsControl;
	private PatchDetailsView patchDetailsView;
	private NewsIndex newsIndex;
	private JPanel leftPanelHeader;
	private JPanel rightPanelHeader;
	private JScrollPane scrollSearchOptions;

	private JLabel lblStatusBar;

	private String version = "<undefined>";
	private String build_number = "<undefined>";
	private String build_timestamp = "<undefined>";

	private CSAction acPatchListViewSendSelectedAction;
	private CSAction acPatchListViewDeleteSelectedAction;
	private CSAction acPatchListViewSelectedSaveMidiFileAction;
	private CSAction acPatchListViewSelectedSaveSourceAction;

	private CSAction acToggleFilterSidebarVisibility;
	private CSAction acTogglePatchDetailsSidebarVisibility;

	private ExecutorService executorService = 
		Executors.newFixedThreadPool(Runtime.getRuntime().availableProcessors()+2);

	private SynchronizedField<String> statusTextField = new SynchronizedField<String>(" ") {
		@Override
		public void update(String value) {
			lblStatusBar.setText(value == null ? " " : (" "+value));
		}
	};

	private JPanel rightPanel;

	private JSplitPane center_right_split;

	private JPanel centerPanel;

	public ExecutorService getExecutorService() {
		return executorService;
	}

	public void runInThread(Runnable r) {
		executorService.execute(r);
	}

	public String getVersion() {
		return version;
	}

	public String getBuildNumber() {
		return build_number;
	}

	public String getBuildTimestamp() {
		return build_timestamp;
	}

	public void start() {
		if (frame != null) {
			return;
		}
		init();
	}

	private void loadVersionInfo() {
		CSProperties versionInfo = CSUtils.loadProperties(getClass().getResource("/version.properties"));
		this.build_number = versionInfo.getProperty("patchmanager.build.number", "<undefined>");
		this.build_timestamp = versionInfo.getProperty("patchmanager.build.timestamp", "<undefined>");
		this.version = versionInfo.getProperty("patchmanager.version", "<undefined>");

		// always print this
		log.info("Patchmanager Version:"+version
				+", Build id:"+build_number
				+", Build time:"+build_timestamp);
	}

	public NewsIndex getNewsIndex() {
		return newsIndex;
	}

	public PatchManagerClient getPatchManagerClient() {
		return patchmanager.getPatchManagerClient();
	}

	public JFrame getFrame() {
		return frame;
	}

	public CSProperties getAppProperties() {
		return appProperties;
	}

	private void writeAppProperties() {
	    appProperties.remove(KEY_LAST_SELECTED_FILE);
	    File file = CSFileSelectionContext.getDefaultContext().getFile();
	    if (file != null) {
	        appProperties.put(KEY_LAST_SELECTED_FILE, file.getAbsolutePath());
	    }
		CSUtils.storeProperties(appProperties, applicationPropertiesFile);
	}

	private void init() {
		loadVersionInfo();


		// load translations+keybindings,...
		try {
			I18N.getSharedInstance().setResourceBundleName("resources/lang/MessageBundle");
		} catch (MissingResourceException ex) {
			if (log.isErrorEnabled()) {
				log.error("MessageBundle resource not found", ex);
			}
		}

		applicationUserdataDir = CSUtils.getApplicationConfigDir("patchmanager");
		applicationPropertiesFile = new File(applicationUserdataDir, "config.properties");



		// load application properties
		appProperties.putAll(CSUtils.loadProperties(applicationPropertiesFile));
		String lastSelectedFileName = appProperties.getProperty(KEY_LAST_SELECTED_FILE);
		if (lastSelectedFileName != null) {
		    CSFileSelectionContext.getDefaultContext().setFile(new File(lastSelectedFileName));
		}
		

		patchmanager = new PatchManager(applicationUserdataDir);

		frame = new JFrame("PatchManager");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setLocationByPlatform(true);



		acToggleFilterSidebarVisibility =
			new CSAction("Search Options")
		.useResourceKey("action.sidebar.filter.toggle-visibility")
		.useInvokationTarget(this, "toggleFilterSidebarVisibility")
		.setSelected(true);

		acTogglePatchDetailsSidebarVisibility =
			new CSAction("Patch Details")
		.useResourceKey("action.sidebar.patch-details.toggle-visibility")
		.useInvokationTarget(this, "togglePatchDetailsSidebarVisibility")
		.setSelected(true);

		acPatchListViewSendSelectedAction = 
			new CSAction("Send Patch")
		.useResourceKey("action.patch.send-to-device")
		.useInvokationTarget(this, "patchListViewSendSelected")
		.setAsEnabled(false);

        acPatchListViewDeleteSelectedAction = 
            new CSAction("Delete Patch")
        .useResourceKey("action.patch.delete")
        .useInvokationTarget(this, "patchListViewDeleteSelected")
        .setAsEnabled(false);
        acPatchListViewSelectedSaveMidiFileAction = 
            new CSAction("Save Midifile...")
        .useResourceKey("patchdetails.midifile.saveas")
        .useInvokationTarget(this, "patchListViewSelectedSaveMidiFile")
        .setAsEnabled(false);
        acPatchListViewSelectedSaveSourceAction = 
            new CSAction("Save Source...")
        .useResourceKey("patchdetails.sourcecode.saveas")
        .useInvokationTarget(this, "patchListViewSelectedSaveSource")
        .setAsEnabled(false);
        
		JMenuBar jmenubar = new JMenuBar();
		JMenu menuFile = new JMenu(new CSAction("File", "menu.file"));
		menuFile.add(new CSAction("Send Patch...", "menu.send-patch").useInvokationTarget(this, "sendPatch"));
		menuFile.add(new CSAction("Exit", "menu.exit").useInvokationTarget(this, "quit"));
		jmenubar.add(menuFile);
		JMenu menuEdit = new JMenu(new CSAction("Edit","menu.edit")) ;
		// selected patch tasks
		if (SwingPatchManager.adminMode) {
			menuEdit.add(acPatchListViewSendSelectedAction);
			menuEdit.add(acPatchListViewDeleteSelectedAction);
			menuEdit.add(acPatchListViewSelectedSaveMidiFileAction);
			menuEdit.add(acPatchListViewSelectedSaveSourceAction);
			menuEdit.addSeparator(); // server tasks
			menuEdit.add(new CSAction(this, "publishPatch"));
			menuEdit.addSeparator(); // misc server tasks
			if (debug.isDebugEnabled()) {
				menuEdit.add(new CSAction(this, "getLatestNews").useResourceKey("action.check-news"));
			}
		}
		menuEdit.add(new CSAction(this, "syncRepository"));

		jmenubar.add(menuEdit);
		JMenu menuView = new JMenu("View") ;
		menuView.add(new JCheckBoxMenuItem(acToggleFilterSidebarVisibility));
		menuView.add(new JCheckBoxMenuItem(acTogglePatchDetailsSidebarVisibility));
		jmenubar.add(menuView);

		JMenu menuHelp = new JMenu("Help") ;
		menuHelp.add(new CSAction(this, "regenerateIndex")
		.useResourceKey("action.index.regenerate"));
		if (debug.isDebugEnabled()) {
			menuHelp.add(new CSAction(this, "askServerAboutNewClientVersion")
			.useResourceKey("action.check-for-updates"));
		}

		menuHelp.add(new CSAction(this, "showAboutDialog"));

		jmenubar.add(menuHelp);

		if (debug.isDebugEnabled()) {
			JMenu menuDebug = new JMenu("Debug") ;
			menuDebug.add(new CSAction(this, "sendBugreport"));
			menuDebug.add(new CSAction(this, "debugRunNeverEndingTask"));
			jmenubar.add(menuDebug);
		}

		frame.setJMenuBar(jmenubar);

		patchListCellRenderer = new PatchListCellRenderer();

		searchPanel = new SearchPanel(this);

		patchListModel = new PatchListModel<IndexedPatch>(Order.BY_RELEVANCE);
		patchListView = new JList(patchListModel);
		patchListView.setCellRenderer(patchListCellRenderer);
		patchListView.setFixedCellHeight(patchListCellRenderer.computeFixedCellHeight());
		patchListView.setVisibleRowCount(5);

		scrollPatchList = new JScrollPane(patchListView);

		JPopupMenu listPopup = new JPopupMenu();
		listPopup.add(acPatchListViewSendSelectedAction);

        if (SwingPatchManager.adminMode) {
		listPopup.add(acPatchListViewSelectedSaveMidiFileAction);
		listPopup.add(acPatchListViewSelectedSaveSourceAction);
		listPopup.addSeparator();
		listPopup.add(acPatchListViewDeleteSelectedAction);
        }

		patchListView.setComponentPopupMenu(listPopup);


		//   updatePatchListViewFixedWidth();

		searchOptionsControl = new SearchOptionsControl();
		scrollSearchOptions = new JScrollPane(searchOptionsControl.getContainer());

		centerPanel = new JPanel(new BorderLayout());
		centerPanel.add(searchPanel.getContainer(), BorderLayout.NORTH);
		centerPanel.add(scrollPatchList, BorderLayout.CENTER);

		leftPanel = new JPanel(new BorderLayout());
		leftPanelHeader = new JPanel(new BorderLayout());
		leftPanelHeader.setBorder(BorderFactory.createEmptyBorder(2,5,2,5));
		leftPanelHeader.add(new JLabel("Search"),BorderLayout.CENTER);
		leftPanel.add(leftPanelHeader, BorderLayout.NORTH);
		leftPanel.add(scrollSearchOptions, BorderLayout.CENTER);

		rightPanel = new JPanel(new BorderLayout());
		rightPanelHeader = new JPanel(new BorderLayout());
		rightPanelHeader.setBorder(BorderFactory.createEmptyBorder(2,5,2,5));
		rightPanelHeader.add(new JLabel("Patch"),BorderLayout.CENTER);
		rightPanelHeader.add(new JButton(acPatchListViewSendSelectedAction),BorderLayout.EAST);
		rightPanel.add(rightPanelHeader, BorderLayout.NORTH);

		patchDetailsView = new PatchDetailsView();
		patchDetailsView.getSaveMidifileButton().setAction(acPatchListViewSelectedSaveMidiFileAction);
		patchDetailsView.getSaveSourceButton().setAction(acPatchListViewSelectedSaveSourceAction);

		scrollPatchDetails = new JScrollPane(patchDetailsView.getContainer());
		rightPanel.add(scrollPatchDetails, BorderLayout.CENTER);

		center_right_split = createSplitPane();
		center_right_split.setContinuousLayout(true);
		/*
        patchmanager.getIndex().readIndex(null);
        try {
        copyPatch800();
        } catch (Exception ex){
            ex.printStackTrace();
        }*/
		lblStatusBar = new JLabel(" ");


		frame.getContentPane().setLayout(new BorderLayout());
		frame.getContentPane().add(leftPanel, BorderLayout.WEST);
		frame.getContentPane().add(center_right_split, BorderLayout.CENTER);
		frame.getContentPane().add(lblStatusBar, BorderLayout.SOUTH);

		eventHandler = new EventHandler();
		eventHandler.install();


		if (!appProperties.getBooleanProperty(
				KEY_SIDEBAR_FILTER_VISIBLE, true)) {
			acToggleFilterSidebarVisibility.setSelected(false);
			toggleFilterSidebarVisibility();
		}
		if (!appProperties.getBooleanProperty(
				KEY_SIDEBAR_PATCH_DETAILS_VISIBLE, true)) {
			acTogglePatchDetailsSidebarVisibility.setSelected(false);
			togglePatchDetailsSidebarVisibility();
		}

		this.newsIndex = new NewsIndex(new File(applicationUserdataDir, "news.index"));
		//      frame.setMinimumSize(new Dimension(600,300));
		frame.pack();

		{

			int headerHeight = searchPanel.getContainer().getHeight();
			{
				int lw = leftPanelHeader.getWidth();
				leftPanelHeader.setPreferredSize(new Dimension(lw, headerHeight));
			}
			{
				int rw = rightPanelHeader.getWidth();
				rightPanelHeader.setPreferredSize(new Dimension(rw, headerHeight));
			}
		}

		Dimension size = frame.getSize();
		size.width+=3*20;
		frame.setSize(size);

		frame.setVisible(true);
		
		runInThread(new SynchronizeRepositoryTask(this));

		indexUpdated();

	}

	@SwingActionData("Run Never Ending Task")
	public void debugRunNeverEndingTask() {
		runInThread(new Runnable () {
			public void run() {
				if (log.isDebugEnabled()) {
					log.debug(this+": Never Ending thread started.");
				}
				try {
					while (true) {
						Thread.sleep(Long.MAX_VALUE);
					}
				} catch (InterruptedException ex) {
					if (log.isDebugEnabled()) {
						log.debug(this+": Never Ending thread was interrupted.");
					}
					Thread.currentThread().interrupt();
				}
			}
		});
	}

	@SwingActionData("About Patchmanager")
	public void showAboutDialog() {
		AboutDialog.showDialog(this);
	}

	private JSplitPane createSplitPane() {
		JSplitPane split = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
				false, centerPanel, rightPanel);
		split.setResizeWeight(1);
		return split;
	}

	public void toggleFilterSidebarVisibility() {
		leftPanel.setVisible(!leftPanel.isVisible());

		acToggleFilterSidebarVisibility.setSelected(leftPanel.isVisible());
		appProperties.setBooleanProperty(KEY_SIDEBAR_FILTER_VISIBLE, acToggleFilterSidebarVisibility.isSelected());

		if (leftPanel.isVisible()) {
			//  center_right_split.setDividerLocation(center_right_split.getLastDividerLocation());
		}
		frame.getContentPane().invalidate();
		if (frame.getContentPane() instanceof JComponent) {
			((JComponent)frame.getContentPane()).revalidate();
		}
		frame.getContentPane().repaint();
	}

	public void togglePatchDetailsSidebarVisibility() {
		boolean sidebarVisible;
		if (center_right_split != null) { // hide patch details
			sidebarVisible = false;
			center_right_split.removeAll();
			frame.getContentPane().remove(center_right_split);
			center_right_split = null;
			frame.getContentPane().add(centerPanel, BorderLayout.CENTER);
		} else { // show patch details
			sidebarVisible = true;
			frame.getContentPane().remove(centerPanel);
			center_right_split = createSplitPane();
			frame.getContentPane().add(center_right_split, BorderLayout.CENTER);
		}
		centerPanel.invalidate();
		centerPanel.revalidate();
		centerPanel.repaint();

		frame.getContentPane().invalidate();
		if (frame.getContentPane() instanceof JComponent)
			((JComponent)frame.getContentPane()).revalidate();
		frame.getContentPane().repaint();


		appProperties.setBooleanProperty(KEY_SIDEBAR_PATCH_DETAILS_VISIBLE, sidebarVisible);

	}

	public void patchListViewSendSelected() {
		IndexedPatch patch = (IndexedPatch)patchListView.getSelectedValue();
		if (patch == null) {
			return;
		}

		File file = patch.getLocalFile();
		if (file == null) {
			return;
		}        

		MidiSendWizard send = new MidiSendWizard();
		send.setProperties(appProperties);
		send.setSourceFile(file);
		send.setSourceFileSelectable(false);
		send.showDialog(frame);
	}

	public void sendPatch() {
		MidiSendWizard send = new MidiSendWizard();
		send.setProperties(appProperties);
		send.showDialog(frame);
	}

	public void patchListViewDeleteSelected() {
		IndexedPatch patch = (IndexedPatch)patchListView.getSelectedValue();
		if (patch == null) {
			return;
		}

		if (JOptionPane.showConfirmDialog(frame, "Are you sure you want to delete the patch ?",
				"Delete",
				JOptionPane.YES_NO_OPTION)
				== JOptionPane.YES_OPTION) {
			runInThread(new DeletePatchTask(this, patch));
		}
	}

	public void patchListViewSelectedSaveMidiFile() {
		IndexedPatch patch = (IndexedPatch)patchListView.getSelectedValue();
		if (patch == null) {
			return;
		}
		SwingPatchManagerUtils.showStoreMidiFileDialog(frame, patch);    
	}


	public void patchListViewSelectedSaveSource() {
		IndexedPatch patch = (IndexedPatch)patchListView.getSelectedValue();
		if (patch == null) {
			return;
		}
		SwingPatchManagerUtils.showStoreSourceDialog(frame, patch);    
	}
	public void quit() {
		frame.setVisible(false);
		frame.dispose();
	}

	public void setStatus(String text) {
		statusTextField.set(text);
	}

	public void indexUpdated() {
		runInThread(new IndexUpdateThread(false));
	}

	@SwingActionData("Regenerate Index")
	public void regenerateIndex() {
		runInThread(new IndexUpdateThread(true));
	}

	public void setSearchRelatedControlsEnabled(boolean value) {
		searchOptionsControl.setControlsEnabled(value);

		searchPanel.getSearchField().setEditable(value);
		searchPanel.getSearchField().setEnabled(value);
	}

	public final class IndexUpdateThread extends SimpleSwingWorker implements
	Collector<IndexedPatch> {
		private static final long serialVersionUID = 6481174902097150715L;
		private ProgressMonitor pm;
		private int counter = 0;
		private boolean regenerate;
		private static final int CANCELED = -1;

		public IndexUpdateThread(boolean regenerate) {
			this.regenerate = regenerate;
		}

		@Override
		protected void setup() {
			setSearchRelatedControlsEnabled(false);
			// this should be done in a thread
			int count = patchmanager.getIndex().computeEstimatedPatchCount();
			pm = new ProgressMonitor(frame, "Reading index. ("+count+" patch files)", "This may take a while.", 0, count);
		}

		@Override
		protected void finish() {
			setStatus("Finished reading index.");
			pm.close();
			// TODO we can rebuild the filter ui using the thread
			searchOptionsControl.rebuildFilterUI(patchmanager.getIndex());
			executeQuery();
		}

		@Override
		protected void cleanup() {
			setSearchRelatedControlsEnabled(true);
		}

		@Override
		protected void update() {
			pm.setProgress(++counter);
			if (pm.isCanceled()) {
				synchronized (this) {
					counter = CANCELED;
				}
			}
		}

		@Override
		public void process() {
			if (regenerate) {
				setStatus("Regenerating index...");
				patchmanager.getIndex().rebuildIndex();
			}
			setStatus("Reading index...");
			patchmanager.getIndex().readIndex(this);
		}

		@Override
		public int collect(IndexedPatch item) {
			synchronized (this) {
				if (counter == CANCELED) {
					return Collector.FINISHED;
				}
				++counter;
			}
			callUpdate();
			return Collector.ACCEPTED_MORE;
		}
	}

	@SwingActionData("Read News")
	public void getLatestNews() {
		runInThread(new GetLatestNewsTask(this));
	}

	public void executeQuery() {
		runInThread(new PerformQueryTask(this));
	}

	@SwingActionData("Download patches")
	public void syncRepository() {
		runInThread(new SynchronizeRepositoryTask(this));
	}

	public Auth getUserAuthentication(boolean allowCreate) {
		final String KEY_USERNAME = "client.username";
		final String KEY_PASSWORD = "client.password";
		final String KEY_MAIL = "client.mail";

		String username = appProperties.getProperty(KEY_USERNAME);
		String password = appProperties.getProperty(KEY_PASSWORD);
		String mail = appProperties.getProperty(KEY_MAIL);

		if (username != null && password != null && mail != null) {
			return new Auth(username, password, mail);
		}
		if (!allowCreate) {
			return null;
		}
		// create login
		NewLoginForm nlf = new NewLoginForm(patchmanager.getPatchManagerClient(), 
				NewLoginForm.OPTION_CREATE_USER_OR_CANCEL);

		nlf.showDialog(frame);

		if (nlf.getSelectedOption() != JOptionPane.OK_OPTION) {
			return null;
		}

		username = nlf.getUsername();
		password = nlf.getPassword();
		mail = nlf.getMail();

		appProperties.put(KEY_USERNAME, username);
		appProperties.put(KEY_PASSWORD, password);
		appProperties.put(KEY_MAIL, mail);
		writeAppProperties();

		return new Auth(username, password, mail);
	}

	@SwingActionData("Create/Publish Patch...")
	public void publishPatch() {
		new CreatePublishPatchWizard(this).showDialog(frame);
	}

	@SwingActionData("Check for Updates")
	public void askServerAboutNewClientVersion() {
		CheckForUpdatesTask task = new CheckForUpdatesTask(this);
		task.setQuietModeEnabled(false);
		runInThread(task);
	}

	@SwingActionData("Send Bug Report")
	public void sendBugreport() {
		BugReportBuilder brb = new BugReportBuilder();
		brb.format("PatchManager Version:%s Build:%s Build-time:%s\n",
				version,
				build_number,
				build_timestamp);
		brb.appendSystemInfo();
		// load logfile
		URL log = getClass().getResource("/patchmanager.log");


		boolean logFileAppended = false;
		if (log != null) {
			try {
				brb.appendLogFile(new File(log.toURI()));
				logFileAppended = true;
			} catch (URISyntaxException ex) {
				// no op
			}
		}
		if (!logFileAppended) {
			brb.append("Could not locate log file.\n");
		}

		new BugreportForm( 
				getPatchManagerClient(),
				getUserAuthentication(false),
				brb.toString()).showDialog(frame);

	}

	/*
    private void updatePatchListViewFixedWidth() {

        int w = patchListView.getWidth();
        Insets insets = new Insets(0,0,0,0);
        insets = scrollPatchList.getInsets(insets);
        w-=insets.left+insets.right;
        insets = scrollPatchList.getViewport().getInsets(insets);
        w-=insets.left+insets.right;
        insets = patchListView.getInsets(insets);
        w-=insets.left+insets.right;
        w = Math.max(300, w);
        System.out.printf("cw:%d\n",w);
        patchListView.setFixedCellWidth(w);
    }*/

	private class EventHandler extends CSEventAdapter 
	implements ChangeListener,
	ListSelectionListener {

		public void install() {
			searchOptionsControl.addChangeListener(this);
			patchListView.addListSelectionListener(this);
			patchListView.addMouseListener(this);
			frame.addWindowListener(this);
		}

		@Override
		public void windowClosing(WindowEvent e) {
			writeAppProperties();
			executorService.shutdown();
		}

		@Override
		public void mouseReleased(MouseEvent e) {
			selectListItemIgnoreMouseButton(e);
		}

		@Override
		public void mousePressed(MouseEvent e) {
			selectListItemIgnoreMouseButton(e);
		}

		public void selectListItemIgnoreMouseButton(MouseEvent e) {
			if (e.getComponent() == patchListView) {
				int index = patchListView.locationToIndex(e.getPoint());
				if (patchListView.getSelectedIndex() != index) {
					patchListView.setSelectedIndex(index);

					if (e.isPopupTrigger()) { 
						// changing the selected list item may 
						// prevent the patchlist popup from showing
						// => force show popup menu
						JPopupMenu popup = patchListView.getComponentPopupMenu();
						if (popup != null) {
							popup.show(patchListView, e.getX(), e.getY());
						}
					}
				}
			}
		}

		@Override
		public void stateChanged(ChangeEvent e) {
			if (e.getSource() == SwingPatchManager.this.searchOptionsControl) {
				executeQuery();
			} 
		}

		@Override
		public void valueChanged(ListSelectionEvent e) {
			IndexedPatch patch = (IndexedPatch) patchListView.getSelectedValue();
			patchDetailsView.setPatch(patch);

			boolean midifileop = patch != null
			&& DefaultPatch.containsMidiFile(patch);
			boolean sourcecodeop = patch != null
			&& DefaultPatch.containsSource(patch);

			Auth auth = getUserAuthentication(false);
			boolean deleteop = patch != null
			&& auth != null
			&& auth.getUsername() != null
			&& (SwingPatchManager.adminMode || auth.getUsername().equals(patch.getMetadata().getAuthor()));

			acPatchListViewSendSelectedAction.setEnabled(midifileop);

			if (SwingPatchManager.adminMode == true) {
				acPatchListViewSelectedSaveMidiFileAction.setEnabled(midifileop);

				acPatchListViewDeleteSelectedAction.setEnabled(deleteop);
				acPatchListViewSelectedSaveSourceAction.setEnabled(sourcecodeop);
			}
		}

	} 

	public SearchOptionsControl getSearchOptionsControl() {
		return searchOptionsControl;
	}

	public PatchIndex getIndex() {
		return patchmanager.getIndex();
	}

	public ClientRepository getRepository() {
		return patchmanager.getRepository();
	}

	public PatchManager getPatchManager() {
		return patchmanager;
	}

	public String getQueryString() {
		return searchPanel.getQueryString();
	}

	public PatchListCellRenderer getPatchListCellRenderer() {
		return patchListCellRenderer;
	}

	public PatchListModel<IndexedPatch> getPatchListModel() {
		return patchListModel;
	}

	public JList getPatchListView() {
		return patchListView;
	}

	/**
	 * @param args
	 */
	 public static void main(String[] args) throws Exception {
		 if (args.length > 0) {
			 if (args[0].equals("-admin")) {
				 SwingPatchManager.adminMode = true;
			 }
		 }
		 // create application on the event dispatch thread as required by swing
		 SwingUtilities.invokeAndWait(new SwingPatchManagerStarter());
	 }

	 private static final class SwingPatchManagerStarter implements Runnable {
		 public void run() {
			 new SwingPatchManager().start();
		 }
	 }

}
