#include "ModernGL.hpp"

#include "OpenGL.hpp"

PyObject * NewVertexArray(PyObject * self, PyObject * args) {
	Program * program;
	VertexBuffer * vbo;
	const char * format;
	PyObject * attributes;

	IndexBuffer * no_ibo = (IndexBuffer *)Py_None;
	IndexBuffer * ibo = no_ibo;
	bool strict = false;

	if (!PyArg_ParseTuple(args, "O!O!sO!|O!p:NewVertexArray", &ProgramType, &program, &VertexBufferType, &vbo, &format, &PyList_Type, &attributes, &IndexBufferType, &ibo, &strict)) {
		return 0;
	}

	int length = 0;
	while (format[length]) {
		if (length % 2 == 0) {
			if (format[length] < '1' || format[length] > '4') {
				PyErr_SetString(ModuleInvalidFormat, "NewVertexArray() argument `format` is invalid.");
				return 0;
			}
		} else {
			if (format[length] != 'i' && format[length] != 'f') {
				PyErr_SetString(ModuleInvalidFormat, "NewVertexArray() argument `format` is invalid.");
				return 0;
			}
		}
		++length;
	}

	if (!length || length % 2) {
		PyErr_SetString(ModuleInvalidFormat, "NewVertexArray() argument `format` is empty or invalid.");
		return 0;
	}

	int count = (int)PyList_Size(attributes);

	if (length / 2 != count) {
		PyErr_Format(ModuleInvalidFormat, "NewVertexArray() size of `format` is %d, length of `attributes` is %d", length / 2, count);
		return 0;
	}

	for (int i = 0; i < count; ++i) {
		const char * name = PyUnicode_AsUTF8(PyList_GET_ITEM(attributes, i));
		int location = OpenGL::glGetAttribLocation(program->program, name);
		if (strict && location < 0) {
			PyErr_Format(ModuleAttributeNotFound, "NewVertexArray() attribute `%s` not found", name);
			return 0;
		}
	}

	int vao = 0;
	OpenGL::glGenVertexArrays(1, (OpenGL::GLuint *)&vao);
	OpenGL::glBindVertexArray(vao);

	if (ibo != no_ibo) {
		OpenGL::glBindBuffer(OpenGL::GL_ELEMENT_ARRAY_BUFFER, ibo->ibo);
	}

	int stride = 0;
	for (int i = 0; format[i]; i += 2) {
		stride += (format[i] - '0') * 4;
	}

	OpenGL::glBindBuffer(OpenGL::GL_ARRAY_BUFFER, vbo->vbo);

	char * ptr = 0;
	for (int i = 0; i < count; ++i) {
		const char * name = PyUnicode_AsUTF8(PyList_GET_ITEM(attributes, i));
		int location = OpenGL::glGetAttribLocation(program->program, name);

		int dimension = format[i * 2] - '0';
		switch (format[i * 2 + 1]) {
			case 'f':
				OpenGL::glVertexAttribPointer(location, dimension, OpenGL::GL_FLOAT, false, stride, ptr);
				break;
			case 'i':
				OpenGL::glVertexAttribPointer(location, dimension, OpenGL::GL_INT, false, stride, ptr);
				break;
		}
		OpenGL::glEnableVertexAttribArray(location);
		ptr += dimension * 4;
	}

	OpenGL::glBindVertexArray(defaultVertexArray);
	return CreateVertexArrayType(vao, program->program, ibo != no_ibo);
}

