#pragma once

#include <Prelude.h>
#include _CH_VECTOR_

#define INPUT_STRING_SIZE 64


struct Face3 {
	Vector3 first = Vector3(0, 0, 0);
	Vector3 second = Vector3(0, 0, 0);
	Vector3 third = Vector3(0, 0, 0);
};

class Object3D {
private:
	struct FaceElement;
	struct Face;

	Vector3* vertex = nullptr;
	Vector3* texture = nullptr;
	Vector3* normal = nullptr;
	Face* face = nullptr;

	bool hasVertex = false;
	bool hasTexture = false;
	bool hasNormal = false;
	bool hasFace = false;

	Vector3 stringToVector3(std::deque<std::string> row);
	Face stringToFace(std::deque<Face> &que, std::deque<std::string> row);
	FaceElement stringToFaceElement(std::string str);
	void tokenizingRecursive(std::deque<std::string>& contents, int& slashCount, std::string str);
	template<typename T> void dequeToArray(std::deque<T> deq, T* arr);
	template<typename T> size_t arraySize(T* arr);

public:
	Object3D(std::string fileName);
	~Object3D();

	size_t faceSize();

	Face3 getVertex(int faceNumber);
	Face3 getTexture(int faceNumber);
	Face3 getNormal(int faceNumber);
};

struct Object3D::FaceElement {
	int v = 0;
	int vt = 0;
	int vn = 0;
};

struct Object3D::Face {
	FaceElement first;
	FaceElement second;
	FaceElement third;
};

Object3D::Object3D(std::string fileName) {
	std::ifstream file(fileName);
	if (!file) {
		std::cout << "There's no existing file" << std::endl;
		this->~Object3D();
	}
	else {
		std::deque<Vector3> vertexDeque;
		std::deque<Vector3> textureDeque;
		std::deque<Vector3> normalDeque;
		std::deque<Face> faceDeque;

		std::deque<std::string> row;

		char input_string[INPUT_STRING_SIZE];

		//while (!file.eof()) {
		while (!file.eof()) {
			
			if (!file.good()) {
				break;
			}
			file.getline(input_string, sizeof(input_string));
			std::string buffer;
			std::stringstream ss(input_string);
			while (ss >> buffer)
				row.push_back(buffer);

			if (!buffer.empty()) {
				if (row.front() == "v")
					vertexDeque.push_back(stringToVector3(row));
				else if (row.front() == "vn")
					normalDeque.push_back(stringToVector3(row));
				else if (row.front() == "vt")
					textureDeque.push_back(stringToVector3(row));
				else if (row.front() == "f")
				{
					stringToFace(faceDeque, row);
					//faceDeque.push_back(stringToFace(row));
					//faceDeque.push_back(stringToFaceQuad(row));
				}
			}

			row.clear();
		}

		if (vertexDeque.size() != 0) {
			hasVertex = true;
			vertex = new Vector3[vertexDeque.size()];
			dequeToArray(vertexDeque, vertex);
		}
		if (textureDeque.size() != 0) {
			hasTexture = true;
			texture = new Vector3[textureDeque.size()];
			dequeToArray(textureDeque, texture);
		}
		if (normalDeque.size() != 0) {
			hasNormal = true;
			normal = new Vector3[normalDeque.size()];
			dequeToArray(normalDeque, normal);
		}
		if (faceDeque.size() != 0) {
			hasFace = true;
			face = new Face[faceDeque.size()];
			dequeToArray(faceDeque, face);
		}
	}
}

Object3D ::~Object3D() {
	if (hasVertex)	delete[] vertex;
	if (hasTexture)	delete[] texture;
	if (hasNormal)	delete[] normal;
	if (hasFace)	delete[] face;
}

Vector3 Object3D::stringToVector3(std::deque<std::string> row) {
	row.pop_front();

	float x = atof(row.front().c_str());
	row.pop_front(); // remove x
	float y = atof(row.front().c_str());
	row.pop_front(); // remove y
	float z = atof(row.front().c_str());

	return Vector3(x, y, z);
}

