// Java wrapper functions

#include <windows.h>

#include "java.h"
#include "dragbase.h"

jobject save_proc;
JavaVM * jvm;


// Callback handler
int __stdcall SaveFile(const char * const filename, const char * const directory, void * param)
{
	JNIEnv *env = NULL;
	jvm->AttachCurrentThread((void**)&env,NULL); 
	// Get Save function
	jclass icls = env->GetObjectClass(save_proc);
	jmethodID jmid = env->GetMethodID(icls, "Save",//(Ljava/lang/String;Ljava/lang/String;)I
									"(Ljava/lang/String;Ljava/lang/String;)I");
	jstring jfilename = env->NewStringUTF(filename);
	jstring jdirectory = env->NewStringUTF(directory);

	// Call save function
	int ret =  env->CallIntMethod(save_proc, jmid, jfilename, jdirectory);
	jvm->DetachCurrentThread();
	return ret;
}


/*
 * Class:     dragbase
 * Method:    Create
 * Signature: (Ljava/awt/Frame;LSaveInterface;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_dragbase_Create_1
  (JNIEnv * env, jobject me, jstring window_title, jobject save_proc, jstring app_name)
{
	::save_proc = env->NewGlobalRef(save_proc);
	env->GetJavaVM(&jvm);

	// Convert java string to c string
	const char * cstr =  env->GetStringUTFChars(window_title, 0);
	SetFilename(cstr);

	// Get hwnd by window title
	HWND hwnd = FindWindow(NULL,cstr);
	if(hwnd == NULL)
		return 1001;

	// Convert java string to c string
	cstr = env->GetStringUTFChars(app_name, 0);

	// Create dragbase
	return Create(hwnd,SaveFile,cstr,NULL);
}

/*
 * Class:     dragbase
 * Method:    SetFilename
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_dragbase_SetFilename
  (JNIEnv * jenv, jobject me, jstring filename)
{
	// Convert java string to c string
	const char * cstr = jenv->GetStringUTFChars(filename, 0);
	SetFilename(cstr);
}
