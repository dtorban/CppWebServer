public class WebServer
{
	private native long init();
	private native void close(long state);

	private long state;

	public WebServer() { 
		System.loadLibrary("CppWSJavaAdapter");
		//state = init(filePathPrefix);
		state = init();
	}

	public void close() {
		close(state);
	}
}
