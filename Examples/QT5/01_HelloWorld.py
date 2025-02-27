# python -m pip install ModernGL

from PyQt5 import QtOpenGL, QtWidgets
import ModernGL as GL
import struct

context = {
	'width' : 800,
	'height' : 600,
}

class QGLControllerWidget(QtOpenGL.QGLWidget):
	def __init__(self, format = None):
		super(QGLControllerWidget, self).__init__(format, None)

	def initializeGL(self):
		try:
			GL.InitializeModernGL()
			GL.Viewport(0, 0, context['width'], context['height'])

			vert = GL.NewVertexShader('''
				#version 330

				in vec2 vert;

				void main() {
					gl_Position = vec4(vert, 0.0, 1.0);
				}
			''')

			frag = GL.NewFragmentShader('''
				#version 330
				
				out vec4 color;

				void main() {
					color = vec4(0.30, 0.50, 1.00, 1.0);
				}
			''')

			prog, iface = GL.NewProgram([vert, frag])
			vbo = GL.NewVertexBuffer(struct.pack('6f', 0.0, 0.8, -0.6, -0.8, 0.6, -0.8))
			context['vao'] = GL.NewVertexArray(prog, vbo, '2f', ['vert'])

		except GL.Error as error:
			print(error)
			exit(1)

	def paintGL(self):
		GL.Clear(240, 240, 240)
		if 'vao' in context:
			GL.RenderTriangles(context['vao'], 3)

class GLCanvas(QtWidgets.QMainWindow):
	def __init__(self, parent = None):
		super(GLCanvas, self).__init__(parent)

		fmt = QtOpenGL.QGLFormat()
		fmt.setVersion(3, 3)
		fmt.setProfile(QtOpenGL.QGLFormat.CoreProfile)
		fmt.setSampleBuffers(True)

		self.setFixedSize(context['width'], context['height'])
		self.widget = QGLControllerWidget(fmt)
		self.setCentralWidget(self.widget)
		self.show()

app = QtWidgets.QApplication([])
window = GLCanvas()
window.show()
app.exec_()
