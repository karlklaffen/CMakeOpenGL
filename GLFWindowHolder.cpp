//#include "GLFWindowHolder.h"
//
//GLFWindowHolder::GLFWindowHolder(std::string_view title, unsigned short w, unsigned short h, unsigned short versionMajor, unsigned short versionMinor) : Window(nullptr) /*WindowWidth(w), WindowHeight(h),*/
///*Initializable("GLFW Window (Title = " + std::string(title) + ", Width = " + std::to_string(w) + ", Height = " + std::to_string(h) + ")")*/ {
//	std::cout << "Initializing GLFW" << std::endl;
//	glfwInit();
//
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//	Window = glfwCreateWindow(w, h, title.data(), NULL, NULL);
//
//	if (Window == nullptr) {
//		std::cout << "ERROR" << std::endl;
//		return;
//	}
//
//	glfwMakeContextCurrent(Window);
//
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//		std::cout << "ERROR" << std::endl;
//		return;
//	}
//
//	//ConfirmInitialization();
//}
//
//GLFWindowHolder::~GLFWindowHolder() {
//	std::cout << "Terminating GLFW" << std::endl;
//	glfwTerminate();
//}
//
//GLFWwindow* GLFWindowHolder::GetWindow() const {
//	return Window;
//}
//
////unsigned int GLFWindowHolder::GetWindowWidth() const {
////	return WindowWidth;
////}
////
////unsigned int GLFWindowHolder::GetWindowHeight() const {
////	return WindowHeight;
////}
////
////void GLFWindowHolder::SetWindowWidth(unsigned int w) {
////	WindowWidth = w;
////}
////
////void GLFWindowHolder::SetWindowHeight(unsigned int h) {
////	WindowHeight = h;
////}