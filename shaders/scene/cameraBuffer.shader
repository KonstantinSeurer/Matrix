
struct CameraTransform {
	mat4 view;
	mat4 invView;
	
	mat4 projection;
	mat4 invProjection;
	
	mat4 viewProjection;
	mat4 invViewProjection;
}

set cameraBuffer {
	storageBuffer STANDARD {
		CameraTransform[1000] transforms;
	} cameras;
}

function CameraTransform getCamera(int index) {
	return cameraBuffer.cameras.transforms[index];
}