PyObject * NewAdvancedVertexArray(PyObject * self, PyObject * args) {
	Program * program;
	PyObject * content;

	IndexBuffer * no_ibo = (IndexBuffer *)Py_None;
	IndexBuffer * ibo = no_ibo;
	bool strict = false;

	if (!PyArg_ParseTuple(args, "O!O!|O!p:NewAdvancedVertexArray", &ProgramType, &program, &PyList_Type, &content, &IndexBufferType, &ibo)) {
		return 0;
	}

	int vao = 0;
	OpenGL::glGenVertexArrays(1, (OpenGL::GLuint *)&vao);
	OpenGL::glBindVertexArray(vao);

	if (ibo != no_ibo) {
		OpenGL::glBindBuffer(OpenGL::GL_ELEMENT_ARRAY_BUFFER, ibo->ibo);
	}

	int size = PyList_Size(content);
	for (int k = 0; k < size; ++k) {
		PyObject * tuple = PyList_GET_ITEM(content, k);
		CHECK_AND_REPORT_ELEMENT_TYPE_ERROR("content", tuple, PyTuple_Type, k);
		if (PyTuple_Size(tuple) != 3) {
			PyErr_Format(PyExc_TypeError, "NewAdvancedVertexArray() fail");
			return 0;
		}

		VertexBuffer * vbo = (VertexBuffer *)PyTuple_GET_ITEM(tuple, 0);
		const char * format = PyUnicode_AsUTF8(PyTuple_GET_ITEM(tuple, 1));
		PyObject * attributes = PyTuple_GET_ITEM(tuple, 2);

		if (!CHECK_TYPE_ERROR(vbo, VertexBufferType)) {
			PyErr_Format(PyExc_TypeError, "NewAdvancedVertexArray() fail");
			return 0;
		}

		if (!CHECK_TYPE_ERROR(attributes, PyList_Type)) {
			PyErr_Format(PyExc_TypeError, "NewAdvancedVertexArray() fail");
			return 0;
		}

		int length = 0;
		while (format[length]) {
			if (length % 2 == 0) {
				if (format[length] < '1' || format[length] > '4') {
					PyErr_SetString(ModuleInvalidFormat, "NewAdvancedVertexArray() ERR 3");
					return 0;
				}
			} else {
				if (format[length] != 'i' && format[length] != 'f') {
					PyErr_SetString(ModuleInvalidFormat, "NewAdvancedVertexArray() ERR 3");
					return 0;
				}
			}
			++length;
		}

		if (!length || length % 2) {
			PyErr_SetString(ModuleInvalidFormat, "NewAdvancedVertexArray() ERR 3");
			return 0;
		}

		int stride = 0;
		for (int i = 0; format[i]; i += 2) {
			stride += (format[i] - '0') * 4;
		}

		int count = (int)PyList_Size(attributes);
		if (length / 2 != count) {
			PyErr_Format(ModuleInvalidFormat, "NewAdvancedVertexArray() ERR 4.");
			return 0;
		}

		char * ptr = 0;
		for (int i = 0; i < count; ++i) {
			const char * name = PyUnicode_AsUTF8(PyList_GET_ITEM(attributes, i));
			int location = OpenGL::glGetAttribLocation(program->program, name);

			int dimension = format[i * 2] - '0';
			switch (format[i * 2 + 1]) {
				case 'f':
					OpenGL::glVertexAttribPointer(location, dimension, OpenGL::GL_FLOAT, false, stride, ptr);
					break;
				case 'i':
					OpenGL::glVertexAttribPointer(location, dimension, OpenGL::GL_INT, false, stride, ptr);
					break;
			}
			OpenGL::glEnableVertexAttribArray(location);
			ptr += dimension * 4;
		}
	}

	OpenGL::glBindVertexArray(defaultVertexArray);
	return CreateVertexArrayType(vao, program->program, ibo != no_ibo);
}

PyObject * DeleteVertexArray(PyObject * self, PyObject * args) {
	VertexArray * vao;

	if (!PyArg_ParseTuple(args, "O!:DeleteVertexArray", &VertexArrayType, &vao)) {
		return 0;
	}

	OpenGL::glDeleteVertexArrays(1, (OpenGL::GLuint *)&vao->vao);
	Py_RETURN_NONE;
}

PyObject * EnableAttributes(PyObject * self, PyObject * args, PyObject * kwargs) {
	VertexArray * vao;
	PyObject * attributes;
	bool strict = false;

	static const char * kwlist[] = {"vao", "attributes", "strict", 0};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!|p:EnableAttributes", (char **)kwlist, &VertexArrayType, &vao, &PyList_Type, &attributes, &strict)) {
		return 0;
	}

	int count = (int)PyList_Size(attributes);
	for (int i = 0; i < count; ++i) {
		const char * name = PyUnicode_AsUTF8(PyList_GET_ITEM(attributes, i));
		int location = OpenGL::glGetAttribLocation(vao->vao, name);
		if (strict && location < 0) {
			PyErr_Format(ModuleError, "ERR: 1");
			return 0;
		}
	}
	
	OpenGL::glBindVertexArray(vao->vao);
	for (int i = 0; i < count; ++i) {
		const char * name = PyUnicode_AsUTF8(PyList_GET_ITEM(attributes, i));
		int location = OpenGL::glGetAttribLocation(vao->program, name);
		OpenGL::glEnableVertexAttribArray(location);
	}

	OpenGL::glBindVertexArray(defaultVertexArray);
	Py_RETURN_NONE;
}

PyObject * DisableAttributes(PyObject * self, PyObject * args, PyObject * kwargs) {
	VertexArray * vao;
	PyObject * attributes;
	bool strict = false;

	static const char * kwlist[] = {"vao", "attributes", "strict", 0};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!|p:DisableAttributes", (char **)kwlist, &VertexArrayType, &vao, &PyList_Type, &attributes, &strict)) {
		return 0;
	}

	int count = (int)PyList_Size(attributes);
	for (int i = 0; i < count; ++i) {
		const char * name = PyUnicode_AsUTF8(PyList_GET_ITEM(attributes, i));
		int location = OpenGL::glGetAttribLocation(vao->program, name);
		if (strict && location < 0) {
			PyErr_Format(ModuleError, "ERR: 1");
			return 0;
		}
	}
	
	OpenGL::glBindVertexArray(vao->vao);
	for (int i = 0; i < count; ++i) {
		const char * name = PyUnicode_AsUTF8(PyList_GET_ITEM(attributes, i));
		int location = OpenGL::glGetAttribLocation(vao->program, name);
		OpenGL::glDisableVertexAttribArray(location);
	}

	OpenGL::glBindVertexArray(defaultVertexArray);
	Py_RETURN_NONE;
}
