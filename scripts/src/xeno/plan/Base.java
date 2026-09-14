package xeno.plan;

import xeno.Camera;
import xeno.Stage;
import xeno.XenoConstants;

public class Base
        implements XenoConstants {
    public static void CameraFixed(Camera camera, float f, float f2, float f3, float f4, float f5, float f6) {
        camera.setTranslate(f, f2, f3);
        camera.setRotate(f4, f5, f6);
    }

    public static void SceneCreate(Stage stage, int n) {
    }
}

