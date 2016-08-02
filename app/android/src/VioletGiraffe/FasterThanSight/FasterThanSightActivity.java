package org.VioletGiraffe.FasterThanSight;

import org.qtproject.qt5.android.bindings.QtActivity;
import android.os.*;
import android.content.*;
import android.app.*;
import android.net.Uri;
import android.util.Log;

public class FasterThanSightActivity extends QtActivity {

	public static native void openTextFile(String fileName);

	@Override
	public void onCreate (Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Intent intent = getIntent();
		handleIntent(intent);
	}

	@Override
	public void onNewIntent(Intent intent) {
		super.onNewIntent(intent);

		handleIntent(intent);
	}

	void handleIntent(Intent intent) {
		if (intent == null)
			return;

		String action = intent.getAction();
		String type = intent.getType();

		if (action != null)
			Log.d("FasterThanSight.handleIntent", "Intent action: " + action + ", type: " + type);

		if (Intent.ACTION_VIEW.equals(action) && type != null && "text/plain".equals(type)) {
			Uri uri = intent.getData();
			openTextFile(uri.getPath().toString());
		}
	}
}