Object3D::Face Object3D::stringToFace(std::deque<Face> &que, std::deque<std::string> row) {
	Face returnValue;

	row.pop_front();
	returnValue.first = stringToFaceElement(row.front().c_str());
	row.pop_front();
	returnValue.second = stringToFaceElement(row.front().c_str());
	row.pop_front();
	returnValue.third = stringToFaceElement(row.front().c_str());
	row.pop_front();
	que.push_back(returnValue);

	while (!row.empty())
	{//for Quad or more Polys
		returnValue.second = returnValue.third;
		returnValue.third = stringToFaceElement(row.front().c_str());
		row.pop_front();
		que.push_back(returnValue);
		
		std::cout << " Not Good OBJ file! : its face have 5 or upper number of vertex!" << std::endl;
	}
	return returnValue;
}

Object3D::FaceElement Object3D::stringToFaceElement(std::string str) {
	FaceElement returnValue;
	std::deque<std::string> test;
	int slashCount = 0;
	tokenizingRecursive(test, slashCount, str);

	switch (test.size()) {
	case 1: // v
		returnValue.v = atoi(test.front().c_str());
		test.clear();
		break;
	case 2:
		switch (slashCount) {
		case 1: // v/vt
			returnValue.v = atoi(test.front().c_str());
			test.pop_front();
			returnValue.vt = atoi(test.front().c_str());
			test.clear();
			break;
		case 2: // v/vn
			returnValue.v = atoi(test.front().c_str());
			test.pop_front();
			returnValue.vn = atoi(test.front().c_str());
			test.clear();
			break;
		}
		break;
	case 3: // v/vt/vn
		returnValue.v = atoi(test.front().c_str());
		test.pop_front();
		returnValue.vt = atoi(test.front().c_str());
		test.pop_front();
		returnValue.vn = atoi(test.front().c_str());
		test.clear();
		break;
	default:
		break;
	}

	return returnValue;
}

void Object3D::tokenizingRecursive(std::deque<std::string>& contents, int& slashCount, std::string str) {
	std::string delimiter = "/";
	size_t position = str.find(delimiter);
	if (position != std::string::npos) {
		slashCount++;
		contents.push_back(str.substr(0, position));
		tokenizingRecursive(contents, slashCount, str.substr(position + 1, str.length() - (position + 1)));
	}
	else {
		contents.push_back(str);
	}
}

template<typename T> void Object3D::dequeToArray(std::deque<T> deq, T* arr) {
	int initialSize = deq.size();
	for (int i = 0; i < initialSize; i++) {
		arr[i] = deq.front();
		deq.pop_front();
	}
}

size_t Object3D::faceSize() {
	if (hasFace) {
		return _msize(face) / sizeof(Face);
	}
	else {
		return 0;
	}
}

Face3 Object3D::getVertex(int faceNumber) {
	Face3 returnValue;
	if (hasVertex) {
		returnValue.first = vertex[face[faceNumber].first.v - 1];
		returnValue.second = vertex[face[faceNumber].second.v - 1];
		returnValue.third = vertex[face[faceNumber].third.v - 1];
		return returnValue;
	}
	else {
		return returnValue;
	}
}

Face3 Object3D::getTexture(int faceNumber) {
	Face3 returnValue;
	if (hasTexture) {
		returnValue.first = texture[face[faceNumber].first.vt - 1];
		returnValue.second = texture[face[faceNumber].second.vt - 1];
		returnValue.third = texture[face[faceNumber].third.vt - 1];
		return returnValue;
	}
	else {
		return returnValue;
	}
}

Face3 Object3D::getNormal(int faceNumber) {
	Face3 returnValue;
	if (hasNormal) {
		returnValue.first = normal[face[faceNumber].first.vn - 1];
		returnValue.second = normal[face[faceNumber].second.vn - 1];
		returnValue.third = normal[face[faceNumber].third.vn - 1];
		return returnValue;
	}
	else {
		return returnValue;
	}
}