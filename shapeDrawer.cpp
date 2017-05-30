#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <fstream>

enum class ShapeType { Undefined, Square, Triangle, Circle };

template <typename T>
std::ostream & operator<<(std::ostream&  os, const std::vector<T>& vec) {
	std::for_each(std::begin(vec), std::end(vec), [&](const T& el) { os << el << " ";  });
	return os;
}

struct ShapeParameters {
	ShapeType type = ShapeType::Undefined;
	std::vector<double> coordinates;
};

class Shape {
protected:
	std::vector<double> coordinates;
public:
	Shape(const std::vector<double>& coordinates) : coordinates(coordinates) { }
	virtual void draw() const = 0;
	virtual ~Shape() = default;
};

class Circle : public Shape {
public:
	Circle(const std::vector<double>& coordinates) : Shape(coordinates) {}
	void draw() const override {
		std::cout << "Circle drawing: [" << coordinates << "]" << std::endl;
	}
};

class Square : public Shape {
public:
	Square(const std::vector<double>& coordinates) : Shape(coordinates) {}
	void draw() const override {
		std::cout << "Square drawing: [" << coordinates << "]" << std::endl;
	}
};

class Triangle : public Shape {
public:
	Triangle(const std::vector<double>& coordinates) : Shape(coordinates) {}
	void draw() const override {
		std::cout << "Triangle drawing: [" << coordinates << "]" << std::endl;
	}
};

class ParameterReader {
	std::string parameters;
	const std::size_t sizeOfType = 4;
	const std::size_t sizeOfPoint = 8;

	ShapeType shapeTypeFromInt(int num) const {
		auto type = ShapeType::Undefined;;
		switch (num) {
		case 1: type = ShapeType::Circle;
			break;
		case 2: type = ShapeType::Triangle;
			break;
		case 3: type = ShapeType::Square;
			break;
		default:
			type = ShapeType::Undefined;
		}
		return type;
	}

	std::vector<double> getCoordinatesFromString(const std::string& str) const {
		std::vector<double> coordinates;
		std::size_t pos = 0;
		while (pos < str.size()) {
			auto coord = std::stod(str.substr(pos, sizeOfPoint));
			coordinates.push_back(coord);
			pos += sizeOfPoint;
		}
		return coordinates;
	}

	ShapeType getShapeType(const std::string& parameters) const {
		std::string type = parameters.substr(0, sizeOfType);
		return shapeTypeFromInt(std::stoi(type));
	}

	void validateParametersString(const std::string& parameters) const {
		auto size = parameters.size();
		auto minimalSize = sizeOfType + sizeOfPoint;
		if (size < minimalSize) {
			throw std::invalid_argument("Invalid parameters, must be [Type] and [Points]");
		}
		auto sizeOfPoints = size - sizeOfType;
		if (sizeOfPoints % sizeOfPoint != 0) {
			throw std::invalid_argument("Invalid number of points");
		}
		auto type_str = parameters.substr(0, sizeOfType);
		auto shapeType = std::stoi(type_str);
		if (shapeTypeFromInt(shapeType) == ShapeType::Undefined){
			throw std::invalid_argument("Invalid Shape Type");
		}
	}

public:
	ShapeParameters getParameters() const {
		ShapeParameters params;
		params.type = getShapeType(this->parameters);
		params.coordinates = getCoordinatesFromString(parameters.substr(sizeOfType));
		return params;
	}

	void setParameters(const std::string & params) {
		validateParametersString(params);
		parameters = params;
	}
};

class ShapeFactory {
	Circle* makeCircle(const std::vector<double>& coordinates) const {
		return new Circle(coordinates);
	}

	Triangle* makeTriangle(const std::vector<double>& coordinates) const {
		return new Triangle(coordinates);
	}

	Square* makeSquare(const std::vector<double>& coordinates) const {
		return new Square(coordinates);
	}

	ShapeFactory() = default;

public:
	ShapeFactory(const ShapeFactory&) = delete;
	ShapeFactory& operator=(const ShapeFactory&) = delete;

	static ShapeFactory & instance() {
		static ShapeFactory factory;
		return factory;
	}

	Shape* makeShape(ShapeType shapeType, const std::vector<double> coordinates) const {
		Shape* result = nullptr;
		switch (shapeType) {
		case ShapeType::Circle:
			result = makeCircle(coordinates);
			break;
		case ShapeType::Square:
			result = makeSquare(coordinates);
			break;
		case ShapeType::Triangle:
			result = makeTriangle(coordinates);
			break;
		default:
			;
		}	
		return result;
	}
};

class ShapeDrawer {
	ParameterReader parametersReader;
	std::vector<std::unique_ptr<Shape>> shapes;
public:
	void readShapes(const std::string& fileName) {
		std::ifstream infile(fileName);
		if (infile.is_open()) {		
			ShapeFactory& shapeFactory = ShapeFactory::instance();
			std::string line;
			while (std::getline(infile, line)) {
				try {
					parametersReader.setParameters(line);
					auto params = parametersReader.getParameters();
					shapes.push_back(std::unique_ptr<Shape>(shapeFactory.makeShape(params.type, params.coordinates)));
				}
				catch (std::exception & ex) {
					std::cout << "Error in reading shape: " << ex.what() << std::endl;
				}
			}
		}
		else {
			std::cout << "Can't open file: " << fileName << std::endl;
		}
	}

	void drawAll() const {
		for(const auto& el:shapes) {
			el->draw();
		}
	}
};

int main(int argc, char* argv[])
{
	
	ShapeDrawer drawer;
	drawer.readShapes("shapes.txt");
	drawer.drawAll();
	
	return 0;
}
