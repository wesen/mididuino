package com.ruinwesen.patchmanager.cli;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

import name.cs.csutils.CSUtils;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.log4j.PropertyConfigurator;
import org.apache.log4j.config.PropertySetter;

import com.ruinwesen.patch.metadata.DefaultPatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.DeviceId;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.EnvironmentId;
import com.ruinwesen.patchmanager.client.PatchManager;
import com.ruinwesen.patchmanager.client.protocol.Auth;
import com.ruinwesen.patchmanager.client.protocol.PatchSource;
import com.ruinwesen.patchmanager.client.protocol.ProtocolException;
import com.ruinwesen.patchmanager.client.protocol.Request;
import com.ruinwesen.patchmanager.client.protocol.RequestApprovePatch;
import com.ruinwesen.patchmanager.client.protocol.RequestDeletePatch;
import com.ruinwesen.patchmanager.client.protocol.RequestGetPatchSourceList;
import com.ruinwesen.patchmanager.client.protocol.RequestGetServerInfo;
import com.ruinwesen.patchmanager.client.protocol.RequestStorePatch;
import com.ruinwesen.patchmanager.client.protocol.Response;
import com.ruinwesen.patchmanager.swing.tasks.DeletePatchTask;

public class CLIUtil {

	static void uploadPatch(Auth auth, String title, String tags, String documentationFile, 
			String deviceId, String environmentId, 
			String comment, String filename) throws Exception {
		PatchMetadata meta = new DefaultPatchMetadata();
		meta.setTitle(title);
		meta.setAuthor(auth.getUsername());
		
		meta.setComment(comment);
		meta.setDeviceId(new DeviceId(deviceId));
		meta.setEnvironmentId(new EnvironmentId(environmentId));

		File tmpPatchFile = File.createTempFile("patch-upload", ".tmp");
		tmpPatchFile.deleteOnExit();
		
		try {
			File mididataFile = new File(filename);
			String documentationText = "";
			if (documentationFile != null) {
				try {
					InputStream is = new FileInputStream(documentationFile);
					if (is != null) {
						ByteArrayOutputStream os = new ByteArrayOutputStream();
						CSUtils.copy(is, os);
						documentationText = new String(os.toByteArray());  
					}
				} catch (IOException ex) {
					// ignore
				}
			}

			InputStream is = null;
			RequestStorePatch.buildPatchFile(null, mididataFile, meta, documentationText, tmpPatchFile);
			is = new BufferedInputStream(new FileInputStream(tmpPatchFile));
			
			try {
				Request request = new RequestStorePatch(auth, meta, is);
				Response response = patchmanager.getPatchManagerClient().execute(request);

				if (!response.isOkStatus()) {
					throw new Exception("error: " + response.getMessage());
				} else {
					System.out.println("response " + response);
				}
				patchmanager.syncRepository(auth, null);
			} catch (Exception e) {
				System.out.println("exception " + e);
				e.printStackTrace();
			} finally {
				is.close();
			}

		} finally {
			tmpPatchFile.delete();
		}
	}

	public static void listPatches(Auth auth) {
		Response response = executeRequest(new RequestGetPatchSourceList("01/01/1970", auth));
		if (response != null) {
			if (response.isOkStatus()) {
				for (PatchSource patch : response.getPatchSourceList()) {
					System.out.println(patch.getPatchId() + "  "  
							+ patch.getMetadata().getLastModifiedDateString() + "  "
							+ patch.getMetadata().getTitle() + "\t" 
							// + " \"" + (patch.getMetadata().getTags()) + "\" "
							+ (patch.isDeleteFlagSet() ? "[deleted]" : ""));
				}
			}
		}
	}

	public static void approvePatch(Auth auth, String patchId) {
		Response response = executeRequest(new RequestApprovePatch(auth, patchId));
		if (response != null) {
			if (response.isOkStatus()) {
				System.out.println("approved patch " + patchId);
			} else {
				System.out.println("error while approving patch " + patchId);
			}
		}
	}

	public static void deletePatch(Auth auth, String patchId) {
		Response response = executeRequest(new RequestDeletePatch(auth, patchId));
		if (response != null) {
			if (response.isOkStatus()) {
				System.out.println("deleted patch " + patchId);
			} else {
				System.out.println("error while deleting patch " + patchId);
			}
		}
	}

	public static Response executeRequest(Request request) {
		Response response;
		try {
			response = patchmanager.getPatchManagerClient().execute(request);
			return response;
		} catch (ProtocolException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
	}

	static PatchManager patchmanager = null;

	public static void main(String[] args) {
		String username = null;
		String password = null;
		String since = "01/01/1970";
		String action = "server-info";
		String tags = "";
		String documentationFile = null;
		String comment = null;
		String environmentId = "mididuino-17";
		String deviceId = "minicommand";
		String title = null;

		int optind = 0;
		for (optind = 0; optind < args.length; optind++) {
			if (args[optind].equals("-username")) {
				username = args[++optind];
			} else if (args[optind].equals("-password")) {
				password = args[++optind];
			} else if (args[optind].equals("-since")) {
				since = args[++optind];
			} else if (args[optind].equals("-tags")) {
				tags = args[++optind];
			} else if (args[optind].equals("-doc")) {
				documentationFile = args[++optind];
			} else if (args[optind].equals("-environment")) {
				environmentId = args[++optind];
			} else if (args[optind].equals("-device")) {
				deviceId = args[++optind];
			} else if (args[optind].equals("-comment")) {
				comment = args[++optind];
			} else if (args[optind].equals("-title")) {
				title = args[++optind];
			} else {
				action = args[optind];
				break;
			}
		}

		patchmanager = new PatchManager(null);
		Auth auth = new Auth(username, password);

		if (action.equals("list")) {
			listPatches(auth);
		} else if (action.equals("approve")) {
			String patchId = args[++optind];
			approvePatch(auth, patchId);
		} else if (action.equals("approve")) {
			String patchId = args[++optind];
			deletePatch(auth, patchId);
		} else if (action.equals("upload")) {
			String filename = args[++optind];
			try {
				uploadPatch(auth, title, tags, documentationFile, deviceId, environmentId, comment, filename);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
}
