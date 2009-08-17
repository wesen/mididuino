package com.ruinwesen.patchmanager.client.protocol;

public class MandatoryAuthenticatingRequest extends AuthenticatingRequest {

	public MandatoryAuthenticatingRequest(String protocolId, String action,
			Auth auth) {
		super(protocolId, action, auth);
		if (auth == null) {
            throw new IllegalArgumentException("auth:"+auth);
        }
	}

}
